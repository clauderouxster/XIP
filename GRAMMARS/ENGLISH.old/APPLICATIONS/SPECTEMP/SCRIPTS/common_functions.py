"""
Python:

printd(),
printp(),
printu().

"""

import sys
import re

## return 1 if the string is an integer
def is_integer(s):
	if re.match("[0-9]+$", s):
		return 1
	else:
		return 0


## return 1 if the string is a float
def is_float(s):
	dots = s.find(".")
	if dots == -1:
		return is_integer(s)
	else:
		dots2 = s.find(".", dots + 1)
		if dots2 == -1:
			return is_integer(s[:dots]) and is_integer(s[dots+1:])
		else:
			return false



## for debuging
def printd():
	print "#####################"
	return 1
def printp():
	print "%%%%%%%%%%%%%%%%%%%%%"
	return 1
def printu():
	print "_____________________"
	return 1
	

## Printrule
def print_rule():
	rule = currentrule()
	message =  "\n";
	message += "------------- CURRENT RULE ------------\n"
	message += "XIP Rule number " + str(rule[0]) + ": \n"
	message += "\n"
	message += str(currentruletext(0))
	message += "   file " + str(rule[5]) + "\n"
	message += "   line " + str(rule[7]) + "\n"
	message += "---------------------------------------\n"
	message +=  "\n";
	sys.stderr.write(message)


## Error message
def err_msg(m1, m2="", m3="", m4=""):
	rule = currentrule()
	message =  "\n";
	message += "---------------- ERROR ----------------\n"
	message += m1 + " " + m2 + " " + m3 + " " + m4 + "\n"
	message += "\n"
	message += "XIP Rule number " + str(rule[0]) + ": \n"
	message += "\n"
	message += str(currentruletext(0))
	message += "   file " + str(rule[5]) + "\n"
	message += "   line " + str(rule[7]) + "\n"
	message += "---------------------------------------\n"
	message +=  "\n";
	sys.stderr.write(message)

	##raise ExceptionGenError, message


