#!/usr/bin/awk -f
##
## @file zero_width_parser.awk
## @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
## @version 1.0
## @date 2025
## @license LGPLv3.0
## @brief Generate table of zero width chars from the Unicode Character Database.
##
## Zero width categories: Mn, Me, Cf
## Exceptions:
##   - Soft hyphen (00AD)
##
BEGIN {
	FS = ";"

	# Global vars
	zero_flag = 0
	first = 0
	last = 0
}

# Ignore comment and empty lines
NF == 0 {
	next
}

{
	value = $1
	category = $3

	# Check if value is zero width
	is_zero = (category == "Mn" || category == "Me" || category == "Cf") &&
			value != "00AD"

	# Start of an interval
	if (!zero_flag && is_zero) {
		zero_flag = 1

		first = value
		last = value
	}

	# Continuing an interval
	if (zero_flag && is_zero) {
		last = value
	}

	# End of an interval
	if (zero_flag && !is_zero) {
		zero_flag = 0

		# Generate struct value
		print "{ 0x" first ", 0x" last " },"
	}
}

END {
	# End interval, if present
	if (zero_flag) {
		# Generate struct value
		print "{ 0x" first ", 0x" last " },"
	}
}
