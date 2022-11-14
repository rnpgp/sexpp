# Copyright (c) 2022, [Ribose Inc](https://www.ribose.com).
# All rights reserved.
# This file is a part of RNP sexp library
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other matrials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# 3.14 -- FetchContent
cmake_minimum_required(VERSION 3.14)

project (sexp VERSION 0.1.0 LANGUAGES C CXX)

option(WITH_SEXP_TESTS "Build tests" ON)
option(WITH_SEXP_CLI "Build sexp console application" ON)
option(ENABLE_SANITIZERS "Enable ASan and other sanitizers" OFF)

include(GNUInstallDirs)

if (ENABLE_SANITIZERS)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Sanitizers have only been tested with the clang compiler.")
    endif()
    if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
        message(STATUS "Forcing build type to Debug for sanitizers)")
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type. Forced to Debug" FORCE)
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "Building ${CMAKE_BUILD_TYPE} configuration")

if (ENABLE_SANITIZERS)
  add_compile_options(-fsanitize=leak,address,undefined -fno-omit-frame-pointer -fno-common -O1)
  link_libraries(-fsanitize=leak,address,undefined)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

add_library(sexp STATIC
    src/sexp-input.cpp
    src/sexp-output.cpp
    src/sexp-object.cpp
    src/sexp-simple-string.cpp
    src/sexp-char-defs.cpp
    src/sexp-error.cpp
    include/sexp/sexp.h
    include/sexp/sexp-error.h
)

target_compile_features(sexp PUBLIC cxx_std_11)
target_include_directories (sexp PUBLIC include)

if(${WITH_SEXP_CLI})
    add_executable (sexp-cli
                    src/sexp-main.cpp
                    include/sexp/sexp.h
                    include/sexp/sexp-error.h
    )
    target_include_directories (sexp-cli PUBLIC include)
    target_link_libraries(sexp-cli PRIVATE sexp)

    target_compile_features(sexp-cli PUBLIC cxx_std_11)
    target_include_directories (sexp-cli PUBLIC include)
endif(${WITH_SEXP_CLI})


if(${WITH_SEXP_TESTS})
    include(CTest)
    include(FetchContent)
    include(GoogleTest)

    set(SEXP_SAMPLES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/samples")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sexp-samples-folder.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/include/sexp-samples-folder.h"
        @ONLY
    )

    message(STATUS "Fetching googletest")
    # Version 1.8.1 to make it compatible with gcc 4.8.5 which misses some C++11 features
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.8.1
    )
    # maintain compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # do not install gtest
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    # explicitely disable unneeded gmock build
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)

    add_executable(sexp-tests
        tests/src/baseline-tests.cpp
        tests/src/compat-tests.cpp
        tests/src/exception-tests.cpp
        tests/src/primitives-tests.cpp
        tests/src/compare-files.cpp
        tests/include/sexp-tests.h
    )

    target_link_libraries(sexp-tests PRIVATE
        sexp
        gtest_main
        gtest
    )

    target_include_directories(sexp-tests PRIVATE
        tests/include
        include
        "${GTEST_INCLUDE_DIRS}"
    )

    target_compile_features(sexp-tests PUBLIC cxx_std_11)

    gtest_discover_tests(sexp-tests)
endif(${WITH_SEXP_TESTS})

set(CONFIGURED_PC "${CMAKE_CURRENT_BINARY_DIR}/sexp.pc")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sexp.pc.in"
    "${CONFIGURED_PC}"
    @ONLY
)

install(TARGETS sexp DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES "${CONFIGURED_PC}" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
if(${WITH_SEXP_CLI})
    install(TARGETS sexp-cli DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif(${WITH_SEXP_CLI})
