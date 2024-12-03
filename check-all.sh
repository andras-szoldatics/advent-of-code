#!/bin/bash

function check_day() {
    pushd ./$1 &>/dev/null

    echo
    echo "=== CHECKING DAY ==="
    echo $(pwd)

    echo "--- building project ---"
    mkdir -p build
    pushd ./build &>/dev/null

    echo
    echo -n "cmake ... "
    cmake -DCMAKE_BUILD_TYPE=Debug .. &>/dev/null
    if [ $? -ne 0 ]; then
        echo "[fail]"

        # re-run to show error
        cmake -DCMAKE_BUILD_TYPE=Debug ..

        exit 1
    else
        echo "[done]"
    fi

    echo -n "build ... "
    cmake --build . &>/dev/null
    if [ $? -ne 0 ]; then
        echo "[fail]"

        # re-run to show error
        cmake --build .

        exit 1
    else
        echo "[done]"
    fi

    popd &>/dev/null
    popd &>/dev/null
}

function check_year() {
    pushd ./$1 &>/dev/null

    # iterate over all days
    for i in $(ls .); do
        if [ -d $i ]; then
            check_day $i
            
            if [ $? -ne 0 ]; then
                echo "=== FAILURE ==="
                exit 1
            fi
        fi
    done

    popd &>/dev/null
}

CWD=$(dirname $0)
# iterate over all directories
for i in $(ls $CWD); do
    if [ -d $i ]; then
        check_year $i

        if [ $? -ne 0 ]; then
            echo "=== FAILURE ==="
            exit 1
        fi
    fi
done

echo
echo "=== DONE ==="
