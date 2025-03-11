# matt-daemon

## TODO
- Emmit logs like subject (This means that we should create global variable for logger)

### Test:
- Delete the log file while the daemon is running: Current behavor: it log nowhere (Maybe it should recreate log file ?).
    Solution: open and close the log file for each call of Tintin_reporter::log() function. (this will automatilcally recreate the file if it is deleted) see branch `log_file_recreate`

## Client usage

```bash
telnet localhost 4242
```

```bash
nc localhost 4242
```

