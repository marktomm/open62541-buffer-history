https://github.com/open62541/open62541/issues/4374

# Prerequisites

Install open62541 on target

```bash
git clone https://github.com/open62541/open62541.git
cd open62541
git co v1.2.1 # commit 4038a318952e3d7d77a85f205d279cdfabc81930
git submodule update --init --recursive 
mkdir build 
cd build
cmake -DUA_LOGLEVEL=100 -DUA_NAMESPACE_ZERO=FULL -DUA_ENABLE_FULL_NS0=1 -DUA_DEBUG=1 -DUA_ENABLE_HISTORIZING=ON ..
make -j9
sudo make install
```

# Builing

```bash
meson build && cd build && meson compile # or last cmd may be ninja or make or whatever is used
```

# Running

All executables are built in `./build` directory in project root (if built using suggested command).

Run server: `cd build && ./server-history-custom`
Run client: `cd build && ./client-history`

Server and client must be run in parallel, i.e. in separate terminal windows. Run server first. Rerun server after each client run.
