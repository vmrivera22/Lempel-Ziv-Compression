#!/bin/bash

for test in `ls test_scripts/*.sh`; do
    if [ "$test" != "test_scripts/utils.sh" ]; then
    	echo "------------------------------------------------------------------------"
		printf "$test:\n"
		echo "------------------------------------------------------------------------"

		output=$(./$test)
		return_code=$?
		if [ $return_code -eq 0 ]; then
	    	printf "SUCCESS\n\n"
		else
	    	printf "FAILED\n\n"
	    	printf "Return code: $return_code\n"
	    	printf "Output:\n$output\n\n"
		fi
    fi

done
