#!/bin/bash

# Test script for ideviceerase command-line argument parsing

# Output files for stderr
STDERR_FILE="test_stderr.txt"
# Dummy values for testing
DUMMY_UDID="0000000000000000000000000000000000000000"
DUMMY_ECID="0x123456789ABCD"

# Function to clean up
cleanup() {
    rm -f $STDERR_FILE
}

# Ensure cleanup on exit
trap cleanup EXIT

# Compile the program using make
# Check if 'ideviceerase' exists and if 'src/ideviceerase.c' is newer
if [ ! -f ./ideviceerase ] || [ src/ideviceerase.c -nt ./ideviceerase ] || [ Makefile -nt ./ideviceerase ]; then
    echo "Compiling ideviceerase..."
    make clean > /dev/null
    make
    if [ $? -ne 0 ]; then
        echo "FAIL: Compilation failed."
        exit 1
    fi
    echo "Compilation successful."
else
    echo "ideviceerase is up to date."
fi

echo ""
echo "Starting command-line argument parsing tests..."
echo "=============================================="

# Test Case 1: No arguments
echo -n "Test Case 1: No arguments - "
./ideviceerase > /dev/null 2> $STDERR_FILE
exit_code=$?
if [ $exit_code -ne 1 ]; then
    echo "FAIL (Expected exit code 1, got $exit_code)"
    cat $STDERR_FILE
else
    if grep -q "Usage: ./ideviceerase -u <device_udid>" $STDERR_FILE && grep -q "Error: UDID is a mandatory argument." $STDERR_FILE; then
        echo "PASS"
    else
        echo "FAIL (Did not find expected usage or error message in stderr)"
        cat $STDERR_FILE
    fi
fi
cleanup

# Test Case 2: Only -u <udid>
# This should proceed past argument parsing.
# The program will then try to connect to a device, which will likely fail (that's okay).
# We are interested in the initial output related to argument parsing and program start.
echo -n "Test Case 2: Only -u <udid> - "
# We expect an exit code other than 0 if no device is connected, but not 1 (arg parsing error)
./ideviceerase -u $DUMMY_UDID > test_stdout.txt 2> $STDERR_FILE
exit_code=$?
# For this test, successful argument parsing means it tries to connect.
# It should print "Connecting to device..."
# And not print the usage string.
if ! grep -q "Usage: ./ideviceerase" $STDERR_FILE && grep -q "Connecting to device $DUMMY_UDID..." test_stdout.txt ; then
    echo "PASS (Argument parsing seems correct, attempted connection)"
else
    echo "FAIL (Argument parsing incorrect or did not attempt connection as expected)"
    echo "Exit code: $exit_code"
    echo "--- STDOUT ---"
    cat test_stdout.txt
    echo "--- STDERR ---"
    cat $STDERR_FILE
fi
rm -f test_stdout.txt
cleanup

# Test Case 3: -u <udid> --ecid <ecid> --debug
# Similar to Test Case 2, it should proceed past parsing.
# Debug output should be present.
echo -n "Test Case 3: -u <udid> --ecid <ecid> --debug - "
./ideviceerase -u $DUMMY_UDID --ecid $DUMMY_ECID --debug > test_stdout.txt 2> $STDERR_FILE
exit_code=$?
if ! grep -q "Usage: ./ideviceerase" $STDERR_FILE && \
   grep -q "Debug mode enabled." test_stdout.txt && \
   grep -q "UDID: $DUMMY_UDID" test_stdout.txt && \
   grep -q "ECID: $DUMMY_ECID" test_stdout.txt && \
   grep -q "Connecting to device $DUMMY_UDID..." test_stdout.txt; then
    echo "PASS (Argument parsing correct, debug info present, attempted connection)"
else
    echo "FAIL (Argument parsing incorrect, debug info missing, or did not attempt connection)"
    echo "Exit code: $exit_code"
    echo "--- STDOUT ---"
    cat test_stdout.txt
    echo "--- STDERR ---"
    cat $STDERR_FILE
fi
rm -f test_stdout.txt
cleanup

# Test Case 4: Invalid option
echo -n "Test Case 4: Invalid option (-x) - "
./ideviceerase -x > /dev/null 2> $STDERR_FILE
exit_code=$?
if [ $exit_code -ne 1 ]; then
    echo "FAIL (Expected exit code 1, got $exit_code)"
    cat $STDERR_FILE
else
    # getopt_long usually prints a message like "invalid option -- 'x'" or "unrecognized option"
    # and then the usage.
    if grep -q "Usage: ./ideviceerase -u <device_udid>" $STDERR_FILE && (grep -q "invalid option" $STDERR_FILE || grep -q "unrecognized option" $STDERR_FILE) ; then
        echo "PASS"
    else
        echo "FAIL (Did not find expected usage or error message for invalid option in stderr)"
        cat $STDERR_FILE
    fi
fi
cleanup

# Test Case 5: Missing mandatory -u option (e.g., only --ecid)
echo -n "Test Case 5: Missing mandatory -u (only --ecid) - "
./ideviceerase --ecid $DUMMY_ECID > /dev/null 2> $STDERR_FILE
exit_code=$?
if [ $exit_code -ne 1 ]; then
    echo "FAIL (Expected exit code 1, got $exit_code)"
    cat $STDERR_FILE
else
    if grep -q "Usage: ./ideviceerase -u <device_udid>" $STDERR_FILE && grep -q "Error: UDID is a mandatory argument." $STDERR_FILE; then
        echo "PASS"
    else
        echo "FAIL (Did not find expected usage or error message for missing -u in stderr)"
        cat $STDERR_FILE
    fi
fi
cleanup

# Test Case 6: UDID provided via --udid
echo -n "Test Case 6: --udid <udid> - "
./ideviceerase --udid $DUMMY_UDID > test_stdout.txt 2> $STDERR_FILE
exit_code=$?
if ! grep -q "Usage: ./ideviceerase" $STDERR_FILE && grep -q "Connecting to device $DUMMY_UDID..." test_stdout.txt ; then
    echo "PASS (Argument parsing seems correct for --udid, attempted connection)"
else
    echo "FAIL (Argument parsing incorrect for --udid or did not attempt connection as expected)"
    echo "Exit code: $exit_code"
    echo "--- STDOUT ---"
    cat test_stdout.txt
    echo "--- STDERR ---"
    cat $STDERR_FILE
fi
rm -f test_stdout.txt
cleanup

echo ""
echo "=============================================="
echo "All argument parsing tests completed."
echo "Note: For tests involving valid arguments, 'PASS' indicates correct parsing."
echo "The program will subsequently try to connect to a device, which may show errors"
echo "if no device is present or if usbmuxd is not running. These subsequent errors"
echo "are expected and do not indicate a parsing failure."
