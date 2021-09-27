#! /bin/bash

command=$1

export CC=$(which clang)
export CXX=$(which clang++)

#export CCACHE_SLOPPINESS="pch_defines,time_macros"
# Meke sure to use `lld` linker it faster and has a better UX
export ASAN_OPTIONS=check_initialization_order=1
LSAN_OPTIONS=suppressions=$(pwd)/.ignore_sanitize
export LSAN_OPTIONS

# The `builder` script is supposed to be run from the
# root of the source tree
ROOT_DIR=$(pwd)
BUILD_DIR=$ROOT_DIR/build
ME=$(cd "$(dirname "$0")/." >/dev/null 2>&1 ; pwd -P)
CMAKEARGS="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DSERENE_CCACHE_DIR=~/.ccache"
scanbuild=scan-build


function gen_precompile_header_index() {
    echo "// DO NOT EDIT THIS FILE: It is aute generated by './builder gen_precompile_index'" > ./include/serene_precompiles.h
    echo "#ifndef SERENE_PRECOMPIL_H" >> ./include/serene_precompiles.h
    echo "#define SERENE_PRECOMPIL_H" >> ./include/serene_precompiles.h
    grep -oP  "#include .llvm/.*" . -R|cut -d':' -f2|tail +2 >> ./include/serene_precompiles.h
    grep -oP  "#include .mlir/.*" . -R|cut -d':' -f2|tail +2 >> ./include/serene_precompiles.h
    echo "#endif" >> ./include/serene_precompiles.h
}

function pushed_build() {
    pushd "$BUILD_DIR" > /dev/null || return
}

function popd_build() {
    popd > /dev/null || return
}

function compile() {
    pushed_build
    cmake --build .
    popd_build
}

function build() {
    pushed_build
    echo "Running: "
    echo "cmake -G Ninja $CMAKE_CCACHE $CMAKEARGS -DCMAKE_BUILD_TYPE=Debug \"$@\" \"$ROOT_DIR\""
    cmake -G Ninja $CMAKEARGS -DCMAKE_BUILD_TYPE=Debug "$@" "$ROOT_DIR"
    cmake --build .
    popd_build
}

function build-20() {
    pushed_build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCPP_20_SUPPORT=ON "$@" "$ROOT_DIR"
    cmake --build .
    popd_build
}

function build-release() {
    pushed_build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release "$ROOT_DIR"
    cmake --build .
    popd_build
}

function build-docs() {
    pushed_build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Docs "$ROOT_DIR"
    cmake --build .
    popd_build
}

function clean() {
    rm -rf "$BUILD_DIR"
}

function run() {
    pushed_build
    "$BUILD_DIR"/bin/serenec "$@"
    popd_build
}

function memcheck() {
    export ASAN_FLAG=""
    build
    pushed_build
    valgrind --tool=memcheck --leak-check=yes --trace-children=yes "$BUILD_DIR"/bin/serenec "$@"
    popd_build
}

function run-tests() {
    "$BUILD_DIR"/src/tests/tests
}

function tests() {
    pushed_build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON "$ROOT_DIR"
    cmake --build .
    popd_build
}


case "$command" in
    "setup")
        rm -rv $ME/.git/hooks/pre-commit
        ln -s $ME/scripts/pre-hook $ME/.git/hooks/pre-commit
        ;;
    "build")
        clean
        mkdir -p "$BUILD_DIR"
        build "${@:2}"
        ;;
    "build-20")
        clean
        mkdir -p "$BUILD_DIR"
        build-20 "${@:2}"
        ;;

    "build-docs")
        clean
        mkdir -p "$BUILD_DIR"
        build-docs "${@:2}"
        ;;

    "build-release")
        clean
        mkdir -p "$BUILD_DIR"
        build-release "${@:2}"
        ;;
    "compile")
        compile
        ;;
    "compile-and-tests")
        compile
        run-tests
        ;;
    "run")
        run "${@:2}"
        ;;
    "run-tests")
        run-tests "${@:2}"
        ;;

    "scan-build")
        clean
        mkdir -p "$BUILD_DIR"
        pushed_build
        exec $scanbuild cmake "$ROOT_DIR"
        exec $scanbuild scan-build make -j 4
        popd_build
        ;;
    "memcheck")
        memcheck "${@:2}"
        ;;
    "tests")
        clean
        mkdir -p "$BUILD_DIR"
        tests
        run-tests
        ;;
    "clean")
        rm -rf "$BUILD_DIR"
        ;;
    "gen_precompile_index")
        gen_precompile_header_index
        ;;
    "full-build")
        clean
        mkdir -p "$BUILD_DIR"
        build
        tests
        run-tests
        memcheck
        ;;
    *)
        echo "Commands: "
        echo "setup - Setup the githooks for devs"
        echo "full-build - Build and test Serene."
        echo "build - Build Serene from scratch in DEBUG mode."
        echo "build-release - Build Serene from scratch in RELEASE mode."
        echo "compile - reCompiles the project using the already exist cmake configuration"
        echo "compile-and-tests - reCompiles the project using the already exist cmake configuration and runs the tests"
        echo "run - Runs the serene executable"
        echo "scan-build - Compiles serene with static analyzer"
        echo "tests - Runs the test cases"
        echo "memcheck - Runs the memcheck(valgrind) tool."
        echo "clean - :D"
        ;;
esac
