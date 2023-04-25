# mobilecc
4G网卡模块拨号上网（QUECTEL EC20 Series LTE Module）

# Compile command
## linux pc
- mkdir build
- cd build
- cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
- make

## arm pc (hi3516e)
- mkdir build-cross
- cd build-cross
- cmake .. -DCMAKE_CXX_COMPILER=arm-himix100-linux-g++ -DCMAKE_PREFIX_PATH=/home/kaisen/opt/hi3516e
- make
