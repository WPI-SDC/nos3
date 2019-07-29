# http://www.cmake.org/Wiki/CmakeEldk

#### AUTO-GENERATED DO NOT EDIT ####

# this one is important
SET(CMAKE_SYSTEM_NAME           Linux)

# this one not so much
SET(CMAKE_SYSTEM_VERSION        1)

# specify the target CPU
SET(CMAKE_SYSTEM_PROCESSOR      i386)

# specify the cross compiler
SET(CMAKE_C_COMPILER            "/usr/bin/gcc")
SET(CMAKE_CXX_COMPILER          "/usr/bin/g++")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   NEVER)

# these settings are specific to cFE/OSAL and determines how the abstraction layers are built
SET(CFE_SYSTEM_PSPNAME      "nos-linux")

SET(OSAL_SYSTEM_OSTYPE      "posix")

SET(CMAKE_C_FLAGS_INIT "-g -Wfatal-errors -Wformat=0 -m32" CACHE STRING "C Flags required by platform")

