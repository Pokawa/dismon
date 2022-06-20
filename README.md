# Dismon

Distributed monitor created using ØMQ and C++20. Gtest was used for unit tests and Spdlog for logging.

## Algorithm

Mutual exclusion was achieved using Ricart-Agrawala algorithm. Monitor supports one lock and unlimited condition
variables. Dismon provides easy to use syncing interface to use with shared data.

### lock

Broadcast REQUEST signal, and wait for all replies. When all replies received perform data synchronisation if
applicable. Upon REQUEST if receiving monitor is inactive or requesting with lower priority it responds immediately with
REPLY signal, otherwise adds REQUEST to deferred set.

### unlock

Increment current version number of shared data and send replies to all deferred requests.

### wait

Broadcast WAIT message with variable name. Unlock monitor and wait for NOTIFY signal. Upon Notify signal broadcast
UNWAIT message with variable name and perform lock action. Upon WAIT / UNWAIT message receiving monitor adds / removes
sender to / from variable queue.

### notify one / all

Send NOTIFY signal to first client / all clients on variable queue.

### sync

Broadcast SYNC signal, and wait for all STATE responses. STATE response consist of version number and serialised state.
If STATE with the highest version number has higher version that local version then apply it locally.

## Compilation

```bash
git clone https://github.com/Pokawa/dismon.git
cd dismon
cmake -S . -B build
cmake --build build
```

## Project tree

```
dismon
├── example (example codes)
│   ├── locking
│   ├── pinging
│   ├── syncing
│   └── waiting
├── include (header files)
│   └── dismon
│       ├── communication
│       └── serialization 
└── test (test files)
    ├── communication
    └── serialization
```

## Examples

All examples can be examined with differed logging levels:

- trace
- debug
- info

### pinging

Example of distributed network connecting and then broadcasting ping message. Example shows how network waits for all
clients and applies predefined PING callback.

```bash
pinging my_addr [other_addr]...
```

### locking

Example of clients working in mutual exclusion. Example of lock and unlock actions.

```bash
locking my_addr [other_addr]...
```

### waiting

Example of client waiting on condition variable nad other client notifying it. Example of wait and notify actions.

```bash
waiter my_addr [other_addr]...
notifier my_addr [other_addr]...
```

### syncing

Example of producer-consumer scenario with shared buffer. Shows condition variable in action and that synchronisation is
transparent for user.

```bash
consumer my_addr [other_addr]...
producer my_addr [other_addr]... start_num
```

