#!/usr/bin/env sh

usage() {
    echo "usage: ${0} build"
    echo ""
    echo "  Exit 0 if all tests passed, otherwise 1."
    echo "  If you get any code other them these two,"
    echo "  an unexpected error may have occured."
    echo ""
    echo "build:"
    echo "  Specify one of the builds to test on."
    echo ""
    echo "  -d, --debug"
    echo "  -r, --release"
}

if [ "$#" -ne 1 ]; then
    usage
    exit 1
fi

case "$1" in
    -d|--debug)
        BUILD_MODE='debug'
        ;;
    -r|--release)
        BUILD_MODE='release'
        ;;
    *)
        usage
        exit 1
esac

NO_COLOR='\033[0m'

echo_in_green() {
    GREEN='\033[0;32m'
    echo "${GREEN}$*${NO_COLOR}"
}

echo_in_red() {
    RED='\033[0;31m'
    echo "${RED}$*${NO_COLOR}"
}

echo_in_yellow() {
    YELLOW='\033[0;33m'
    echo "${YELLOW}$*${NO_COLOR}"
}

TITILE_BANNER="[==========]"
RUN_BANNER="[ RUN      ]"
BODY_BANNER="[          ]"
OK_BANNER="[       OK ]"
FAILED_BANNER="[  FAILED  ]"
SECTION_BANNER="[----------]"

#
# To add a new test, extend the following test case list
#
EXEC=bin/regexp
echo_in_yellow "${TITILE_BANNER} Running CLI tests on ${BUILD_MODE} build..."
pass_count=0
fail_count=0
if make clean >/dev/null 2>&1 && make ${BUILD_MODE} >/dev/null 2>&1; then
    # begin test cases
    echo_in_yellow "${RUN_BANNER} Normal matched"
    args="(a|b)*abb ababb"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 0"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Normal matched (cache)"
    args="-c (a|b)*abb ababb"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 0"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Normal unmatched"
    args="(a|b)*abb abab"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Normal unmatched (cache)"
    args="-c (a|b)*abb abab"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Ill-formed regex"
    args="(a|b*abb ababb"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Missing string"
    args="(a|b)*abb"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Unknown argument"
    args="(a|b)*abb ababb unknown"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    DOT_EXT="dot"
    echo_in_yellow "${RUN_BANNER} Graph NFA to default file"
    DEFAULT="nfa"
    echo "${BODY_BANNER} set-up: Removing ${DEFAULT}.${DOT_EXT}..."
    rm -f "${DEFAULT}.${DOT_EXT}"
    args="-g (a|b)*abb"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 0"
        fail_count=$((fail_count + 1))
    elif [ ! -f "${DEFAULT}.${DOT_EXT}" ]; then
        echo_in_red "${FAILED_BANNER} output file ${DEFAULT}.${DOT_EXT} not found"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi
    echo "${BODY_BANNER} tear-down: Removing ${DEFAULT}.${DOT_EXT}..."
    rm -f "${DEFAULT}.${DOT_EXT}"

    echo_in_yellow "${RUN_BANNER} Graph NFA to designated file"
    DESIGNSTED="cli_test_nfa"
    echo "${BODY_BANNER} set-up: Removing ${DESIGNSTED}.${DOT_EXT}..."
    rm -f "${DESIGNSTED}.${DOT_EXT}"
    args="-g (a|b)*abb -o ${DESIGNSTED}"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 0"
        fail_count=$((fail_count + 1))
    elif [ ! -f "${DESIGNSTED}.${DOT_EXT}" ]; then
        echo_in_red "${FAILED_BANNER} output file ${DESIGNSTED}.${DOT_EXT} not found"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi
    echo "${BODY_BANNER} tear-down: Removing ${DESIGNSTED}.${DOT_EXT}..."
    rm -f "${DESIGNSTED}.${DOT_EXT}"

    echo_in_yellow "${RUN_BANNER} Output specified without using graph mode"
    args="(a|b)*abb ababb -o file"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Output option set without file specified"
    args="(a|b)*abb ababb -o"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "${RUN_BANNER} Cache option set under graph mode"
    args="(a|b)*abb ababb -g -c"
    echo "${BODY_BANNER} ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "${FAILED_BANNER} should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "${OK_BANNER}"
        pass_count=$((pass_count + 1))
    fi
    # tail of test cases
    echo_in_yellow "${SECTION_BANNER} $((pass_count + fail_count)) tests ran."
else
    echo_in_red "${FAILED_BANNER} Compilation error"
    exit 1
fi

TITILE_BANNER="[==========]"
FAILED_BANNER="[  FAILED  ]"
PASSED_BANNER="[  PASSED  ]"
echo ""
echo_in_yellow "${TITILE_BANNER} Summary"
if [ $pass_count -ne 0 ]; then
    echo_in_green "${PASSED_BANNER} ${pass_count} tests."
fi
if [ $fail_count -ne 0 ]; then
    echo_in_red "${FAILED_BANNER} ${fail_count} tests."
    exit 1
fi
exit 0
