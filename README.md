# Installation

## Platform specific prerequisites
### Debian based (Ubuntu):
**a. Check that system is up to date:**
```console
sudo apt-get update
sudo apt-get upgrade
```

**b. Install dependencies:**
```console
sudo apt install build-essential automake autogen autoconf libtool libtool-bin python python-pip cmake
```

### Mac:
**a. Install dependencies:**
```console
brew install cmake automake autogen libtool autoconf autogen
```

**b. Install [XCode command line tools](https://www.ics.uci.edu/~pattis/common/handouts/macmingweclipse/allexperimental/macxcodecommandlinetools.html).**

## Package installation
```console
git clone --recursive https://github.com/bloXroute-Labs/bxextensions.git
cd bxextensions/
sudo python setup.py install
```
**the use "--recursive" is for fetching depended submodules**

### Dev Installation:

run `sudo python setup.py develop` instead of `sudo python setup.py install`. 