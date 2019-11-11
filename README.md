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
$ sudo apt install git build-essential automake autogen autoconf libtool libtool-bin python3-dev python3-pip libssl-dev cmake
```

### RHEL based (CentOS):
**a. Check that system is up to date:**
```bash
$ sudo yum update
$ sudo yum install yum-utils
$ sudo yum install https://centos7.iuscommunity.org/ius-release.rpm
```

**b. Install dependencies:**
```bash
$ sudo yum groupinstall development
$ sudo yum install libtool gcc gcc-c++ openssl-devel cmake3 python36-devel python36-pip
$ sudo ln -s /usr/bin/cmake3 /usr/bin/cmake
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
$ brew install cmake automake autogen libtool autoconf autogen openssl@1.1
$ echo 'export OPENSSL_ROOT_DIR="/usr/local/opt/openssl@1.1"' >> ~/.bash_profile
$ echo 'export OPENSSL_INCLUDE_DIR="/usr/local/opt/openssl@1.1/include"' >> ~/.bash_profile
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
$ python build_extensions.py --build-type Debug
```
**add bxextensions/ to your `$PYTHONPATH`**
