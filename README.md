# Installation

## Platform specific prerequisites
### Debian based (Ubuntu):
**a. Check that system is up to date:**
```bash
$ sudo apt update
$ sudo apt upgrade
```

**b. Install dependencies:**
```bash
$ sudo apt install build-essential automake autogen autoconf libtool libtool-bin python python-pip libssl-dev cmake
```

### Alpine Linux:
**a. Check that system is up to date:**
```bash
$ apk update
```

**b. Install dependencies:**
```bash
$ apk add --no-cache build-base musl automake autoconf libtool openssl-dev cmake
```

### Mac:
**a. Install dependencies:**
```bash
$ brew install cmake automake autogen libtool autoconf autogen
```

**b. Install [XCode command line tools](https://www.ics.uci.edu/~pattis/common/handouts/macmingweclipse/allexperimental/macxcodecommandlinetools.html).**

## Clone
```bash
$ git clone --recursive https://github.com/bloXroute-Labs/bxextensions.git
```
**the use "--recursive" is for fetching depended submodules**

## Existing repository
```bash
$ cd bxextensions/
$ git submodule init
$ git submodule update
```

## Package installation
```bash
$ cd bxextensions/
$ python setup.py install
```
**use `sudo` if needed**

## Dev Installation:
```bash
$ git clone --recursive https://github.com/bloXroute-Labs/bxextensions.git
$ cd bxextensions/
$ python build_extensions.py
```
**add bxextensions/ to your $PYTHONPATH**
