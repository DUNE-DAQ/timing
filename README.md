ProtoDUNE-SP timing board software
==================================
A collection of C++ classes and a python CLI providing an interface to timing hardware and firmware.

System requirements
--------

To build the package, please ensure you are running a CentOS 7 system with the following packages installed.

```
> python 2.7
> boost 1.53
> uhal 2.7.9
```

Instructions on installing uhal can be found here: https://ipbus.web.cern.ch/ipbus/doc/user/html/software/installation.html.

In addition, you will also need have the python packages below installed.

```
> click 7.1.2
> click-didyoumean
```

Building
--------

Clone a copy of this repository and navigate into `timing-board-software`, then run `make`.

Running
--------

Prepare your environment by sourcing the script, `timing-board-software/tests/env.sh`.