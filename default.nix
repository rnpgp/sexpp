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

{ pkgs ? import <nixpkgs> { }
, lib ? pkgs.lib
, stdenv ? pkgs.stdenv
}:

stdenv.mkDerivation rec {
  pname = "sexp";
  version = "unstable";

  src = ./.;

  cmakeFlags = [
    "-DCMAKE_INSTALL_PREFIX=${placeholder "out"}"
    "-DWITH_SEXP_TESTS=on"
    "-DWITH_SEXP_CLI=on"
    "-DDOWNLOAD_GTEST=off"
  ];

  nativeBuildInputs = with pkgs; [ cmake gtest pkg-config ];

  meta = with lib; {
    homepage = "https://github.com/rnpgp/sexp";
    description = "S-expressions parser and generator C++ library, fully compliant to [https://people.csail.mit.edu/rivest/Sexp.txt]";
    license = licenses.bsd2;
    platforms = platforms.all;
    maintainers = with maintainers; [ ribose-jeffreylau ];
  };
}