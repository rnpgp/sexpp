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

option(WITH_TESTS "Build tests" ON)
option(WITH_APP "Build sexp console application" ON)

set(CMAKE_BUILD_TYPE Debug)
# Die to GTest C++ 11  deprecation
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

add_library(libsexp STATIC
    src/sexp-basic.cpp
    src/sexp-input.cpp
    src/sexp-output.cpp
    include/sexp.h
)

target_include_directories (libsexp PUBLIC include)

if(${WITH_APP})
    add_executable (sexp src/sexp-main.cpp include/sexp.h)
    target_include_directories (sexp PUBLIC include)
    target_link_libraries(sexp PRIVATE libsexp)
endif(${WITH_APP})


if(${WITH_TESTS})
    message(STATUS "Fetching googletest")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
    )
    # maintain compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # explicitely disable unneeded gmock build
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    find_package(GCrypt REQUIRED)

    add_executable(sexp-tests
        tests/src/sexp-tests.cpp
        tests/include/sexp-tests.h
    )

    target_link_libraries(sexp-tests PRIVATE
        libsexp
        ${GCRYPT_LIBRARIES}
        gtest_main
        gtest
    )

    target_include_directories(sexp-tests PRIVATE
        tests/include
        include
        ${GCRYPT_INCLUDE_DIRS}
        ${GTEST_INCLUDE_DIRS}
    )

endif(${WITH_TESTS})
