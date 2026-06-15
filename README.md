# Compiling the code

Make sure you have ARGoS >= 3.0.0-beta52 installed!

Commands:

```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../src
make
sudo make install
```

If ARGoS does not find the new robot, try:

```shell
cmake -DCMAKE_BUILD_TYPE=Release ../src -DCMAKE_INSTALL_PREFIX=/usr
```

For Debug builds:

```shell
cmake -DCMAKE_BUILD_TYPE=Debug
```
# Reference

If you use this plug-in in your research, please cite:

D. H. Stolfi and G. Danoy, **An ARGoS plug-in for the Crazyflie drone.** 2024, <https://arxiv.org/abs/2401.16948>.

