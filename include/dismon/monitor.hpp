#ifndef DISMON_MONITOR_HPP
#define DISMON_MONITOR_HPP

#include <dismon/serialization/serializable.hpp>
#include <dismon/communication/communication_server.hpp>
#include <tuple>

namespace dismon {

    class monitor {
        // communication related
        zmq::context_t context;
        communication_server server;

        // lock related
        std::set<client_id> deferred;
        std::set<client_id> replied;

        client_id clients;
        message_id requesting_id{};

        // local synchronisation related
        bool requesting{false};
        bool executing{false};

        std::mutex mutex;
        std::condition_variable lock_var;
        std::condition_variable wait_var;

        // condition variables related
        std::map<std::string, std::set<client_id>> cond_vars;

        // synced object related
        bool syncing;
        serializable *synced_object;
        version_id current_version_number{0};
        std::set<std::tuple<version_id, client_id, binary_data>> received_states;
        std::condition_variable sync_var;

    public:
        monitor(serializable &object, const std::string &self, const std::set<std::string> &others,
                std::uint16_t port) : server(
                context, self, others, port), clients(others.size()), synced_object(&object), syncing(true) {
            default_callbacks();
            server.start();
        }

        monitor(const std::string &self, const std::set<std::string> &others, std::uint16_t port) : server(
                context, self, others, port), clients(others.size()), synced_object(nullptr), syncing(false) {
            default_callbacks();
            server.start();
        }

        void lock() {
            std::unique_lock lock{mutex};
            lock_routine(lock);
        }

        void unlock() {
            std::lock_guard lock{mutex};
            if (executing) {
                // bumping up version
                current_version_number += 1;

                // sending replies to deferred requests
                if (not deferred.empty()) {
                    server.multicast(deferred, MESSAGE_TYPE::REPLY, {});
                    spdlog::debug("sending REPLY to {}", deferred);
                    deferred.clear();
                }

                // unlocked
                spdlog::info("unlocked");
                executing = false;
            }
        }

        void wait(const std::string &variable_name) {
            std::unique_lock lock{mutex};
            if (executing) {
                auto payload = from_string(variable_name);

                // broadcasting wait
                server.broadcast(MESSAGE_TYPE::WAIT, payload);
                spdlog::debug("sending WAIT on {}", variable_name);

                // unlocking obtained lock
                auto unlocker = std::thread(&monitor::unlock, this);

                // waiting for notify
                spdlog::info("waiting");
                wait_var.wait(lock);
                spdlog::info("notified");

                unlocker.join();

                // broadcasting unwait
                server.broadcast(MESSAGE_TYPE::UNWAIT, payload);
                spdlog::debug("sending UNWAIT on {}", variable_name);

                // obtaining lock
                lock_routine(lock);
            }
        }

        template <class Rep, class Period>
        void wait_for(const std::string& variable_name, const std::chrono::duration<Rep, Period>& rel_time) {
            std::unique_lock lock{mutex};
            if (executing) {
                auto payload = from_string(variable_name);

                // broadcasting wait
                server.broadcast(MESSAGE_TYPE::WAIT, payload);
                spdlog::debug("sending WAIT on {}", variable_name);

                // unlocking obtained lock
                auto unlocker = std::thread(&monitor::unlock, this);

                // waiting for notify
                spdlog::info("waiting");
                wait_var.wait_for(lock, rel_time);
                spdlog::info("notified");

                unlocker.join();

                // broadcasting unwait
                server.broadcast(MESSAGE_TYPE::UNWAIT, payload);
                spdlog::debug("sending UNWAIT on {}", variable_name);

                // obtaining lock
                lock_routine(lock);
            }
        }

        void notify_one(const std::string &variable_name) {
            std::lock_guard lock{mutex};
            if (not cond_vars[variable_name].empty()) {
                auto client = *cond_vars[variable_name].begin();
                spdlog::debug("sending NOTIFY to {}", client);
                server.unicast(client, MESSAGE_TYPE::NOTIFY, {});
                spdlog::info("notifying one");
            }
        }

        void notify_all(const std::string &variable_name) {
            std::lock_guard lock{mutex};
            if (not cond_vars[variable_name].empty()) {
                spdlog::debug("sending NOTIFY to {}", cond_vars[variable_name]);
                server.multicast(cond_vars[variable_name], MESSAGE_TYPE::NOTIFY, {});
                spdlog::info("notifying all");
            }
        }

    private:

        void lock_routine(std::unique_lock<std::mutex> &lock) {
            if (not requesting) {
                // sending request message
                requesting = true;

                requesting_id = server.broadcast(MESSAGE_TYPE::REQUEST, {});
                spdlog::debug("sending REQUEST(id={})", requesting_id);

                // waiting for all replies
                while (replied.size() != clients) {
                    lock_var.wait(lock);
                }

                // lock obtained
                spdlog::info("locked");
                replied.clear();
                executing = true;
                requesting = false;

                if (syncing) {
                    sync_routine(lock);
                }
            }
        }

        void sync_routine(std::unique_lock<std::mutex> &lock) {
            server.broadcast(MESSAGE_TYPE::SYNC, {});
            spdlog::debug("sending SYNC", requesting_id);

            sync_var.wait(lock);

            auto [version, sender, state] = *received_states.rbegin();
            if (version > current_version_number) {
                current_version_number = version;
                ibinary_stream stream{state};
                stream >> *synced_object;
                spdlog::info("synced");
                spdlog::debug("current state from {} version {}", sender, current_version_number);
            }
            received_states.clear();
        }

        void default_callbacks() {
            // REQUEST
            auto request_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                if (is_inactive() or has_lower_priority(m.header.sender, m.header.id)) {
                    spdlog::debug("sending REPLY to {}", m.header.sender);
                    server.unicast(m.header.sender, MESSAGE_TYPE::REPLY, {});
                } else {
                    spdlog::debug("deferring REQUEST from {}", m.header.sender);
                    deferred.insert(m.header.sender);
                }
            };
            server.on_message(MESSAGE_TYPE::REQUEST, std::move(request_message_callback));

            // REPLY
            auto reply_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                spdlog::debug("received REPLY from {}", m.header.sender);
                replied.insert(m.header.sender);
                if (replied.size() == clients) {
                    lock_var.notify_one();
                }
            };
            server.on_message(MESSAGE_TYPE::REPLY, std::move(reply_message_callback));

            // WAIT
            auto wait_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                auto variable_name = to_string(m.payload);
                cond_vars[variable_name].insert(m.header.sender);
                spdlog::debug("received WAIT from {} on {}", m.header.sender, variable_name);
            };
            server.on_message(MESSAGE_TYPE::WAIT, std::move(wait_message_callback));

            // UNWAIT
            auto unwait_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                auto variable_name = to_string(m.payload);
                cond_vars[variable_name].erase(m.header.sender);
                spdlog::debug("received UNWAIT from {} on {}", m.header.sender, variable_name);
            };
            server.on_message(MESSAGE_TYPE::UNWAIT, std::move(unwait_message_callback));

            // NOTIFY
            auto notify_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                spdlog::debug("received NOTIFY from {}", m.header.sender);
                wait_var.notify_one();
            };
            server.on_message(MESSAGE_TYPE::NOTIFY, std::move(notify_message_callback));

            // SYNC
            auto sync_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                obinary_stream stream;
                stream << current_version_number << *synced_object;
                server.unicast(m.header.sender, MESSAGE_TYPE::STATE, stream.data());
                spdlog::debug("received SYNC from {}", m.header.sender);
            };
            server.on_message(MESSAGE_TYPE::SYNC, std::move(sync_message_callback));

            // STATE
            auto state_message_callback = [this](const message &m) {
                std::lock_guard lock{mutex};
                version_id version;
                ibinary_stream(m.payload) >> version;
                binary_data state{m.payload.begin() + sizeof(version_id), m.payload.end()};
                received_states.emplace(version, m.header.sender, state);
                spdlog::debug("received SATE from {} version {}", m.header.sender, version);

                if (received_states.size() == clients) {
                    sync_var.notify_one();
                }
            };
            server.on_message(MESSAGE_TYPE::STATE, std::move(state_message_callback));
        }

        [[nodiscard]] bool is_inactive() const {
            return not(requesting or executing);
        }

        [[nodiscard]] bool has_lower_priority(client_id other_client_id, message_id other_message_id) const {
            return requesting and (other_message_id < requesting_id or
                                   (other_message_id == requesting_id and other_client_id < server.self_id));
        }
    };

}

#endif //DISMON_MONITOR_HPP
