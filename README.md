# Dismon

Distributed monitor created using ØMQ and C++20. Gtest was used for unit tests and Spdlog for logging.

## Algorithm

Mutual exclusion was achieved using Ricart-Agrawala algorithm. Monitor supports one lock and unlimited condition
variables. Dismon provides easy to use syncing interface to use with shared data.

### lock

Broadcast _REQUEST_ signal, and wait for all replies. When all replies received perform data synchronisation if
applicable. Upon _REQUEST_ if receiving monitor is inactive or requesting with lower priority it responds immediately with
_REPLY_ signal, otherwise adds _REQUEST__ to deferred set.

### unlock

Increment current version number of shared data and send replies to all deferred requests.

### wait

Broadcast _WAIT_ message with variable name. Unlock monitor and wait for _NOTIFY_ signal. Upon _NOTIFY_ signal broadcast
UNWAIT message with variable name and perform lock action. Upon _WAIT_ / _UNWAIT_ message receiving monitor adds / removes
sender to / from variable queue.

### notify one / all

Send _NOTIFY_ signal to first client / all clients on variable queue.

### sync

Broadcast _SYNC_ signal, and wait for all _STATE_ responses. _STATE_ response consist of version number and serialised state.
If _STATE_ with the highest version number has higher version that local version then apply it locally.

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
## TODO

- [ ] Simplify syncing mechanism
- [ ] Add network configuration struct
- [ ] Add support for mutiple mutexes and syncing objects
- [ ] Create standalone classes of mutex nad conditional variable
- [ ] Add conditional critical regions
