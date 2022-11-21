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
    "-DWITH_TESTS=on"
    "-DWITH_SEXP_CLI=off"
    "-DDOWNLOAD_GTEST=off"
  ];

  nativeBuildInputs = with pkgs; [ cmake gtest pkg-config ];

  # NOTE: check-only inputs should ideally be moved to checkInputs, but it
  # would fail during buildPhase.
  # checkInputs = [ gtest];

  outputs = [ "out" "lib" "dev" ];

  preConfigure = ''
    commitEpoch=$(date +%s)
    baseVersion=$(cat version.txt)
    echo "v$baseVersion-0-g0-dirty+$commitEpoch" > version.txt
    # For generating the correct timestamp in cmake
    export SOURCE_DATE_EPOCH=$commitEpoch
  '';

  meta = with lib; {
    homepage = "https://github.com/rnpgp/sexp";
    description = "S-expressions parser and generator C++ library, fully compliant to [https://people.csail.mit.edu/rivest/Sexp.txt]";
    license = licenses.bsd2;
    platforms = platforms.all;
    maintainers = with maintainers; [ ribose-jeffreylau ];
  };
}