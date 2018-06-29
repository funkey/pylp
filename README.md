# pylp
Unified python wrappers for popular ILP solvers

## installation

Clone this repository:
```bash
git clone https://github.com/funkey/pylp
```

Check out the submodules:
```bash
git submodule update --init
```

Get dependencies:
```bash
sudo apt-get install cmake libboost-all-dev python-dev
```

To get Gurobi support, set the `cmake` variable `Gurobi_ROOT_DIR` (or the
environment variable `$GUROBI_ROOT_DIR` before you run `cmake`) to the Gurobi
subdirectory containing the /lib and /bin directories.

Compile and install the wrapper:
```bash
python setup.py install
```
