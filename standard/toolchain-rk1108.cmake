set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)
set(BUILD_ARM 1)
set(BUILD_RK1108 1)

set(RK1108_TOOLCHAIN_ROOT /opt/rk1108_toolchain)
set(RK1108_COMMON_FLAGS " --sysroot=${RK1108_TOOLCHAIN_ROOT}/usr/arm-rkcvr-linux-uclibcgnueabihf/sysroot -D__RK1108__ -fno-omit-frame-pointer")

set(CMAKE_C_COMPILER ${RK1108_TOOLCHAIN_ROOT}/usr/bin/arm-linux-gcc CACHE FILEPATH "C Compiler" FORCE)
set(CMAKE_CXX_COMPILER ${RK1108_TOOLCHAIN_ROOT}/usr/bin/arm-linux-g++ CACHE FILEPATH "C++ Compiler" FORCE)

set(CMAKE_C_FLAGS "${RK1108_COMMON_FLAGS}" CACHE STRING "toolchain_cflags" FORCE)
set(CMAKE_CXX_FLAGS "${RK1108_COMMON_FLAGS}" CACHE STRING "toolchain_cxxflags" FORCE)
set(CMAKE_LINK_FLAGS "${RK1108_COMMON_FLAGS}" CACHE STRING "toolchain_linkflags" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)