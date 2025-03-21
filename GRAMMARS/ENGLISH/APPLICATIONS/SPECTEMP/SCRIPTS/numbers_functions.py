"""
Python:

words_to_number(string words).
"""

#####
## Module for numbers expressions
##
##   Xavier Tannier
##
#####


import string
import common_functions

units = {"one":1,
	 "two":2,
	 "three":3,
	 "four":4,
	 "five":5,
	 "six":6,
	 "seven":7,
	 "eight":8,
	 "nine":9,
	 "ten":10,
	 "eleven":11,
	 "twelve":12,
	 "thirteen":13,
	 "fourteen":14,
	 "fifteen":15,
	 "sixteen":16,
	 "seventeen":17,
	 "eighteen":18,
	 "nineteen":19,
	 "twenty":20,
	 "thirty":30,
	 "forty":40,
	 "fourty":40,  ## incorrect but used
	 "fifty":50,
	 "sixty":60,
	 "seventy":70,
	 "eighty":80,
	 "ninety":90
	 }
	
multiple = {"hundred":100,
	    "hundreds":100,
	    "thousand":1000,
	    "thousands":1000,
	    "billion":1000000,
	    "billions":1000000
	   }

## Converts a number written in letters
##   into a integer
def words_to_number(words):
	if common_functions.is_integer(words):
		return int(words)
	## 'replace' method does not work (why?)
	## words.replace("-", " ")

	## => replace '-' by ' ' "by hands"
	w=""
	for c in words:
		if c=='-':
			w += ' '
		else:
			w += c

	## split words
	tab_words = w.split()
	i=0
	n=0
	length = len(tab_words)
	## conversion
	for w in tab_words:
		if units.has_key(w):
			i = i + units[w]
			## for years
			##  nineteen eighty-one (without "thousand")
			if i>9 and i<30 and n == 0 and length <= 3:
				i = i * 100
		elif multiple.has_key(w):
			i = i * multiple[w]
		n=n+1
	return i


