#!/bin/sh
# Smoke tests: boot the OS, drive the terminal through stdin and check the
# output of every app. Usage: sh tests/run_tests.sh ./tmsos
BIN=${1:-./tmsos}
cd "$(dirname "$0")/.." || exit 1
fail=0

check() { # name, input, expected substring...
    name=$1; input=$2; shift 2
    out=$(printf "$input" | "$BIN" 2>&1)
    for want in "$@"; do
        case $out in
            *"$want"*) ;;
            *) echo "FAIL $name: expected '$want'"; echo "$out" | sed 's/^/    /'; fail=1; return ;;
        esac
    done
    echo "ok   $name"
}

check "boot + help"   'help\nhlt\n'                       "Welcome to TMS-OS" "run <app>" "Powering off"
check "echo"          'echo hello world\nhlt\n'           "hello world"
check "unknown"       'frobnicate\nhlt\n'                 "unknown command"
check "missing app"   'run nope\nhlt\n'                   "not found"
check "calc"          'run calc\n6 * 7\n12 + 30 - 2 * 3\n100 / 3\n5 - 9\ne\nhlt\n' \
                      "is : 42" "is : 120" "is : 33" "is : -4"
check "sum"           'run sum\n10\n0\nhlt\n'             "55"
check "fact"          'run fact\n6\n0\nhlt\n'             "720"
check "fib"           'run fib\nadd\n10\nadd\n20\nexit\nhlt\n' "55" "6765"
check "reverse"       'run reverse\nstressed\n\nhlt\n'    "desserts"
check "arrays"        'run arrays\nadd\n3\n4\n9\n2\nmax\nmin\navg\nexit\nhlt\n' \
                      "maximum number in this array is: 9" "minimum number in this array is: 2" "is: 5"
check "back to shell" 'run sum\n0\necho still here\nhlt\n' "still here"
check "history"       'echo a\nhist\nhlt\n'               "echo a"
check "end of input"  'echo bye\n'                        "Shutting down OS"

exit $fail
