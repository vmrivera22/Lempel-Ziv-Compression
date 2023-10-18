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

cp $file $original

./encode -i $original -o $compressed >> output.txt 2>>error.txt & pid1=$!
wait
./decode -i $compressed -o $decompressed >> output.txt 2>>error.txt & pid2=$!
wait

rc=0
msg=""

diff $original $decompressed > diff.txt
diff_val=$?
if [[ $diff_val -ne 0 ]]; then
    msg=$"${msg}Decompressed File differed from the original."
    rc=1
fi

new_files="$original $decompressed $compressed output.txt error.txt diff.txt"

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
    echo "Diff:"
    cat diff.txt
fi

cleanup $new_files

exit $rc
