#!/usr/bin/awk -f
##
## @file east_asian_parser.awk
## @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
## @version 1.0
## @date 2025
## @license LGPLv3.0
## @brief Generate table from the EastAsianWidth Unicode data file.
##
## East Asian Width Reference from Unicode:
## https://www.unicode.org/reports/tr11/
##
## Full width codes (size 2): W, F, A
## Half width codes (size 1): N, Na, H, A
##
## However A is treated as half-width here.
##
BEGIN {
	FS = ";"
}

# Ignore comment and empty lines
/^#/ || NF == 0 {
	next
}

{
	# Remove line-end comment
	sub(/#.*/, "")

	# Filter half width codes
	code = trim($2)
	if (code == "N" || code == "Na" || code == "H" || code == "A") {
		next
	}

	# Split range
	n = split($1, range, /\.\./)
	for (i in range) {
		range[i] = trim(range[i])
	}

	# Generate struct value
	if (n == 1) {
		print "{ 0x" range[1] ", 0x" range[1] " },"
	} else if (n == 2) {
		print "{ 0x" range[1] ", 0x" range[2] " },"
	} else {
		print "Unable to parse file" > /dev/stderr
		exit
	}
}

# Helper function: trim leading and trailing spaces
function trim(s) {
	sub(/^[ \t\r\n]+/, "", s)
	sub(/[ \t\r\n]+$/, "", s)
	return s
}
