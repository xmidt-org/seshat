# seshat

nanomsg and wrp service discovery agent

[![Build Status](https://travis-ci.org/Comcast/seshat.svg?branch=master)](https://travis-ci.org/Comcast/seshat)
[![codecov.io](http://codecov.io/github/Comcast/seshat/coverage.svg?branch=master)](http://codecov.io/github/Comcast/seshat?branch=master)
[![Coverity](https://img.shields.io/coverity/scan/11941.svg)]("https://scan.coverity.com/projects/comcast-seshat)
[![Apache V2 License](http://img.shields.io/badge/license-Apache%20V2-blue.svg)](https://github.com/Comcast/seshat/blob/master/LICENSE.txt)

# Building and Testing Instructions

```
mkdir build
cd build
cmake ..
make
make test
make coverage
firefox index.html
```

# Coding Formatter Settings

Please format pull requests using the following command to keep the style consistent.

```
astyle -A10 -S -f -U -p -D -c -xC90 -xL
```
