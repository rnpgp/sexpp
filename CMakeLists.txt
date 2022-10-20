cmake_minimum_required(VERSION 3.14)

project (sexp C)

list (APPEND SOURCES
             src/sexp-basic.c
             src/sexp-input.c
             src/sexp-output.c
             src/sexp-main.c
             include/sexp.h )

include_directories (include)

add_executable (sexp ${SOURCES})
