##### Dependencies

###### CXX

*   [glog](https://github.com/google/glog)

*   [gflags](https://github.com/gflags/gflags)

###### Python3

*   pip3 install matplotlib

*   pip3 install pandas

*   pip3 install seaborn

##### Caveat

for `MacOS` to link glog and gflags specify the lib directories as indicated
below:

```bash
export C_INCLUDE_PATH=/usr/local/include
export CPLUS_INCLUDE_PATH=/usr/local/include
export LIBRARY_PATH=/usr/local/lib
```

##### Usage

from the project root directory

```bash
mkdir build && cd build || return
cmake ..
make
```

`CMakeLists.txt` is configured to auto-run the tartget post build (i.e., nothing else is required post build).
