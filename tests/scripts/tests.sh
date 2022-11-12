#! /bin/bash
#
# Copyright (c) 2022 [Ribose Inc](https://www.ribose.com).
# All rights reserved.
# This file is a part of sexp
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
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

# More safety, by turning some bugs into errors.
# Without `errexit` you don’t need ! and can replace
# PIPESTATUS with a simple $?
set -o errexit -o pipefail -o noclobber -o nounset

assert_installed() {
   assertTrue "$1 was not installed" "[ -f $1 ]"
}

assert_installed_var() {
   assertTrue "{$1,$2}/$3 was not installed" "[ -f $1/$3 ] || [ -f $2/$3 ]"
}

# ......................................................................
# Check that sexp is installed as expected
test_install_script() {
   echo "==> Install script test"

   DIR_INSTALL="$DIR_ROOT/install"
   DIR_INS_B="$DIR_INSTALL/bin"
   DIR_INS_L="$DIR_INSTALL/lib"
   DIR_INS_L64="$DIR_INSTALL/lib64"
   DIR_INS_P="$DIR_INS_L/pkgconfig"
   DIR_INS_P64="$DIR_INS_L64/pkgconfig"
   DIR_INS_I="$DIR_INSTALL/include/sexp"

   if [[ "$OSTYPE" == "windows" ]]; then
      assert_installed "$DIR_INS_B/sexp-cli.exe"
      assert_installed "$DIR_INS_L/sexp.lib"
   else
      assert_installed "$DIR_INS_B/sexp-cli"
      assert_installed_var "$DIR_INS_L" "$DIR_INS_L64" "libsexp.a"
   fi

   assert_installed_var "$DIR_INS_P" "$DIR_INS_P64" "sexp.pc"
   assert_installed "$DIR_INS_I/sexp.h"
   assert_installed "$DIR_INS_I/sexp-error.h"
}

# ......................................................................
# main
DIR0="$( cd "$( dirname "$0" )" && pwd )"
DIR1="${DIR_ROOT:="$DIR0/../.."}"
DIR_ROOT="$( cd "$DIR1" && pwd )"

DIR_TESTS="$( cd "$DIR0/.." && pwd)"

echo "Running sexp additional tests"
# shellcheck source=/dev/null
. "$DIR_TESTS"/shunit2/shunit2
