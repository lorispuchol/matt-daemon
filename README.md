# matt-daemon

TODO:

#### Test:
- Send a terminating signal to the daemon: Current behavor: Daemon is still running, it terminates.when a client tries to connect to it (if a client is still connected after the signal, it receive the message when deamon terminates: on a new connection).
- After signal, only a second signal or a connection attemp kill the daemon
- On the above case, test to relaunch a daemon (it must not be possible if the previous still running).
- Delete the log file while the daemon is running: Current behavor: it log nowhere (Maybe it should log recreate log file).

## Client usage

```bash
telnet localhost 4242
```

```bash
nc localhost 4242
```

