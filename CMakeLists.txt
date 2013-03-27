cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

# The name of our project is "Mempig". CMakeLists files in this project can
# refer to the root source directory of the project as ${MEMPIG_SOURCE_DIR}
# and to the root binary directory of the project as ${MEMPIG_BINARY_DIR}.
project(mempig)

enable_testing()

# Define "make check" as an alias for "make test."
add_custom_target(check COMMAND ctest)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Mempig memory hogging tool")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_CONTACT "cmccabe@alumni.cmu.edu")
SET(CPACK_GENERATOR "TGZ")
INCLUDE(CPack)

set(CMAKE_BUILD_TYPE, Release) # can also be Debug
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_REENTRANT -D_FILE_OFFSET_BITS=64")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
# We use pthread_key_create with a non-NULL destructor parameter, so we
# cannot allow our shared library to be unloaded.  If we did allow this,
# we might get users into a situation where the destructor functions were
# invoked, but they had been unloaded.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z nodelete")

set(CMAKE_C_FLAGS_RELEASE "-O3 -Wuninitialized")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -fstack-protector -rdynamic")
set(CMAKE_CURRENT_BINARY_DIR, builds)
SET(CMAKE_INSTALL_PREFIX /usr/local)

# Set up include paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

add_executable(mempig
    daemon.c
    log.c
    mempig.c
)
