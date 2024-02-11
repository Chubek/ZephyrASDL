#!/usr/bin/awk -f

BEGIN {
    if (ARGC != 2) {
        print "Usage: awk -f generate_header.awk <header_guard_name> <input_file>"
        exit 1
    }
    header_guard = ARGV[1]
    delete ARGV[1]
    
    print "#ifndef " header_guard
    print "#define " header_guard "\n"
    print "// Configuration Constants\n"
}

{  
    if (length($0) < 2) {
 	next;
    }

    # Extract the key and value from each line
    key = $1
    value = $2
    
    # Remove trailing 'H' from value and convert to hexadecimal
    sub(/H$/, "", value)
    value = "0x" value
    
    # Output the #define statement
    printf "#define %s %s\n", key, value
}

END {
    print "\n#endif /* " header_guard " */"
}

