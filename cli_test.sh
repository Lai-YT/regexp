#!/usr/bin/env sh

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

EXEC=bin/regexp
echo_in_yellow "[==========] Running CLI tests..."
pass_count=0
fail_count=0
if make clean >/dev/null 2>&1 && make >/dev/null 2>&1; then
    echo_in_yellow "[ RUN      ] Normal matched"
    args="(a|b)*abb ababb"
    echo "[          ] ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 0"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "[ RUN      ] Normal unmatched"
    args="(a|b)*abb abab"
    echo "[          ] ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "[ RUN      ] Ill-formed regex"
    args="(a|b*abb ababb"
    echo "[          ] ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "[ RUN      ] Missing string"
    args="(a|b)*abb"
    echo "[          ] ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi

    echo_in_yellow "[ RUN      ] Unknown argument"
    args="(a|b)*abb ababb unknown"
    echo "[          ] ${EXEC} ${args}"
    if echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 1"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi

    DOT_EXT="dot"
    echo_in_yellow "[ RUN      ] Graph NFA to default file"
    DEFAULT="nfa"
    echo "[          ] set-up: Removing ${DEFAULT}.${DOT_EXT}..."
    rm -f "${DEFAULT}.${DOT_EXT}"
    args="-g (a|b)*abb"
    echo "[          ] ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 0"
        fail_count=$((fail_count + 1))
    elif [ ! -f "${DEFAULT}.${DOT_EXT}" ]; then
        echo_in_red "[  FAILED  ] output file ${DEFAULT}.${DOT_EXT} not found"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi
    echo "[          ] tear-down: Removing ${DEFAULT}.${DOT_EXT}..."
    rm -f "${DEFAULT}.${DOT_EXT}"

    echo_in_yellow "[ RUN      ] Graph NFA to designated file"
    DESIGNSTED="cli_test_nfa"
    echo "[          ] set-up: Removing ${DESIGNSTED}.${DOT_EXT}..."
    rm -f "${DESIGNSTED}.${DOT_EXT}"
    args="-g (a|b)*abb -o ${DESIGNSTED}"
    echo "[          ] ${EXEC} ${args}"
    if ! echo "${args}" | xargs ${EXEC} >/dev/null 2>&1; then
        echo_in_red "[  FAILED  ] should exit 0"
        fail_count=$((fail_count + 1))
    elif [ ! -f "${DESIGNSTED}.${DOT_EXT}" ]; then
        echo_in_red "[  FAILED  ] output file ${DESIGNSTED}.${DOT_EXT} not found"
        fail_count=$((fail_count + 1))
    else
        echo_in_green "[       OK ]"
        pass_count=$((pass_count + 1))
    fi
    echo "[          ] tear-down: Removing ${DESIGNSTED}.${DOT_EXT}..."
    rm -f "${DESIGNSTED}.${DOT_EXT}"

    echo_in_yellow "[----------] $((pass_count + fail_count)) tests ran."
else
    echo_in_red "[  FAILED  ] Compilation error"
    exit 1
fi

echo ""
echo_in_yellow "[==========] Summary"
if [ $pass_count -ne 0 ]; then
    echo_in_green "[  PASSED  ] ${pass_count} tests."
fi
if [ $fail_count -ne 0 ]; then
    echo_in_red "[  FAILED  ] ${fail_count} tests."
    exit 1
fi
exit 0
