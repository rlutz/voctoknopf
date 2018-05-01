Mocktocore
----------

The `mocktocore` script was created to make testing of hardware and
software components which communicate with Voctomix easier.  It
behaves identical to the `voctocore` command server but doesn't do any
actual video mixing, thus being very easy to set up.  It does need the
Voctocore configuration file, though, as this file contains the
definition of the sources and composition modes.

Mocktocore is run as
```
$ ./mocktocore -i 34c3-config.ini
```

You can connect to the server from the command line via
```
$ nc localhost 9999
```

**TODO:** The internal methods invoked for a command aren't
necessarily processed as a transaction which could lead to an
inconsistent state.  This is probably a bug in the Voctomix software.
