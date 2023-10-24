#!/bin/bash

source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ `check_dir` -eq 1 ]]; then
    exit 1
fi

touch "original.txt" "output.txt" "outtemp.txt" "intemp.txt" "error.txt" "diff.txt"

file="test_files/small.txt"
original="original.txt"
decompressed="intemp.txt"
compressed="outtemp.txt"

chmod 740 $original $decompressed $compressed "output.txt" "error.txt" "diff.txt"

new_files="$original $decompressed $compressed output.txt error.txt diff.txt"

cp $file $original

rc=$!
if [[ rc -ne 0 ]]; then
    echo "Input test file does not exist."
    cleanup $new_files
    exit $rc
fi

valgrind --leak-check=yes -s --error-exitcode=1 --log-file=val_log.txt ./encode -i $original -o $compressed >> output.txt 2>>error.txt & pid1=$!
wait $pid1
valgrind --leak-check=yes -s --error-exitcode=1 --log-file=val_log2.txt ./decode -i $compressed -o $decompressed >> output.txt 2>>error.txt & pid2=$!
wait $pid2

new_files="$new_files "val_log.txt" "val_log2.txt""

msg=""

diff $original $decompressed > diff.txt
diff_val=$?
if [[ $diff_val -ne 0 ]]; then
    msg=$"${msg}Decompressed File differed from the original."
    rc=1
fi

grep -i "ERROR SUMMARY: 0 errors from 0 contexts" val_log.txt
err_encode=$?
grep -i "All heap blocks were freed -- no leaks are possible" val_log.txt
leak_encode=$?
grep -i "ERROR SUMMARY: 0 errors from 0 contexts" val_log2.txt
err_decode=$?
grep -i "All heap blocks were freed -- no leaks are possible" val_log2.txt
leak_decode=$?

if [ $err_encode -ne 0 ] || [ $leak_encode -ne 0 ]; then
    msg=$"${msg}Valgrind detected an Error or Memory Leak while compressing the file."
    rc=1
fi

if [ $err_decode -ne 0 ] || [ $leak_decode -ne 0 ]; then
    msg=$"${msg}Valgrind detected an Error or Memory Leak while decompressing the file."
    rc=1
fi

if [[ $rc -eq 0 ]]; then
    echo "Compression and Decompression Successful."
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "Error:"
    cat error.txt
    echo "--------------------------------------------------------------------------------"
    echo "STDOUT:"
    cat output.txt
    echo "--------------------------------------------------------------------------------"
    echo "valgrind's output:"
    if [ $err_encode -ne 0 ] || [ $leak_encode -ne 0 ]; then
    cat val_log.txt
    fi
    if [ $err_decode -ne 0 ] || [ $leak_decode -ne 0 ]; then
    cat val_log2.txt
    fi
    echo "--------------------------------------------------------------------------------"
    echo "Diff:"
    cat diff.txt
fi

cleanup $new_files

exit $rc
