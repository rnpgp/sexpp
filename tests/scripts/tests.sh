#! /bin/bash
#
# Copyright 2021-2023 Ribose Inc. (https://www.ribose.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

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

   if [[ "${SHARED_LIB:-}" == "on" ]]; then
      case "$OSTYPE" in
         darwin*)
            assert_installed "$DIR_INS_B/sexp"
            assert_installed "$DIR_INS_L/libsexp.dylib"
         ;;
         windows )
            assert_installed "$DIR_INS_B/sexp.exe"
            assert_installed "$DIR_INS_B/sexp.dll"
            assert_installed "$DIR_INS_L/sexp.lib"
         ;;
         msys)

            assert_installed "$DIR_INS_B/sexp.exe"
            assert_installed "$DIR_INS_B/libsexp.dll"
            assert_installed "$DIR_INS_L/libsexp.dll.a"
         ;;
         *)
            assert_installed "$DIR_INS_B/sexp"
            assert_installed_var "$DIR_INS_L" "$DIR_INS_L64" "libsexp.so"
         ;;   
      esac
   else
      case "$OSTYPE" in
         windows)
            assert_installed "$DIR_INS_B/sexp.exe"
            assert_installed "$DIR_INS_L/sexp.lib" 
         ;;
         msys)
            assert_installed "$DIR_INS_B/sexp.exe"
            assert_installed "$DIR_INS_L/libsexp.a"
         ;;
         *)
            assert_installed "$DIR_INS_B/sexp"
            assert_installed_var "$DIR_INS_L" "$DIR_INS_L64" "libsexp.a"
         ;;   
      esac
   fi

   assert_installed_var "$DIR_INS_P" "$DIR_INS_P64" "sexp.pc"
   assert_installed "$DIR_INS_I/sexp.h"
   assert_installed "$DIR_INS_I/sexp-error.h"
}

# ......................................................................
# Check sexp client application
# THese are the examples from README.adoc
test_sexp_cli() {
   echo "==> SEXP client application test"

# On Windows there will be CRLF vs LS mismatch
# We would rather skip these tests
   if [[ "$OSTYPE" == "windows" || "$OSTYPE" == "msys" ]]; then
      startSkipping
   fi

   app="$DIR_INS_B/sexp"
# shellcheck disable=SC2251
! IFS= read -r -d '' expected << EOM
Input:

Writing base64 (of canonical) output to certificate.dat
EOM
   export LD_LIBRARY_PATH="$DIR_INS_L":"$DIR_INS_L64"
   echo "(aa bb (cc dd))" > input1.dat
   output=$("$app" -o certificate.dat -p -b < input1.dat)
#  $expected possibly includes extra EOL at the end -- it depends on OS
   assertContains "$expected" "$output"
   output=$(cat certificate.dat)
   assertEquals "{KDI6YWEyOmJiKDI6Y2MyOmRkKSk=}" "$output"

   output=$("$app" -i certificate.dat -x)
   assertEquals "(2:aa2:bb(2:cc2:dd))" "$output"

# shellcheck disable=SC2251
! IFS= read -r -d '' expected << EOM
Reading input from certificate.dat

Canonical output:
(2:aa2:bb(2:cc2:dd))
Base64 (of canonical) output:
{KDI6YWEyOmJiKDI6Y2MyOmRkKSk=}
Advanced transport output:
(aa bb (cc dd))
EOM

   output=$("$app" -i certificate.dat -a -b -c -p -w 0)
   assertContains "$expected" "$output"

# shellcheck disable=SC2251
! IFS= read -r -d '' expected << EOM
Input:

Canonical output:
(3:abc3:def(3:ghi3:jkl))
Base64 (of canonical) output:
{KDM6YWJjMzpkZWYoMzpnaGkzOmprbCkp}
Advanced transport output:
(abc def (ghi jkl))

Input:
EOM
   echo "(abc def (ghi jkl))" > input2.dat
   output=$("$app" < input2.dat)
   assertContains "$expected" "$output"

   if [[ "$OSTYPE" == "windows" || "$OSTYPE" == "msys" ]]; then
      endSkipping
   fi

}

# ......................................................................
# main

DIR0="$( cd "$( dirname "$0" )" && pwd )"
DIR1="${DIR_ROOT:="$DIR0/../.."}"
DIR_ROOT="$( cd "$DIR1" && pwd )"

if [[ -z "${DIR_INSTALL:-}" ]]; then
   DIR_INSTALL="$DIR_ROOT/install"
fi

DIR_INS_B="$DIR_INSTALL/bin"
DIR_INS_L="$DIR_INSTALL/lib"
DIR_INS_L64="$DIR_INSTALL/lib64"
DIR_INS_P="$DIR_INS_L/pkgconfig"
DIR_INS_P64="$DIR_INS_L64/pkgconfig"
DIR_INS_I="$DIR_INSTALL/include/sexp"

DIR_TESTS="$( cd "$DIR0/.." && pwd)"

echo "Running sexp additional tests"
# shellcheck source=/dev/null
. "$DIR_TESTS"/shunit2/shunit2
