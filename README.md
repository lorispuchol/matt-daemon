# matt-daemon

## TODO

#### Test:
- Send a terminating signal to the daemon: Current behavor: Daemon is still running, it terminates only when a client tries to connect to it or il a second signal is sent (if a client was connected before the signal and still after, daemon logs messages on real termination). FIXED
- After signal, only a second signal or a connection attemp kill the daemon FIXED
- On the above case, test to relaunch a daemon (it must not be possible if the previous still running). FIXED

- a 4th is able to connect to server while 3 others are already connected (It doesnt remain the client messages but it shouldn't be able to connect) and it receive the messages from the 4th when the 3th deconnect
- Delete the log file while the daemon is running: Current behavor: it log nowhere (Maybe it should log recreate log file).

## Client usage

```bash
telnet localhost 4242
```

```bash
nc localhost 4242
```

