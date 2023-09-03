# QMSInfer

QMSInfer is a tool for verification of software countermeasures against side-
channel attacks. In this tool, we implemented three methods: symbolic method,
SMT-based method, refinement-based method.

# Building QMSInfer using cmake
At first, you must build QMSInfer with minimum cmake-version 3.5, Z3-version 4.6.1.
If so, when you clone down this project, get in this directory.
Execute:
```bash
mkdir build
cd build
cmake ../
make
```

# Using QMSInfer
When you build QMSInfer, you can get into the build directory.
Execute:
```bash
./qmsInfer -help
```
You will get the help information to tell how to use QMSInfer.