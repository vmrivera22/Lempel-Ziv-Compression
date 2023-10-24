# Lempel-Ziv-Compression
# Victor Rivera

Overview:
    Program to compress and decompress binary and text documents.

Sources Used:
    For this program I used psudo code provided by Professor Long for the decode.c and encode.c algorithms.

How to run the program:
    Type 'make' to create the program encode and decode executables. The program can be ran from the directory the executables are in. To compress a file use the command:
    "./encode <-option 1> ... <-optiion n>" 

    Options:
        -v: Display compression statistics.
        -i <input>: Specify input to compress (stdin by default).
        -o <output>: Specify output of compressed input (stdout by default).

    To decode a file use the command:
    "./decode <-option 1> ... <-option n>"

    Options:
        -v: Display decompression statistics.
        -i <input>: Specify input to decompress (stdin by default).
        -o <output>: Specify output of decompressed input (stdout by default).

    Enter 'make clean' to delete executables and .o files.

Files:
    word.c/.h:
        File contains functions used to create and manage a WordTable. A WordTable is an array of Word type data structures (can be codes or symbols). A Word data structure contains a character string 'sym' and its length. The WordTable is used for quicker lookups when decompressing/decoding a file.  

    trie.c/.h:
        File contains functions used to create and manage a trie data structure. The trie is used to store existing prefixes, or words used when compressing/encoding a file.

    io.c/h:
        File contains functions used to read/write (I/O) from and to files and buffers.

    code.h:
        File defines key codes used to encode and decode files. It defines STOP_CODE (end of decoding/encoding), EMPTY_CODE (code denoting the empty word), START_CODE (Starting code of new Words), and MAX_CODE (Maximum code).
    
    endian.h:
        File contains functions used to check if the order of bytes on the system is big or little endian. It also contains functions to swap the endianness of uint16_t, uint32_t, and uint64_t data types.

    encode.c/h:
        This file is the main file used for compression/encoding. The file contains the compressing algorithm and handels user input and options for compression.

    decode.c/h:
        This file is the main file used for decompression/decoding. The file contains the compressing algorithm and handels user input and options for decompression.