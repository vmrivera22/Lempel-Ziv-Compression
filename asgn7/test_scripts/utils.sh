cleanup() {
    for file in $@; do
	    rm -r $file
    done
}

check_dir() {
    if [ ! -f encode ]; then
	    echo "Could not find encode binary."
	    return 1
    fi
    if [ ! -f decode ]; then
	    echo "Could not find decode binary."
	    return 1
    fi
    return 0
}
