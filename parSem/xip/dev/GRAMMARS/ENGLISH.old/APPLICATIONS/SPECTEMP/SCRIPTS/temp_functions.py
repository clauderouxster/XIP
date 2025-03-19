"""
Python:

fail(),
init_dict(dictionary tdict),
close_dict(string filename, int sentencenumber, dictionary tdict),
print_xip_dict(dictionary tdict),
print_sentencenumber(int sentencenumber),
print_node(#1),
print_dict(),
is_set(#1),
is_date(#1),
month_is_known(),
add_empty_date(#1, string str_type),
add_gmt(#1),
add_century_millennium(#0, #1, #2),
add_around_christ(#0, #1),
add_date_decade(#1, string str_with_part),
add_date_year(#1),
add_letters_year(#1, #0),
add_date_short_year(#1),
add_date_period(#1),
add_date_month(#1),
add_month_of_the_year(#1),
add_day_of_the_week(#1),
add_day_of_the_month(#1),
add_day_of_the_year(#1),
add_date_day(#1),
add_date_hours(#1, #0),
add_date_hours_and_minutes(#1, #0),
add_date_minutes(#1, #0),
add_date_seconds(#1, #0),
add_date_x(#1),
add_special_date(#0, string str_date, string str_type),
add_quarter(#1, #2, #0),
add_quarter_alone(#1, #0),
build_full_date(#1, #0),
add_duration_hours(#1, #0),
add_duration_minutes(#1, #0),
add_duration_seconds(#1, #0),
add_duration_half(#0, #1),
add_duration(#1, #2, #0, int numberAbovePlural),
add_duration_x(#1, #0),
add_duration_from_quant(#1, #2, #0),
add_anchor_type(#1, string str_type),
add_anchor_date(#0, string unit, int unit_length, string temp_rel, string temp_ref),
add_anchor_date_from_node(#0, string temp_ref),
add_time_with_to(#1, #2, #0),
add_time_with_split_char(#1, #0, string split_char),
check_date_interval(#1, #2),
check_year_interval(#1, #2),
check_days_interval(#1, #2),
check_numbers_interval(#1, #2),
add_interval(#1, #2, #0, string type1, string type2),
add_anchor_interval(#1, #2, #0),
branch_two_nodes(#1, #2, #0),
copy(#1, #2, string str_type, string str_temp_level),
del_entries(#1, string str_type),
key_exists(#1, string str_type),
has_value(#1, string str_type),
change_hour(#1, #2),
change_type(#1, string str_old_type, string str_new_type),
add_set(#0, string str_times),
add_set_implicit(#0, #1),
add_quant(#0, string str_quant),
add_freq(#1),
add_temporal_relation(#1),
add_explicit_temporal_relation(#1, string temp_rel, string temp_ref),
add_temp_ref(#1, string value),
add_inverse_temporal_relation(#1, #0, string _str_var),
set_to_date(#1),
date_to_set(#1),
keep_temp_level_date(#1),
inverse_anchor_date_relation(#1),
concat_durations(#1, #2, #3, #0),
concat_dates(#1, #2, #3, #0, string type1, string type2),
concat_3_dates(#1, #2, #3, #4, #0, string type1, string type2, string type3),
concat_4_dates(#1, #2, #3, #4, #5, #0, string type1, string type2, string type3, string type4),
merge_dates(#1, #2, #0, string type1, string type2),
merge_2_dates(#1, #2, #0, string type1, string type2),
merge_3_dates(#1, #2, #3, #0, string type1, string type2, string type3),
merge_4_dates(#1, #2, #3, #4, #0, string type1, string type2, string type3, string type4),
merge_durations(#1, #2, #0),
merge_2_durations(#1, #2, #0),
merge_3_durations(#1, #2, #3, #0),
merge_4_durations(#1, #2, #3, #4, #0),
merge_sets(#1, #2, #0),
merge_set_and_date(#1, #2, #0),
merge_set_and_anchor_date(#1, #2, #0),
merge_set_and_duration(#1, #2, #0),
anchor_duration_to_set(#1, #2, #0),
merge_duration_and_set(#1, #2, #0),
merge_anchor_date_and_duration(#1, #2, #0),
merge_anchor_date_and_date(#1, #2, #0),
merge_anchor_dates(#1, #2, #0),
merge_anchor_date_to_node(#1, #2, #0),
add_mod(#1, #2, #0, string str_type),
add_mod_explicit(#2, #0, string str_mod, string str_type),
add_mod_alone(#1),
add_coref(#1, string str_type),
duration_lt(#1, #2),
granularity_lt(#1, #2),
inverse_relation(string str_rel).

"""

#####
## Module for temporal expressions analysing
##
##   Xavier Tannier
##
#####
##
## add_date_year
## add_date_month
## add_date_day
##  ...
##
##   build a local representation of lexical temporal entries
##     (a year, a month, a day)
##
##   dates must have the following format:
##
##   +yyyymmddThhmmss-OdddWwwd
##
##   where:
##     - lower case letters are replaced by '-' (unknown) or a number
##     - upper case letters are kept
##     - the first character can be '-' (undefined), '+' (AC) or '-' (BC)
##     - the following 8 letters represent the date
##     - the 6 letters after T represent the time
##     - the last character before the O must be replaced by Z
##           if the time is in GMT 
##     - the three letters after O represent the rank of the day in the year
##               e.g.: 2006----T------O245W--- is the 245th day of 2006
##     - the three letters efter W represent the day of the week
##               e.g.: 2006----T------O---W141 is the 1st day (Monday)
##                                         of the 14th week of 2006
#####
##
## merge_dates(#1, #2, #0, dates)
##   merges dates represented by nodes #1 and #2 in #0
##
## Important:
##     - the merging does NOT check the consistency: it is possible
##         to merge a Wednesday with a date even if it's a Monday in the real calendar
##     - but you cannot merge 2006 with 2007.
#####
##
## Durations must have the following format:
##
##   PyYmMwWdDThHmMsS
##     - lower case letters are replaced by '-' (unknown) or a number
##     - upper case letters are kept
##     - eg P5Y6M0W3DT15H10M20S :
##           5 years, 6 months, 3 days, 15 hours, 10 minutes, 20 seconds
##           weeks are not represented in days because of granularity treatment
##
#####

import sys
import re
import numbers_functions
import common_functions

#####
## Variables initialisation
#####
temp_dict = {}
temp_types = ["DATE", "DURATION", "SET", "ANCHOR"]
var_month_is_known = 0
##ExceptionGenError = "Exception"


def init_dict(tdict):
	global temp_dict
	temp_dict = tdict
	return 1

def print_xip_dict(tdict):
	print "------------ DICTIONARY -------------"
	print tdict

def close_dict(pathname, sentencenumber, tdict):
	global var_month_is_known
	global temp_dict
	if pathname == "":
		pathname = "stdinput"
	sentencenumber = sentencenumber + 1
	key_list = temp_dict.keys()
	for key in key_list:
		## add pathname and sentence number		
		all_in_a_string="file:" + pathname + ";sentence:" + str(sentencenumber) + ";key:" + key + ";"
		subkey_list = temp_dict[key].keys()
		for subkey in subkey_list:
			all_in_a_string = all_in_a_string + subkey + ":" + temp_dict[key][subkey] + ";"
		## add node number	
		match = re.search("[0-9]+", key)
		node_number = key[match.start():match.end()]
		all_in_a_string = all_in_a_string + 'nodenumber:' + str(node_number)
		## update tdict
		tdict[key]=all_in_a_string
	var_month_is_known = 0
	temp_dict = {}
	return 1


def get_dict():
	global temp_dict
	return temp_dict


def print_sentencenumber(sentencenumber):
	print "----------------------------------------"
	print "Sentence number:", str(sentencenumber)
	print "----------------------------------------"
	return 1

def print_node(i):
	n=XipNode(i)
	print "Node", str(i), ":", n.lemma
	return 1

def print_dict():
	global temp_dict
	str_text = "<DICT>\n"
	key_list = temp_dict.keys()
	for key in key_list:
		str_att_list = ""
		temp_node = temp_dict[key]
		atts = temp_node.keys()
		for att in atts:
			str_att_list = str_att_list + att + "=\"" + temp_node[att] + "\"" + " "
		str_att_list = str_att_list[:-1]

		full_name = key
		node = "   <TEMP_NODE key=\"" + key + "\" " + str_att_list + "/>\n"
		str_text = str_text + node
	str_text = str_text + "</DICT>"
	# comment next line if no dic in output
 	print str_text
	return 1



#####
## Add a new entry in the temporal dictionary
#####
def set_entry(str_key, str_type, str_value):	
	global temp_dict
	if not str_type in temp_types:
		common_functions.err_msg("Error in set_entry,", str_type, "is not a known type.")
		return 0
	if str_value == None:
		return 0
	temp_dict[str_type.upper() + str_key] = {}
	temp_dict[str_type.upper() + str_key]["type"] = str_type
	temp_dict[str_type.upper() + str_key]["value"] = str_value
	print_dict()
	return 1


#####
## Add a new entry in the temporal dictionary
#####
def add_entry(str_key, str_dict):
	temp_dict[str_key] = str_dict
	return 1
	
	
#####
## Get a value corresponding to a key and a type
#####
def get_value(str_key, str_type):
	global temp_dict
	if not temp_dict.has_key(str_type.upper() + str_key):
		common_functions.err_msg("Error in get_value", str_key, "is not an existing key.")
		return None
	return temp_dict[str_type.upper() + str_key]["value"]



#####
## Get a value corresponding to a key
#####
def get_value_by_entire_key(str_key):
	global temp_dict
	if not temp_dict.has_key(str_key):
		common_functions.err_msg("Error in get_value", str_key, "is not an existing key.")
		return None
	return temp_dict[str_key]["value"]


#####
## Get an attribute corresponding to a key and a type
#####
def get_attribute(str_key, str_type, str_att):
	global temp_dict
	if not temp_dict.has_key(str_type.upper() + str_key):
		common_functions.err_msg("Error in get_attribute", str_type, str_key, "is not an existing key.")
		return None
	d = temp_dict[str_type.upper() + str_key]
	if not d.has_key(str_att):
		return None
	else:
		return d[str_att]



#####
## Set a value corresponding to a key and a type
#####
def set_value(str_key, str_type, str_value):
	return update_entry(str_key, str_type, "value", str_value)


#####
## Get attributes for a given key (value excepted)
#####
def get_attributes(str_key, str_type):
	global temp_dict
	if not temp_dict.has_key(str_type.upper() + str_key):
		common_functions.err_msg("Error in get_attributes,", str_type, str_key, "is not an existing key.")
		return None
	att = temp_dict[str_type.upper() + str_key]
	del att["value"]
	return att


#####
## Set attributes for a given key 
#####
def set_attributes(str_key, str_type, attributes):
	global temp_dict
	if not temp_dict.has_key(str_type.upper() + str_key):
		common_functions.err_msg("Error in set_attributes,", str_key, "is not an existing key.")
		return None
	temp_dict[str_type.upper() + str_key].update(attributes)
	print_dict()
	return 1

#####
## Get entries corresponding to a key and a type
#####
def get_entries(str_key, str_type):
	global temp_dict
	dates_list = {}
	## get all keys
	key_list = temp_dict.keys()
	for key in key_list:
		if re.match("^" + str_type.upper() + str_key + "[a-z\+]?$", key):
			dates_list[key] = temp_dict[key]
	return dates_list
		

#####
## Delete an entry
#####
def del_entry(str_key, str_type):
	global temp_dict
	if not temp_dict.has_key(str_type.upper() + str_key):
		common_functions.err_msg("Error in del_entry,", str_key, "is not an existing key.")
		return None
	del temp_dict[str_type.upper() + str_key]
	print_dict()
	return 1


#####
## Delete entries
#####
def del_entries(i, str_type):
	global temp_dict
	if str_type == "":
		str_type = "ALL"
	## get all keys
	key_list = temp_dict.keys()
	for key in key_list:
		if (str_type == "ALL" or key.startswith(str_type.upper())):
			if re.match("[a-zA-Z_]+" + str(i) + "[a-z\+]?", key):
				del temp_dict[key]
	print_dict()
	return 1


#####
## Update an existing entry in the temporal dictionary
#####
def update_entry(str_key, str_type, str_att_name, str_att_value):
	global temp_dict
	if not temp_dict.has_key(str_type.upper() + str_key):
		common_functions.err_msg("Error in update_entry,", str_key, "is not an existing key for", str_type.upper())
		return None
	## value: date or duration value
	## quant and freq correspond to TIMEML standard attributes
	## mod: start, beginning, ... see definition of feature t_mod in XIP
	##       to have a list of values
	## coref: yes or no : yes for "this month", ...
	## anchor_type: DATE or ANCHOR
	if not (str_att_name.lower() == "value" or \
		str_att_name.lower() == "quant" or \
		str_att_name.lower() == "freq" or \
		str_att_name.lower() == "mod" or \
		str_att_name.lower() == "coref" or \
		str_att_name.lower() == "temp_rel" or \
		str_att_name.lower() == "temp_ref" or \
		str_att_name.lower() == "anchor_type" or \
		str_att_name.lower() == "anchored_to1" or \
		str_att_name.lower() == "anchored_to2" or \
		str_att_name.lower() == "anchored_to"):
		common_functions.err_msg("Error in add_update_entry,", str_att_name, "is not a known attribute name.")
		return None
	temp_dict[str_type.upper() + str_key][str_att_name] = str_att_value
	print_dict()
	return 1

#####
## Change the type of the temporal expression
#####
def change_type(i, str_old_type, str_new_type):
	global temp_dict
	str_key = str(i)
	if not temp_dict.has_key(str_old_type.upper() + str_key):
		common_functions.err_msg("Error in change_type,", str_old_type  + str_key, "is not an existing key.")
		return 0
	## change type
	old = temp_dict[str_old_type.upper() + str_key]
	if old["type"] != str_old_type:
		common_functions.err_msg("Error in change_type, TYPE is not valid.")
		return 0
	str_new_type = str_new_type.upper()
	if not str_new_type in temp_types:
		common_functions.err_msg("Error in change_type,", str_new_type, "is not a known type.")
		return 0
	old["type"] = str_new_type
	## delete old entry
	del temp_dict[str_old_type.upper() + str_key]
	## update
	temp_dict[str_new_type.upper() + str_key] = old
	print_dict()
	return 1

	
#####
## Returns 1 if the node is a set (entry of type SET in the dictionary)
#####
def is_set(i1):
	global temp_dict
	if temp_dict.has_key("SET" + str(i1)):
		return 1
	else:
		return 0



#####
## Returns 1 if the node is a date (entry of type DATE in the dictionary)
#####
def is_date(i1):
	global temp_dict
	key_list = temp_dict.keys()
	for key in key_list:
		if re.match("DATE" + str(i1) + "[a-z\+]?", key):
			return 1
	return 0


#####
## Returns 1 if a month has already been created since the beginning of the document
#####
def month_is_known():
	global var_month_is_known
	return var_month_is_known



#####
## "create" functions
##
## Create dates or durations in the proper format
#####
##
## DATES
##
def create_empty_date():
	return "---------T-------O---W---"

def create_gmt():
	return "---------T------ZO---W---"

def create_around_christ(a):
	return a + "--------T-------O---W---"

def create_date_year(y):
	return "-" + y + "----T-------O---W---"

def create_date_month(m):
	## if one character, add a 0
	global var_month_is_known
	if len(m) == 1:
		m = "0" + m
	elif len(m) != 2:
		return None
	var_month_is_known = 1
	return "-----" + m + "--T-------O---W---"

def create_date_day(d):
	## if one character, add a 0
	if len(d) == 1:
		d = "0" + d
	elif len(d) != 2:
		return None
	return "-------" + d + "T-------O---W---"

def create_date_hours(h, pm_and_am = True):
	## if one character, add a 0
	if len(h) == 1:
		h = "0" + h
	elif len(h) != 2:
		return create_empty_date()
	if common_functions.is_integer(h) and pm_and_am == True and int(h) <= 12:
		return "---------T" + h + "-----O---W---" + "|" + "---------T" + str(int(h) + 12) + "-----O---W---"
	else:
		return "---------T" + h + "-----O---W---"


def create_date_minutes(m):
	## if one character, add a 0
	if len(m) == 1:
		m = "0" + m
	elif len(m) != 2:
		return None
	return "---------T--" + m + "---O---W---"

def create_date_seconds(s):
	## if one character, add a 0
	if len(s) == 1:
		s = "0" + s
	elif len(s) != 2:
		return None
	return "---------T----" + s + "-O---W---"

def create_day_of_the_year(d):
	return "---------T-------O" + d + "W---"

def create_date_week(w):
	return "---------T-------O---W" + w + "-"

def create_day_of_the_week(d):
	return "---------T-------O---W--" + d

#####
## DURATIONS
#####
def create_empty_duration():
	return "P0Y0M0W0DT0H0M0S"

def create_duration_years(y):
	return "P" + y + "Y0M0W0DT0H0M0S"

def create_duration_months(m):
	return "P0Y" + m + "M0W0DT0H0M0S"

def create_duration_weeks(w):
	return "P0Y0M" + w + "W0DT0H0M0S"

def create_duration_days(d):
	return "P0Y0M0W" + d + "DT0H0M0S"

def create_duration_hours(h):
	return "P0Y0M0W0DT" + h + "H0M0S"

def create_duration_minutes(m):
	return "P0Y0M0W0DT0H" + m + "M0S"

def create_duration_seconds(s):
	return "P0Y0M0W0DT0H0M" + s + "S"

def create_duration(period, str_number):
        ## seconds
	if period.lower() == "s":
		duration = create_duration_seconds(str_number)
	## minutes
	elif period.lower() == "mi":
		duration = create_duration_minutes(str_number)
	## hours
	elif period.lower() == "h":
		duration = create_duration_hours(str_number)
	## weeks
	elif period.lower() == "w":
		duration = create_duration_weeks(str_number)
	## day
	elif period.lower() == "d":
		duration = create_duration_days(str_number)
	## month
	elif period.lower() == "mo":
		duration = create_duration_months(str_number)
	## year
	elif period.lower() == "y":
		duration = create_duration_years(str_number)
	## '+'
	elif period.lower() == "+":
		duration = create_empty_duration()
	else:
		common_functions.err_msg("Error, period not recognized in create_duration")
		return None
	return duration
		

#####
## "get" functions
##
## Returns parts of the date or duration
#####
## return date year
def get_date_year(date):
	return date[1:5]

## return date month
def get_date_month(date):
	return date[5:7]


## return date day
def get_date_day(date):
	return date[7:9]

## return date hours
def get_date_hours(date):
	return date[10:12]

## return date minutes
def get_date_minutes(date):
	return date[12:14]


## return date seconds
def get_date_seconds(date):
	return date[14:16]


## return date ord
def get_date_day_of_year(date):
	return date[18:21]


## return date week
def get_date_week(date):
	return date[22:24]


## return date day of the week
def get_date_day_of_the_week(date):
	return date[24:25]


## return duration years
def get_duration_years(duration):
	fm=re.search("Y", duration)
	if fm != None:
		y = duration[1:fm.start()]
	else:
		y = None
	return y

## return duration months
def get_duration_months(duration):
	fy=re.search("Y", duration)
	fm=re.search("M", duration)
	if fy != None and fm != None:
		m = duration[fy.end():fm.start()]
	else:
		m = None
	return m

## return duration weeks
def get_duration_weeks(duration):
	fb=re.search("M", duration)
	fe=re.search("W", duration)
	if fb != None and fe != None:
		r = duration[fb.end():fe.start()]
	else:
		r = None
	return r

## return duration days
def get_duration_days(duration):
	fb=re.search("W", duration)
	fe=re.search("D", duration)
	if fb != None and fe != None:
		d = duration[fb.end():fe.start()]
	else:
		d = None
	return d

## return duration hours
def get_duration_hours(duration):
	ft=re.search("T", duration)
	fh=re.search("H", duration)
	if ft != None and fh != None:
		h = duration[ft.end():fh.start()]
	else:
		h = None
	return h
		
## return duration minutes
def get_duration_minutes(duration):
	time = duration.split("T")[1]
	fh=re.search("H", time)
	fm=re.search("M", time)
	if fh != None and fm != None:
		m = time[fh.end():fm.start()]
	else:
		m = None
	return m

## return duration seconds
def get_duration_seconds(duration):
	time = duration.split("T")[1]
	fm=re.search("M", time)
	fs=re.search("S", time)
	if fm != None and fs != None:
		s = time[fm.end():fs.start()]
	else:
		s = None
	return s


#####
## "set" functions
##
## Change parts of the date or durations
#####
## change hours
def set_date_hours(date, hour):
	date = date[:10] + hour + date[12:]
	return date

## copy a temporal expression from node 1 to node 2
def copy(i1, i2, str_type_to_copy="ALL", str_temp_level="ALL"):

	global temp_dict
	if str_type_to_copy == "":
		str_type_to_copy = "ALL"
	if str_temp_level == "":
		str_temp_level = "ALL"
	temp_anchor = {}
	## get all keys
	key_list = temp_dict.keys()
	## for each key, if the (n-1) first characters
	##   compose i1, than node 1 must get the value
	ok = 0
	for key in key_list:
		str_type = None
		for temp_type in temp_types:
			if str_type_to_copy.upper() == "ALL" or temp_type == str_type_to_copy.upper():
				s = re.search(temp_type, key)
				if s != None and s.start() == 0:
					str_type = temp_type
					key = key[s.end():]
					break
		if str_type == None:
			if str_type_to_copy.upper() == "ALL":
				common_functions.err_msg("Error in copy with key", key)
				return 0
		if key == str(i1):
			temp_dict[str_type + str(i2)] = temp_dict[str_type + key]
			ok = 1
			## del ?
			if i1 != i2:
			#	del_entry(key, str_type)
				temp_anchor[str_type + key] = str_type + str(i2)
		elif key[:-1] == str(i1) and not common_functions.is_integer(key[-1]):
			ok = 1
			if str_temp_level.upper() == "ALL" or key[-1].upper() == str_temp_level.upper():
				temp_dict[str_type + str(i2) + key[-1]] = temp_dict[str_type + key]
			## del ?
			if i1 != i2:
			#	del_entry(key, str_type)
				temp_anchor[str_type + key] = str_type + str(i2) + key[-1]
	if ok == 0:
		common_functions.err_msg("Error in copy.")
		return 0
	## change anchored_to attributes if necessary
	key_list = temp_dict.keys()
	for key in key_list:
		if temp_dict[key].has_key("anchored_to"):
			anchored_to = temp_dict[key]["anchored_to"]
			if temp_anchor.has_key(anchored_to):
				temp_dict[key]["anchored_to"] = temp_anchor[anchored_to]
	##
	print_dict()
	return 1


#####
## Returns 1 if node 1 has an entry if the temporal dictionary
#####
def key_exists(i1, str_type = "ALL"):
	global temp_dict
	if str_type == "":
		str_type = "ALL"
	## get all keys
	key_list = temp_dict.keys()
	for key in key_list:
		if (str_type == "ALL" or key.startswith(str_type.upper())):
			if re.match("[a-zA-Z_]+" + str(i1) + "[a-z\+]?", key):
				return 1
	return 0
	


#####
## Returns 1 if node 1 has a non-null value or an anchored date
#####
def has_value(i1, str_type):
	global temp_dict
	## get all keys
	key_list = temp_dict.keys()
	for key in key_list:
		if re.match(str_type.upper() + str(i1) + "[a-z\+]?", key.upper()):
			elem = temp_dict[key]
			value = elem["value"]
			if value != None and \
			   value != create_empty_date() and \
			   value != create_empty_duration():
				return 1
			ad = elem["anchored_to"]
			if value != None:
				return 1
	return 0

	

#####
## "add" functions
##
## Create new dates according to the context
#####
## DATES
#####
## create an empty date format
def add_empty_date(i, str_type):
	set_entry(str(i) + str_type, "DATE", create_empty_date())
	return 1

## create a GMT time format
def add_gmt(i):
	t = get_feature(i, "TIMEZONE")
	## if feature timezone has value GMT
	if t == "GMT":
		set_entry(str(i) + "+", "DATE", create_gmt())
	## value "+", do nothing for the moment
	elif t == "+":
		set_entry(str(i) + t, "DATE", create_empty_date())
	else:
		common_functions.err_msg("Error in add_gmt")
		return 0
	return 1

## create a century or a millennium
def add_century_millennium(i0, i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	str_number = str(int(str_number) - 1)
	## the unit is given by the period of node 2
	unit = get_feature(i2, "PERIOD_LENGTH")
	if unit == "100":
		if len(str_number) == 1:
			str_number = "0" + str_number + "--"
		elif len(str_number) == 2:
			str_number = str_number + "--"
		else:
			return 0
	elif unit == "1000":
		if len(str_number) == 1:
			str_number = str_number + "---"
		else:
			return 0
	set_entry(str(i0) + "y", "DATE", create_date_year(str_number))
	return 1

## create a position before or after Christ
def add_around_christ(i0, i1):
	## the position (before or after) is given by the feature
	##   from
	f = get_feature(i1, "FORM")
	a = "-"
	if f.lower() == "fbefore":
		a = "="
	elif f.lower() == "fafter":
		a = "+"
	else:
		return 0
	set_entry(str(i0) + "+", "DATE", create_around_christ(a))
	return 1

	

## create a decade
def add_date_decade(i1, str_with_part):
	n = XipNode(i1)
	y = n.lemma
	## sixties (in letters)
	if not (re.match("[0-9]+$", y) or re.match("[0-9]+s$", y)):
		y = get_number_from_node(i1)
		if y == None:
			common_functions.err_msg("Error in add_date_decade")
			return 0
	## remove the "0s" at the end
	if str_with_part == "true" and y[-2:] == "0s":
		y = y[:-2]
	elif str_with_part == "false" and y[-1:] == "0":
		y = y[:-1]
	elif str_with_part == "unknown" and y[-2:] == "0s":
		y = y[:-2]
	elif str_with_part == "unknown" and y[-1:] == "0":
		y = y[:-1]
	else:
		return 0
	## representation building, according to the number of characters
	if len(y) == 3:
		str_y = y + "-"
	elif len(y) == 1:
		str_y = "--" + y + "-"
	else:
		return 0
	set_entry(str(i1) + "y", "DATE", create_date_year(str_y))
	return 1

## create a year
def add_date_year(i1):
	n=XipNode(i1)
	## the year is given by the lemma
	y=n.lemma
	str_y = ""
	## representation building, according to the number of characters
	if common_functions.is_integer(y) and len(y) <= 4:
		for i in range(4 - len(y)):
			str_y = str_y + "0"
		str_y = str_y + y
		set_entry(str(i1) + "y", "DATE", create_date_year(str_y))
		return 1
	else:
		return 0


## create a year from a full-letter word
## nineteen ninty one
def add_letters_year(i1, i0):
	str_number = get_number_from_node(i1)
	set_entry(str(i0) + "y", "DATE", create_date_year(str_number))
	return 1

	

## create a short year
def add_date_short_year(i1):
	n=XipNode(i1)
	## the year is given by the lemma
	y=n.lemma
	str_y = ""
	## representation building, according to the number of characters
	for i in range(4 - len(y)):
		str_y = str_y + "-"
	str_y = str_y + y
	set_entry(str(i1) + "y", "DATE", create_date_year(str_y))
	return 1

## create a specific period
def add_date_period(i1):
	p = get_feature(i1, "PERIOD")
	sp = get_feature(i1, "SPEC_PERIOD")
	## representation building
	if p.lower() == "h":
		set_entry(str(i1) + "p", "DATE", create_date_hours(sp))
	elif p.lower() == "mo":
		set_entry(str(i1) + "p", "DATE", create_date_month(sp))
	elif p.lower() == "d":
		set_entry(str(i1) + "p", "DATE", create_day_of_the_week(sp))
	else:
		common_functions.err_msg("Error in add_date_period")
		return 0
	return 1


## create a month
def add_date_month(i1):
	m = get_feature(i1, "MONTH")
	## representation building
	if m != None:
		## if '+', get the lemma (else keep m)
		if m == "+":
			n=XipNode(i1)
			m=n.lemma
		## representation building
		set_entry(str(i1) + "m", "DATE", create_date_month(m))
		return 1
	else:
		common_functions.err_msg("Error in add_date_month")
		return 0


## create a month (according to a number a not to a name of month)
def add_month_of_the_year(i1):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i1) + "m", "DATE", create_date_month(str_number))
	return 1


## create a day (of the week)
def add_day_of_the_week(i1):
	d = get_feature(i1, "DAY")
	## representation building
	if (d != None) and (d != "+"):
		## only one character possible
		if len(d) != 1:
			return 0
		set_entry(str(i1) + "d", "DATE", create_day_of_the_week(d))
		return 1
	else:
		common_functions.err_msg("Error in add_day_of_the_week")
		return 0

## create a day (of the month)
def add_day_of_the_month(i1):
	## the day is given by the lemma
	d = get_number_from_node(i1)
	if d == None or d == "0":
		return 0
	## representation building
	## if one character, add a 0
	if len(d) == 1:
		d = "0" + d
	elif len(d) != 2:
		return 0
	set_entry(str(i1) + "d", "DATE", create_date_day(d))
	return 1

## create a day (of the year)
def add_day_of_the_year(i1):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	## if one or two characters, add 0s
	if len(str_number) == 1:
		d = "00" + str_number
	elif len(str_number) == 2:
		d = "0" + str_number
	elif len(str_number) == 3:
		d = str_number
	else:
		common_functions.err_msg("Error in add_day_of_the_year")
		return 0
	set_entry(str(i1) + "o", "DATE", create_day_of_the_year(d))
	return 1

## create a day (of the month)
def add_date_day(i1):
	## get DAY feature
	d = get_feature(i1, "DAY")
	## representation building
	if d != None:
		## if '+', this is a day of the month (a number)
		if d == "+":
			return add_day_of_the_month(i1)
		## else (1..7]), it is a day of the week (Monday..Sunday)
		else:
			return add_day_of_the_week(i1)
	else:
		common_functions.err_msg("Error in add_date_day")
		return 0




## create a time with hours
def add_date_hours(i1, i0):
	## if a noun (noon, midnight...) get the 'hour' feature
	if XipNode(i1).pos == "NOUN":
		str_number = get_feature(i1, "HOUR")
	## else get the lemma
	else:
		str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## check that it's a number
	if (common_functions.is_integer(str_number)):
		h = create_date_hours(str_number, not (XipNode(i1).pos == "NOUN"))
	else:
		common_functions.err_msg("Error in add_date_hours.")
		return 0
	## representation building
	return set_entry(str(i0) + "t", "DATE", h)




## create a time with hours and minutes in the same "number"
##  eg: 1100 GMT
def add_date_hours_and_minutes(i1, i0):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	if not common_functions.is_integer(str_number) or len(str_number) != 4:
		common_functions.err_msg("Error in add_date_hours_and_minutes with number format.")
		return 0
	## time building
	h = create_date_hours(str_number[0:2], True)
	m = create_date_minutes(str_number[2:4])
	d = merge_date_formats(h, m)
	## representation building
	return set_entry(str(i0) + "t", "DATE", d)
	
		

## checks if a dig with form DDDDDDDD can be a date
##  e.g. 19981023
def is_full_date(str_number):
	## 8-char number
	if len(str_number) != 8:
		return 0
	if not common_functions.is_integer(str_number):
		return 0
	## year
	## only modern years, because such a form is for newswire
	##   (eg CNN)
	century = int(str_number[0:2])
	if not century == 19 or century == 20:
		return 0
	month = int(str_number[4:6])
	if not month <= 12:
		return 0
	day = int(str_number[6:])
	if not day <= 31:
		return 0
	return 1
	

## Build a full date from a dig with form DDDDDDDD
## calls is_full_date to check if it can really be a date, because it's highly ambiguous
def build_full_date(i1, i0):
	str_number = get_number_from_node(i1)
	if is_full_date(str_number):
		## get parts of the date
		year=str_number[0:4]
		month=str_number[4:6]
		day=str_number[6:8]
		## build each part
		y = create_date_year(year)
		m = create_date_month(month)
		d = create_date_day(day)
		date1 = merge_date_formats(y, m)
		date = merge_date_formats(date1, d)
		## rpresentation building
		return set_entry(str(i0) + "d", "DATE", date)
	else:
		return 0





#####
## DURATIONS
#####
## create a duration with millenniums
def add_duration_millenniums(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	str_number = mult(str_number, "1000")
	## representation building
	set_entry(str(i2), "DURATION", create_duration_years(str_number))
	return 1

## create a duration with centuries
def add_duration_centuries(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	str_number = mult(str_number, "100")
	## representation building
	set_entry(str(i2), "DURATION", create_duration_years(str_number))
	return 1

## create a duration with decades
def add_duration_decades(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	str_number = mult(str_number, "10")
	## representation building
	set_entry(str(i2), "DURATION", create_duration_years(str_number))
	return 1

## create a duration with years
def add_duration_years(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_years(str_number))
	return 1

## create a duration with months
def add_duration_months(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_months(str_number))
	return 1

## create a duration with weeks
def add_duration_weeks(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_weeks(str_number))
	return 1
	
## create a duration with days
def add_duration_days(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_days(str_number))
	return 1
	
## create a duration with hours
def add_duration_hours(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_hours(str_number))
	return 1

## create a duration with minutes
def add_duration_minutes(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_minutes(str_number))
	return 1

## create a duration with second
def add_duration_seconds(i1, i2):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i2), "DURATION", create_duration_seconds(str_number))
	return 1

## create a duration of 30 minutes
def add_duration_half(i0, i1):
	if get_feature(i1, "SPEC_PERIOD") != None:
		period = get_feature(i1, "PERIOD")
		if period == None:
			common_functions.err_msg("Error in add_duration_half for spec_period, period not found")
			return 0
		lower_period = get_lower_unit(period)
		if lower_period == None:
			common_functions.err_msg("Error in add_duration_half for spec_period, lower period not found")
			return 0
		str_number = "X"
		copy(i1, i0)
	else:
		lower_period = get_feature(i1, "LOWER_UNIT")
		lower_number = get_feature(i1, "LOWER_NUMBER")
		if lower_number == None or lower_period == None:
			common_functions.err_msg("Error in add_duration_half, special features not found.")
			return 0
		str_number = str(int(lower_number) / 2)
	duration = create_duration(lower_period, str_number)
	## representation building
	return set_entry(str(i0), "DURATION", duration)



## create a duration according to the type of node 2
def add_duration(i1, i2, i0, numberAbovePlural=0):
	## spec_period
	spec_period = get_feature(i2, "SPEC_PERIOD")
	## if no node for indicating number (like in 'each month')
	if i1 == -1:
		str_number = "1"
	else:
		str_number = get_number_from_node(i1, i2)
		if str_number == None:
##			common_functions.err_msg("Error str_number in add_duration")
			return 0
	## correction 1 => X if the word is plural
	pl = (get_feature(i2, "PL") != None)
	## number 1 but pl
	if str_number == "1" and pl:
		if numberAbovePlural == 0:
			str_number = "X"
	## number > 1 but sg -> fails (only for spec_period, else it can be an adj)
	if spec_period != None and \
	       str_number != "1" and \
	       str_number != "X" and \
	       not pl and \
	       numberAbovePlural == 0:
		return 0
	## type
	period = get_feature(i2, "PERIOD")
	if period == None:
		common_functions.err_msg("Error period in add_duration")
		return 0
	## duration
	str_period_length = get_feature(i2, "PERIOD_LENGTH")
	if str_period_length == None:
		common_functions.err_msg("Error period_length in add_duration")
		return 0
	str_key = str(i0)
	## if spec_period, special treatment
	if spec_period != None and pl:
		## different treatment if singular or plural
		## if singular: one Winter = 3 months
		##   keep period_length and period
		## if plural: 3 winters => value=WI, quant="every", freq="3X"
		## get the value that should have been created
		##   at the lexical level for the date represented
		##   by the spec_period
		##   spec period is a code in TIDES list
		if spec_period != "+":
			str_anchored_to = "DATE" + str(i2) + "p"
#			str_value = get_value(str(i2) + "p", "DATE")
#			if str_value == None:
#				common_functions.err_msg("    Error => add_duration")
#				return 0
			#del_entry(str(i2) + "p", "DATE")
		##   spec period is + (for mondays, januaries, ...)
		else:
			if period == "MO":
				str_anchored_to = "DATE" + str(i2) + "m"
#				str_value = get_value(str(i2) + "m", "DATE")
				#del_entry(str(i2) + "m", "DATE")
			else:
				str_anchored_to = "DATE" + str(i2) + period.lower()
#				str_value = get_value(str(i2) + period.lower(), "DATE")
				#del_entry(str(i2) + period.lower(), "DATE")
		str_value = create_empty_duration()
		set_entry(str_key, "SET", str_value) 
##		if str_number != "X":
##			update_entry(str_key, "SET", "quant", "EVERY")
		update_entry(str_key, "SET", "freq", str_number + "X")
		update_entry(str_key, "SET", "anchored_to", str_anchored_to)
		update_entry(str_key, "SET", "temp_rel", "DURING")
		update_entry(str_key, "SET", "temp_ref", "+")		
		## also a duration and a date (implicitely linked together)
		##  eg: "two nights" is also a duration of 2 days
		##      linked with a date NI.
		period_frequ_unit = get_feature(i2, "PERIOD_FREQU_UNIT")
		if period_frequ_unit == None:
			common_functions.err_msg("Error in add_duration, node", i2, "has no period_frequ_unit feature.")
			return 0
		## duration
		duration = create_duration(period_frequ_unit, str_number)
		set_entry(str_key, "DURATION", duration)
	else:
		## one night can also be a date ("a nice day, or maybe a night")
		if spec_period != None and not pl:
			copy(i2, i0)
                ## get the length of the duration
	        ## the length of the period is not defined
		if str_period_length == "+":
			str_number = "X"
		else:
			str_number = mult(str_number, str_period_length)
			if str_number == None:
				common_functions.err_msg("Error, problem with period_length, bad format in add_duration.")
				return 0
	        ## choice
		duration = create_duration(period, str_number)
		if duration == None:
			common_functions.err_msg("Error, problem when creating duration in add_duration")
			return 0
		set_entry(str_key, "DURATION", duration)
	return 1



		
## create a duration according to the type of node 2 (number = X)
def add_duration_x(i2, i0):
	return add_duration(-1, i2, i0, 0)


## create a duration from a special quantifier ("a couple of", ...)
def add_duration_from_quant(i1, i2, i0):
	str_number = get_feature(i1, "QUANT_NUMBER")
	if (str_number == "X"):
		return add_duration_x(i2, i0)
	else:
		if common_functions.is_integer(str_number):
	                ## type
			period = get_feature(i2, "PERIOD")
			if period == None:
				common_functions.err_msg("Error period in add_duration_from_quant")
				return 0
			duration = create_duration(period, str_number)
	                ## representation building
			return set_entry(str(i0), "DURATION", duration)
		else:
			common_functions.err_msg("Error, problem when creating duration from quant")
			return 0
	

## add an attribute "anchor_type" to the anchor date
def add_anchor_type(i, str_type):
	return update_entry(str(i), "ANCHOR", "anchor_type", str_type)

## create an anchor date, given the unit, unit length and direction
def add_anchor_date(i, str_unit, str_unit_length, str_temp_rel, str_temp_ref):
	if str_unit != "": 
		value = create_duration(str_unit, str_unit_length)
	else:
		value = create_empty_duration()
	if not has_feature(i, "SPEC_PERIOD"):
		str_key = str(i)
		if value == None:
			common_functions.err_msg("Error in add_anchor_date when creating value.")
			return 0
		set_entry(str_key, "ANCHOR", value)
		update_entry(str_key, "ANCHOR", "temp_rel", str_temp_rel)
		update_entry(str_key, "ANCHOR", "temp_ref", str_temp_ref)
	else:
		copy(i, i)
		set_entry(str(i), "ANCHOR", value)
		update_entry(str(i), "ANCHOR", "temp_rel", str_temp_rel)
		update_entry(str(i), "ANCHOR", "temp_ref", str_temp_ref)
		update_entry(str(i), "ANCHOR", "anchored_to", "DATE" + str(i) + "p")
	return 1

	


## create an anchor date from a node
def add_anchor_date_from_node(i, temp_ref):
	if has_feature(i, "SPEC_PERIOD"):
		add_date_period(i)
		set_entry(str(i), "ANCHOR", create_empty_duration())
		update_entry(str(i), "ANCHOR", "anchored_to", "DATE" + str(i) + "p")
	else:
	        ## get period
		period = get_feature(i, "PERIOD")
		if (has_feature(i, "SG")):
			duration = create_duration(period, "1")
		else:
			duration = create_duration(period, "X")		
		set_entry(str(i), "ANCHOR", duration)
	update_entry(str(i), "ANCHOR", "temp_rel", "PRESENT")
	update_entry(str(i), "ANCHOR", "temp_ref", temp_ref)
	return 1





## create an anchor date, given the unit, unit length and direction
##def add_anchor_date_from_duration(i0, str_temp_rel, str_temp_ref):
##	value = get_value(str(i0), "DURATION")
##	if value == None:
##		common_functions.err_msg("Error in add_anchor_date_from_duration")
##		return 0
##	set_entry(str(i0), "ANCHOR", value)
##	update_entry(str(i0), "ANCHOR", "temp_rel", str_temp_rel)
##	update_entry(str(i0), "ANCHOR", "temp_ref", str_temp_ref)
##	return 1
	

	



## create a time with minutes
def add_date_minutes(i1, i0):
	str_number = get_number_from_node(i1)
	if str_number == None:
		n=XipNode(i1)
		s = n.lemma
		if s == "quarter":
			str_number = "15"
		elif s == "half":
			str_number = "30"
		else:
			return 0
	## representation building
	set_entry(str(i0) + "t", "DATE", create_date_minutes(str_number))
	return 1
	
## create a time with seconds
def add_date_seconds(i1, i0):
	str_number = get_number_from_node(i1)
	if str_number == None:
		return 0
	## representation building
	set_entry(str(i0) + "t", "DATE", create_date_seconds(str_number))
	return 1

## create a date depending on the period of node i
def add_date_x(i):
	## get period
	period = get_feature(i, "PERIOD")
	if period == None:
		common_functions.err_msg("Error in add_date_x with period.")
		return 0
        ## seconds
	if period.lower() == "s":
		date = create_date_seconds(str_number)
       	## minutes
	elif period.lower() == "mi":
		date = create_date_minutes(str_number)
       	## hours
	elif period.lower() == "h":
		date = create_date_hours(str_number)
       	## weeks
	elif period.lower() == "w":
		date = create_date_weeks(str_number)
       	## day
	elif period.lower() == "d":
		date = create_date_days(str_number)
	## month
	elif period.lower() == "mo":
		date = create_date_months(str_number)
	## year
	elif period.lower() == "y":
		date = create_date_years(str_number)
      	## '+'
	elif period.lower() == "+":
		date = create_empty_date()
	else:
		common_functions.err_msg("Error, period not recognized in add_date_x")
		return 0
	return 1


## add a special date
def add_special_date(i, str_date, str_type):
	return set_entry(str(i) + str_type, "DATE", str_date)



## add a year quarter
def add_quarter(i1, i2, i0):
	str_number1 = get_number_from_node(i1)
	if str_number1 == None:
		return 0
	if int(str_number1) > 4:
		common_functions.err_msg("Problem in add_quarter, number should be < 5.")
		return 0
	str_number2 = get_number_from_node(i2)
	if str_number2 == None:
		return 0
        ## representation building
	d1 = create_date_month("Q" + str_number1)
	d2 = create_date_year(str_number2)
	d = merge_strings(d1, d2)
	set_entry(str(i0) + "m", "DATE", d)
	return 1
	

## add a year quarter without year
def add_quarter_alone(i1, i0):
	str_number1 = get_number_from_node(i1)
	if str_number1 == None:
		return 0
	if int(str_number1) > 4:
		common_functions.err_msg("Problem in add_quarter, number should be < 5.")
		return 0
        ## representation building
	d1 = create_date_month("Q" + str_number1)
	set_entry(str(i0) + "m", "DATE", d1)
	return 1

	

## create a time built with "to"  (ten to three, ...)
def add_time_with_to(i1, i2, i0):
	## get minutes and hours
	str_min = get_number_from_node(i1)
	## if i2 is a noun (noon, midnight...) get the 'hour' feature
	if XipNode(i2).pos == "NOUN":
		str_hour = get_feature(i2, "HOUR")
	## else get the lemma
	else:
		str_hour = get_number_from_node(i2)
	if str_min == None:
		n=XipNode(i1)
		s = n.lemma
		if s == "quarter":
			int_min = 15
		elif s == "half":
			int_min = 30
		else:
			return 0
	else:
		int_min = int(str_min)
	if str_hour == None:
		return 0
	else:
		int_hour = int(str_hour)
	if int_min > 20:
		return 0
	## hour is hour - 1
	## minutes are 60 - minutes
	int_hour = int_hour - 1
	int_min = 60 - int_min
	## build representation
	str_date_hour = create_date_hours(str(int_hour), not (XipNode(i2).pos == "NOUN"))
	str_date_min = create_date_minutes(str(int_min))
	date = merge_date_formats(str_date_hour, str_date_min)
	set_entry(str(i0) + "t", "DATE", date)
	return 1

	
## create a time built with colons or dots  (11:45, 11:45:35)
def add_time_with_split_char(i1, i0, split_char):
	n=XipNode(i1)
	s1 = n.lemma
	list1 = s1.split(split_char)
	str_date_hour = create_date_hours(str(list1[0]))
	str_date_min = create_date_minutes(str(list1[1]))
	if len(list1) == 2:
		date = merge_date_formats(str_date_hour, str_date_min)
	elif len(list1) == 3:
		str_date_sec = create_date_seconds(str(list1[2]))
		date = merge_date_formats(str_date_hour, str_date_min)
		date = merge_date_formats(date, str_date_sec)
	else:
		return 0
	set_entry(str(i0) + "t", "DATE", date)
	return 1


#####
## "change" functions
##
## change a part of a date according to the context
#####
## change the hour accroding to the post-hour AM or PM
def change_hour(i1, i2):
	## get post-hour type
	m = get_feature(i2, "POSTHOUR")
	date = get_value(str(i1) + "t", "DATE")
	if date == None:
		common_functions.err_msg("Error in change_hour,", str(i1), "has no 'time' value")
		return 0
	## this date is a disjunction of two times
	## separated by |
	## the first one is AM
	## the second one is PM
	dates = date.split("|")
	if not len(dates) == 2:
		common_functions.err_msg("Error in change_hour, date format not recognized.")
		return 0
	## change if needed
	## PM: add 12 hours
	if m == "PM":
		hour = get_date_hours(dates[0])
		## check that it's a number
		if (re.match("[0-9]", hour[0]) and re.match("[0-9]", hour[1])):
			## add 12 hours
			hour = str(int(hour) + 12)
			dates[0] = set_date_hours(dates[0], hour)
			## Here two dates should be the same
			if dates[0] != dates[1]:
				common_functions.err_msg("Error in change_hour, the format is not the one I planned.")
				return 0
			set_value(str(i1) + "t", "DATE", dates[0])
		else:
			common_functions.err_msg("Error in change_hour.")
			return 0
	## AM: Keep only the first date
	elif m == "AM":
		set_value(str(i1) + "t", "DATE", dates[0])
	else:
		## neither AM nor PM, error
		common_functions.err_msg("Error when treating post-hour !!")
		return 0
	return 1


	
	

#####
## Add a set introducing a frequency
#####
def add_set(i, str_times):
	set_entry(str(i), "SET", create_empty_duration())
	update_entry(str(i), "SET", "freq", str_times + "X")
	return 1

#####
## Add a set introducing a frequency according to the number of node i
#####
def add_set_implicit(i0, i1):
	str_number = get_number_from_node(i1)
	if str_number == None:
		common_functions.err_msg("Error in add_set_implicit")
		return 0
	return add_set(i0, str_number)


#####
## Add a set introducing a quant frequency
#####
def add_quant(i, str_quant):
	if not key_exists(i, "SET"):
		set_entry(str(i), "SET", create_empty_duration())
	update_entry(str(i), "SET", "quant", str_quant)
	return 1


#####
## Add a set introducing a frequency depending on i
#####
def add_freq(i):
	## get frequency information
	period = get_feature(i, "PERIOD_FREQU_UNIT")
	frequ  = get_feature(i, "PERIOD_FREQU")
	if period == None or frequ == None:
		common_functions.err_msg("Error in add_freq with, specific features not found.")
		return 0
	add_set(i, frequ)
	add_quant(i, "EACH")
	if period != "X":
		duration = create_duration(period, "1")
		update_entry(str(i), "SET", "value", duration)
	## spec_period (eg nightly)
	spec_period = get_feature(i, "SPEC_PERIOD")
	if spec_period != None:
		period = get_feature(i, "PERIOD")
		if period == None:
			common_functions.err_msg("\"period\" feature is missing in add_freq.")
			return 0
		add_date_period(i)
		update_entry(str(i), "SET", "anchored_to", "DATE" + str(i) + "p")
	return 1


	

	
#####
## Add a duration introducing a temporal relation
#####
def add_temporal_relation(i):
	## get temporal relation and reference
	rel = get_feature(i, "TEMP_REL")
	ref = get_feature(i, "TEMP_REF")
	mod = get_feature(i, "T_MOD")
	if rel == None:
		common_functions.err_msg("Error temporal relation in add_temporal_relation.")
		return 0
	if ref == None:
		ref = "+"
	set_entry(str(i), "ANCHOR", create_empty_duration())
	update_entry(str(i), "ANCHOR", "temp_rel", rel)
	update_entry(str(i), "ANCHOR", "temp_ref", ref)
	if mod != None:
		update_entry(str(i), "ANCHOR", "mod", mod)
	return 1



#####
## Add a duration introducing a temporal relation with explicit temp_rel and temp_ref
#####
def add_explicit_temporal_relation(i, temp_rel, temp_ref):
	## get temporal relation and reference
	set_entry(str(i), "ANCHOR", create_empty_duration())
	update_entry(str(i), "ANCHOR", "temp_rel", temp_rel)
	update_entry(str(i), "ANCHOR", "temp_ref", temp_ref)
	return 1


#####
## changes or adds a temp_ref
#####
def add_temp_ref(i, value):
	update_entry(str(i), "ANCHOR", "temp_ref", value)
	return 1
	

#####
## Add a duration introducing a temporal relation
##   that is the inverse of the one corresponding to the node 1
##   useful for expression like "not after, etc..."
#####
def add_inverse_temporal_relation(i1, i2):
	global _str_var
	## get temporal relation and reference
	rel = get_feature(i1, "TEMP_REL")
	ref = get_feature(i1, "TEMP_REF")
	if rel == None:
		common_functions.err_msg("Error temporal relation in add_temporal_relation.")
		return 0
	if ref == None:
		ref = "+"
	_str_var = inverse_relation(rel)
	if _str_var == None:
		common_functions.err_msg("Error in add_temporal_relation, inverse relation not found for", rel)
	set_entry(str(i2), "ANCHOR", create_empty_duration())
	update_entry(str(i2), "ANCHOR", "temp_rel", _str_var)
	update_entry(str(i2), "ANCHOR", "temp_ref", ref)
	return 1



#####
## Change a set into a date with the same value
#####
def set_to_date(i):
	anchor = get_attribute(str(i), "SET", "anchored_to")
	if anchor == None:
		date = get_value(str(i), "SET")
		if date == None:
			common_functions.err_msg("Problem in set_to_date, key", str(i), "not found for a SET")
			return 0
		del_entry(str(i), "SET")
		return set_entry(str(i), "DATE", date)
	else:
		del_entry(str(i), "SET")
		add_anchor_date(i, "", 0, "DURING", "+")
		return update_entry(str(i), "ANCHOR", "anchored_to", anchor)
		


def keep_temp_level_date(i):
	chtype = get_feature(i, "TEMP_LEVEL").lower()
	if chtype == None:
		common_functions.err_msg("Problem in keep_temp_level_date, temp_level not found")
		return 0
	dates = get_entries(str(i), "DATE")
	key_list = dates.keys()
	ok = 0
	for key in key_list:
		if key[-1] == chtype:
			ok = 1
		else:
			del_entry(key[4:], "DATE")
	return ok


#####
## Change a date into a set with the same value
#####
def date_to_set(i):
	add_quant(i, "XX")
	update_entry(str(i), "SET", "temp_ref", "+")
	update_entry(str(i), "SET", "temp_rel", "DURING")
	level = get_feature(i, "TEMP_LEVEL").lower()
	anchored_to = "DATE" + str(i) + level
	update_entry(str(i), "SET", "anchored_to", anchored_to)
	return 1
	
	

#####
## Return a new anchor with relation inversed
#####
def inverse_anchor_date_relation(i1):
	temp_rel = get_attribute(str(i1), "ANCHOR", "temp_rel")
	i_temp_rel = inverse_relation(temp_rel)
	copy(i1, i1 + 1000, "ANCHOR")
	set_entry(str(i1), "ANCHOR", create_empty_duration())
	update_entry(str(i1), "ANCHOR", "temp_rel", i_temp_rel)
	update_entry(str(i1), "ANCHOR", "temp_ref", "r")
	update_entry(str(i1), "ANCHOR", "anchored_to", "ANCHOR" + str(i1 + 1000))
	return 1

	
## used in check_*_interval
def compare(elem1, elem2):
	if len(elem1) != len(elem2):
		return None
	else:
		for i in range(len(elem1)):
			if elem1[i] != "-" and elem2[i] != "-" and elem1[i] != elem2[i]:
				if int(elem1[i]) > int(elem2[i]):
					return 1
				elif int(elem1[i]) < int(elem2[i]):
					return -1
		return 0


	
#####
##  check that the two date nodes can compose an interval
##     (for the moment just same day date)
#####
def check_date_interval(i1, i2):
	## get dates to merge from node numbers
	printd()
	level1 = get_feature(i1, "TEMP_LEVEL").lower()
	level2 = get_feature(i2, "TEMP_LEVEL").lower()
	date1 = get_value(str(i1) + level1, "DATE")
	date2 = get_value(str(i2) + level2, "DATE")
	print date1
	print date2
	## years
	year1 = date1[1:5]
	year2 = date2[1:5]
	print year1
	print year2
	c = compare(year1, year2)
	if c == -1:
		return 1
	elif c == 1:
		return 0
	else:
		## months
		month1 = date1[5:7]
		month2 = date2[5:7]
		print month1
		print month2
		c = compare(month1, month2)
		if c == -1:
			return 1
		elif c == 1:
			return 0
		else:
			## days
			day1 = date1[7:9]
			day2 = date2[7:9]
			print day1
			print day2
			c = compare(day1, day2)
			print c
			if c == -1:
				return 1
			elif c == 1:
				return 0
			else:
				return 1



	

#####
##  check that the two date nodes can compose an interval
##    (just first year before second one)
#####
def check_year_interval(i1, i2):
	## get dates to merge from node numbers
	level1 = get_feature(i1, "TEMP_LEVEL").lower()
	level2 = get_feature(i2, "TEMP_LEVEL").lower()
	date1 = get_value(str(i1) + level1, "DATE")
	date2 = get_value(str(i2) + level2, "DATE")
	year1 = date1[1:5]
	year2 = date2[1:5]
	for i in range(4):
		if year1[i] != "-" and year2[i] != "-" and year1[i] != year2[i]:
			if int(year1[i]) > int(year2[i]):
				return 0
			else:
				return 1
	return 1

	

#####
##  check that the two day nodes can compose an interval
##    (first day before second one)
#####
def check_days_interval(i1, i2):
	## get dates to merge from node numbers
	date1 = get_value(str(i1) + "d", "DATE")
	date2 = get_value(str(i2) + "d", "DATE")
	day1 = date1[7:9]
	day2 = date2[7:9]
	if not common_functions.is_integer(day1) or not common_functions.is_integer(day2):
		common_functions.err_msg("Problem in check_day_interval, both day part should be integers.")
		return 0
	else:
		if int(day1) >= int(day2):
			return 0
		else:
			return 1



#####
##  check that the two numbers can compose an interval
##    (first one lower than second one)
#####
def check_numbers_interval(i1, i2):
	str_number1 = get_number_from_node(i1)
	str_number2 = get_number_from_node(i1)		
	## get dates to merge from node numbers
	if not common_functions.is_integer(str_number1) or not common_functions.is_integer(str_number2):
		common_functions.err_msg("Problem in check_number_interval, both elements should be integers.")
		return 0
	else:
		if int(str_number1) >= int(str_number2):
			return 0
		else:
			return 1

		

#####
## Create an interval between two dates
#####
def add_interval(i1, i2, i0, type1, type2):
	return concat_date_formats(i1, i2, i0, type1, type2, "/")


#####
## Create an interval between two anchor dates or one anchor date and a date
#####
def add_anchor_interval(i1, i2, i0):
        ## identitfy node types
	anchor1 = get_feature(i1, "C_ANCHOR")
	anchor2 = get_feature(i2, "C_ANCHOR")
	## anchor
	set_entry(str(i0), "ANCHOR", create_empty_duration())
	update_entry(str(i0), "ANCHOR", "temp_rel", "between")
	update_entry(str(i0), "ANCHOR", "temp_ref", "r")
	if anchor1 == "+":
		update_entry(str(i0), "ANCHOR", "anchored_to1", "ANCHOR" + str(i1))
	else:
		date_list = get_entries(str(i1), "DATE")
		update_entry(str(i0), "ANCHOR", "anchored_to1", date_list.items()[0][0])
	if anchor2 == "+":
		update_entry(str(i0), "ANCHOR", "anchored_to2", "ANCHOR" + str(i2))
	else:
		date_list = get_entries(str(i2), "DATE")
		update_entry(str(i0), "ANCHOR", "anchored_to2", date_list.items()[0][0])
	return 1



#####
## Create a branch anchor date
#####
def branch_two_nodes(i1, i2, i0):
        ## identitfy node types
	anchor1 = get_feature(i1, "C_ANCHOR")
	anchor2 = get_feature(i2, "C_ANCHOR")
	## anchor
	set_entry(str(i0), "ANCHOR", create_empty_duration())
	update_entry(str(i0), "ANCHOR", "temp_rel", "branch")
	update_entry(str(i0), "ANCHOR", "temp_ref", "r")
	if anchor1 == "+":
		update_entry(str(i0), "ANCHOR", "anchored_to1", "ANCHOR" + str(i1))
	else:
		date_list = get_entries(str(i1), "DATE")
		update_entry(str(i0), "ANCHOR", "anchored_to1", date_list.items()[0][0])
	if anchor2 == "+":
		update_entry(str(i0), "ANCHOR", "anchored_to1", "ANCHOR" + str(i2))
	else:
		date_list = get_entries(str(i2), "DATE")
		update_entry(str(i0), "ANCHOR", "anchored_to2", date_list.items()[0][0])
	return 1



## concat date formats
def concat_date_formats(i1, i2, i0, type1, type2, char):
	## granularity levels
	if type1 == "":
		type1 = get_feature(i1, "TEMP_LEVEL").lower()
	if type2 == "":
		type2 = get_feature(i2, "TEMP_LEVEL").lower()
	if type1 != type2:
		common_functions.err_msg("Problem in add_interval, both date should have the same type.")
		return 0
	type0 = get_feature(i0, "TEMP_LEVEL")
	if type0 == None:
		common_functions.err_msg(" Error", "No temp_level feature defined for new date (in concat_date_formats)")
		return 0
	else:
		type0 = type0.lower()
	## get dates to merge from node numbers
	date1 = get_value(str(i1) + type1, "DATE")
	date2 = get_value(str(i2) + type2, "DATE")
	## delete old entries
	del_entry(str(i1) + type1, "DATE")
	del_entry(str(i2) + type2, "DATE")
	## concat intervals
	## if split char is "/" (interval)
	##  we must re-order dates if necessary
	##  eg. : date1 is 10:00 or 22:00 and date2 11:00 or 23:00
	##     newdate must be 10:00 to 11:00 or 22:00 to 23:00
	##      and NOT 10:00 or 22:00 to 11:00 or 23:00
	##   also (still for intervals)
	if char == "/":
		new_date = ""
		dates1 = date1.split("|")
		dates2 = date2.split("|")
		l1 = len(dates1)
		l2 = len(dates2)
		if l1 != l2:
			## 1970 or 1973 to 1981 (found in corpus!)
			## -> 1970 to 1981 or 1973 to 1981
			for i in range(l1):
				for j in range(l2):
					new_date = new_date + dates1[i] + "/" + dates2[j] + "|"
		else:
			for i in range(l1):
				new_date = new_date + dates1[i] + "/" + dates2[i] + "|"
		new_date = new_date[:-1]
		
#			else:
#				new_date = new_date + dates1[i] + "/" + dates2[-1] + "|"
#			if i == l1 - 1 and i < l2 - 1:
#				j = i + 1
#				while j < l2:
	else:
		new_date = date1 + char + date2
	set_entry(str(i0) + type0, "DATE", new_date)
	return 1


		
## merge strings
def merge_strings(s1, s2):
	## must have the same length
	if len(s1) != len(s2):
		return None
	new_s = ""
	## merge
	for i in range(len(s1)):
		## same character or s2[i] undefined
		if (s1[i] == s2[i]) or (s2[i] == '-'):
			new_s = new_s + s1[i]
		## s1[i] undefined
		elif s1[i] == '-':
			new_s = new_s + s2[i]
		## all defined, one character and one integer
		##    keep the integer
		## (eg: 09 (September) more specific than FA (Automn))
		elif (re.match("[0-9]", s1[i]) and not re.match("[0-9]", s2[i])):
			new_s = new_s + s1[i]
		elif (not re.match("[0-9]", s1[i]) and re.match("[0-9]", s2[i])):
			new_s = new_s + s2[i]
		## all defined but not same and not in previous cases: error
		else:
			return None
	return new_s



## merge date formats
def merge_date_formats(d1, d2):
	new_date = ""
	##             or   and  interval
	split_chars = ["|", "#", "/"]
	done = 0
	## split first date, try every char
	for split_char in split_chars:
		l1 = d1.split(split_char)
		## if spliting worked
		if len(l1) > 1:
			## launch recursively with all parts and d2
			for d in l1:
				aux_date = merge_date_formats(d, d2)
				if aux_date == None:
					return None
				## concat
				new_date = new_date + aux_date + split_char
			new_date = new_date[:-1]
			done = 1
			break
	## if no spliting has been done with the first date, try the second one
	if done == 0:
	        ## split second date, try every char
		for split_char in split_chars:
			l2 = d2.split(split_char)
		        ## if spliting worked
			if len(l2) > 1:
				## launch recursively with all parts and d1
				for d in l2:
					aux_date = merge_date_formats(d1, d)
					if aux_date == None:
						return None
					## concat
					new_date = new_date + aux_date + split_char
				new_date = new_date[:-1]
				done = 1
				break
	## if no spliting has been done at all, end of recursion
	if done == 0:
		new_date = merge_strings(d1, d2)
	if new_date == "":
		return None
	else:
		return new_date

				
			

## error message
def non_consistent_error_message(date1, date2):
	common_functions.err_msg("Error when merging", date1, "and", date2, "!!!")
	common_functions.err_msg("   Dates not consistent.")


## concat 2 durations
def concat_durations(i1, i2, i3, i0):
	coord = get_feature(i3, "FORM")
	if coord == None:
		common_functions.err_msg("Error with coordination in concat_2_dates")
		return 0
	## get duration to merge from node numbers
	d1 = get_value(str(i1), "DURATION")
	d2 = get_value(str(i2), "DURATION")
	## delete old entries
	#del_entry(str(i1), "DURATION")
	#del_entry(str(i2), "DURATION")
	## concat intervals
	if coord == "FAND":
		splitchar = "#"
	elif coord == "FOR":
		splitchar = "|"
	elif coord == "FTO" or coord == "FINFTO" or coord == "FBETWEEN":
		splitchar = "/"
	else:
		splitchar = "#"
	d = d1 + splitchar + d2
	set_entry(str(i0), "DURATION", d)
	## sometimes the duration can also be a set (3 nights)
	if is_set(i1) and is_set(i2):
		f1 = get_attribute(str(i1), "SET", "freq")
		f2 = get_attribute(str(i2), "SET", "freq")
		f = f1 + splitchar + f2
		update_entry(str(i2), "SET", "freq", f)
		copy(i2, i0, "SET")
	return 1

## concat 2 dates
def concat_dates(i1, i2, i3, i0, type1, type2):
	return concat_2_dates(i1, i2, i3, i0, type1, type2)

## concat 2 dates
def concat_2_dates(i1, i2, i3, i0, type1, type2):
	coord = get_feature(i3, "FORM")
	if coord == None:
		common_functions.err_msg("Error with coordination in concat_2_dates")
		return 0
	if coord == "FAND":
		return concat_date_formats(i1, i2, i0, type1, type2, "#")
	elif coord == "FOR":
		return concat_date_formats(i1, i2, i0, type1, type2, "|")
	elif coord == "FINFTO" or coord == "FBETWEEN":
		return concat_date_formats(i1, i2, i0, type1, type2, "/")
	else:
		return concat_date_formats(i1, i2, i0, type1, type2, "#")
	
## concat 3 dates
def concat_3_dates(i1, i2, i3, i4, i0, type1, type2, type3):
	coord = get_feature(i4, "FORM")
	if coord == None:
		common_functions.err_msg("Error with coordination in concat_3_dates")
		return 0
	if coord == "FAND":
		coord = "#"
	elif coord == "FOR":
		coord = "|"
	else:
		coord = "#"
	return concat_date_formats(i1, i2, i0, type1, type2, coord) & \
	       concat_date_formats(i0, i3, i0, type2, type3, coord)
	
## concat 4 dates
def concat_4_dates(i1, i2, i3, i4, i5, i0, type1, type2, type3, type4):
	coord = get_feature(i5, "FORM")
	if coord == None:
		common_functions.err_msg("Error with coordination in concat_4_dates")
		return 0
	if coord == "FAND":
		coord = "#"
	elif coord == "FOR":
		coord = "|"
	else:
		coord = "#"
	return concat_date_formats(i1, i2, i0, type1, type2, coord) & \
	       concat_date_formats(i0, i3, i0, type2, type3, coord) & \
	       concat_date_formats(i0, i4, i0, type3, type4, coord)



## merge_dates = merge two dates
def merge_dates(i1, i2, i0, type1, type2):
	return merge_2_dates(i1, i2, i0, type1, type2)


## merge two dates
def merge_2_dates(i1, i2, i0, type1, type2):
	## granularity levels
	if type1 == "":
		type1 = get_feature(i1, "TEMP_LEVEL").lower()
	if type2 == "":
		type2 = get_feature(i2, "TEMP_LEVEL").lower()
	type0 = get_feature(i0, "TEMP_LEVEL")
	if type0 == None:
		common_functions.err_msg(" Error")
		common_functions.err_msg("No temp_level feature defined for new date")
		return 0
	else:
		type0 = type0.lower()
	## get dates to merge from node numbers
	date1 = get_value(str(i1) + type1, "DATE")
	date2 = get_value(str(i2) + type2, "DATE")
	## merge
	new_date = merge_date_formats(date1, date2)
	if new_date == None:
		non_consistent_error_message(date1, date2)
		return None
	## delete old nodes
	del_entry(str(i1) + type1, "DATE")
	del_entry(str(i2) + type2, "DATE")
	## update dictionary
	set_entry(str(i0) + type0, "DATE", new_date)
	return 1


## merge three dates
def merge_3_dates(i1, i2, i3, i0, type1, type2, type3):
	## granularity levels
	if type1 == "":
		type1 = get_feature(i1, "TEMP_LEVEL").lower()
	if type2 == "":
		type2 = get_feature(i2, "TEMP_LEVEL").lower()
	if type3 == "":
		type3 = get_feature(i3, "TEMP_LEVEL").lower()
	type0 = get_feature(i0, "TEMP_LEVEL")
	if type0 == None:
		common_functions.err_msg(" Error")
		common_functions.err_msg("No temp_level feature defined for new date")
		return 0
	else:
		type0 = type0.lower()
	## get dates to merge from node numbers
	date1 = get_value(str(i1) + type1, "DATE")
	date2 = get_value(str(i2) + type2, "DATE")
	date3 = get_value(str(i3) + type3, "DATE")
	## merge
	aux_date = merge_date_formats(date1, date2)
	if aux_date == None:
		non_consistent_error_message(date1, date2)
		return None
	new_date = merge_date_formats(aux_date, date3)
	if new_date == None:
		non_consistent_error_message(aux_date, date3)
		return None
	## delete old nodes
	del_entry(str(i1) + type1, "DATE")
	del_entry(str(i2) + type2, "DATE")
	del_entry(str(i3) + type3, "DATE")
	## update dictionary
	set_entry(str(i0) + type0, "DATE", new_date)
	return 1


## merge four dates
def merge_4_dates(i1, i2, i3, i4, i0, type1, type2, type3, type4):
	## granularity levels
	if type1 == "":
		type1 = get_feature(i1, "TEMP_LEVEL").lower()
	if type2 == "":
		type2 = get_feature(i2, "TEMP_LEVEL").lower()
	if type3 == "":
		type3 = get_feature(i3, "TEMP_LEVEL").lower()
	if type4 == "":
		type4 = get_feature(i4, "TEMP_LEVEL").lower()
	type0 = get_feature(i0, "TEMP_LEVEL")
	if type0 == None:
		common_functions.err_msg(" Error")
		common_functions.err_msg("No temp_level feature defined for new date")
		return 0
	else:
		type0 = type0.lower()
	## get dates to merge from node numbers
	date1 = get_value(str(i1) + type1, "DATE")
	date2 = get_value(str(i2) + type2, "DATE")
	date3 = get_value(str(i3) + type3, "DATE")
	date4 = get_value(str(i4) + type4, "DATE")
	## merge
	aux_date1 = merge_date_formats(date1, date2)
	if aux_date1 == None:
		non_consistent_error_message(date1, date2)
		return None
	aux_date2 = merge_date_formats(aux_date1, date3)
	if aux_date2 == None:
		non_consistent_error_message(aux_date1, date3)
		return None
	new_date = merge_date_formats(aux_date2, date4)
	if new_date == None:
		non_consistent_error_message(aux_date2, date4)
		return None
	## delete old nodes
	del_entry(str(i1) + type1, "DATE")
	del_entry(str(i2) + type2, "DATE")
	del_entry(str(i3) + type3, "DATE")
	del_entry(str(i4) + type4, "DATE")
	## update dictionary
	set_entry(str(i0) + type0, "DATE", new_date)
	return 1



## merge numbers for durations
def merge_numbers(n1, n2):
	if n1 == "0":
		return n2
	elif n2 == "0":
		return n1
	elif n1 == "X" :
		return n2
	elif n2 == "X":
		return n1
	elif common_functions.is_integer(n1) and common_functions.is_integer(n2):
		return str(int(n1) + int(n2))
	else:
		common_functions.err_msg("Problem when merging durations !")
		return None
	


## merge attributes for durations and sets
def merge_attributes(a1, a2):
	a = {}
	k1 = a1.keys()
	for k in k1:
		if a2.has_key(k):
			if a1[k] != a2[k]:
				common_functions.err_msg("Error in merge_attributes, att", k, "are not consistent.")
				return None
			else:
				del a2[k]
		a[k] = a1[k]
	k2 = a2.keys()
	for k in k2:
		a[k] = a2[k]
	return a

## merge_durations = merge two durations
def merge_durations(i1, i2, i0):
	return merge_2_durations(i1, i2, i0)


## merge two durations
def merge_2_durations(i1, i2, i0):
	## MERGE VALUE
	## get durations to merge from node numbers
	d1 = get_value(str(i1), "DURATION")
	d2 = get_value(str(i2), "DURATION")
	## merge years
	y1 = get_duration_years(d1)
	y2 = get_duration_years(d2)
	y = merge_numbers(y1, y2)
	## merge months
	mo1 = get_duration_months(d1)
	mo2 = get_duration_months(d2)
	mo = merge_numbers(mo1, mo2)
	## merge weeks
	w1 = get_duration_weeks(d1)
	w2 = get_duration_weeks(d2)
	w = merge_numbers(w1, w2)
	## merge days
	da1 = get_duration_days(d1)
	da2 = get_duration_days(d2)
	da = merge_numbers(da1, da2)
	## merge hours
	h1 = get_duration_hours(d1)
	h2 = get_duration_hours(d2)
	h = merge_numbers(h1, h2)
	## merge minutes
	mi1 = get_duration_minutes(d1)
	mi2 = get_duration_minutes(d2)
	mi = merge_numbers(mi1, mi2)
	## merge seconds
	s1 = get_duration_seconds(d1)
	s2 = get_duration_seconds(d2)
	s = merge_numbers(s1, s2)
	##
	new_duration = "P" + y + "Y" + mo + "M" + w + "W" + da + "DT" + h + "H" + mi + "M" + s + "S"
	## MERGE OTHER ATTRIBUTES
	att1 = get_attributes(str(i1), "DURATION")
	att2 = get_attributes(str(i2), "DURATION")
	att = merge_attributes(att1, att2)
	## delete old nodes
	del_entry(str(i1), "DURATION")
	del_entry(str(i2), "DURATION")
	## update dictionary
	set_entry(str(i0), "DURATION", new_duration)
	set_attributes(str(i0), "DURATION", att)
	return 1


## merge three durations
def merge_3_durations(i1, i2, i3, i0):
	return merge_2_durations(i1, i2, i0) & merge_2_durations(i0, i3, i0)


## merge four durations
def merge_4_durations(i1, i2, i3, i4, i0):
	return merge_2_durations(i1, i2, i1) & merge_2_durations(i3, i4, i3) & merge_2_durations(i1, i3, i0)


## merge two sets
def merge_sets(i1, i2, i0):
	v2 = get_value(str(i2), "SET")
	f1 = get_attribute(str(i1), "SET", "freq")
	q1 = get_attribute(str(i1), "SET", "quant")
	a1 = get_attribute(str(i1), "SET", "anchored_to")
	f2 = get_attribute(str(i2), "SET", "freq")
	q2 = get_attribute(str(i2), "SET", "quant")
	a2 = get_attribute(str(i2), "SET", "anchored_to")
	## freq
	if f1 != None and f2 != None:
		common_functions.err_msg("Problem when merging freq attributes in merge_sets")
		return 0
##	elif f1 != None:
##		update_entry(str(i1), "SET", "freq", f1)
	elif f2 != None:
		update_entry(str(i1), "SET", "freq", f2)
	## quant
	if q1 != None and q2 != None:
		common_functions.err_msg("Problem when merging quant attributes in merge_sets")
		return 0
##	elif q1 != None:
##		update_entry(str(i1), "SET", "quant", q1)
	elif q2 != None:
		update_entry(str(i1), "SET", "quant", q2)
	## anchor
	if a1 != None and a2 != None:
		common_functions.err_msg("Problem when merging anchored_to attributes in merge_sets")
		return 0
##	elif a1 != None:
##		update_entry(str(i1), "SET", "anchored_to", a1)
	elif a2 != None:
		update_entry(str(i1), "SET", "anchored_to", a2)
	## value
	update_entry(str(i1), "SET", "value", v2)
	copy(i1, i0)
	del_entry(str(i2), "SET")
	return 1



## merge a set and a date
def merge_set_and_date(i1, i2, i0):
	level = get_feature(i2, "TEMP_LEVEL")
	if level == None:
		common_functions.err_msg("Error in merge_set_and_date, temp_level unknown.")
	v1 = get_value(str(i1), "SET")
	v2 = get_value(str(i2) + level.lower(), "DATE")
	anchored_to = get_attribute(str(i1), "SET", "anchored_to")
	
	## the date must be anchored to the set
        ## two dates: problem (merge ?)
	if anchored_to != None:
		common_functions.err_msg("Error merge_set_and_date, two dates are conflicting")
	else:
		## set has a duration, so value can't be the date
		## create an anchored date
		update_entry(str(i1), "SET", "anchored_to", "DATE" + str(i2) + level.lower())
		update_entry(str(i1), "SET", "temp_rel", "DURING")
		update_entry(str(i1), "SET", "temp_ref", "+")		
		copy(i1, i0)
		return 1
#	att = get_attributes(str(i1), "SET")
        ## delete old nodes
	#del_entry(str(i2) + level.lower(), "DATE")
	#del_entry(str(i1), "SET")
        ## update dictionary
#	set_entry(str(i0), "SET", v)
#	set_attributes(str(i0), "SET", att)
#	return 1
			


## merge a set and an anchor_date
def merge_set_and_anchor_date(i1, i2, i0):
	update_entry(str(i1), "SET", "anchored_to", "ANCHOR" + str(i2))
	update_entry(str(i1), "SET", "temp_rel", "DURING")
	update_entry(str(i1), "SET", "temp_ref", "+")
	copy(i1, i0)
	return 1


## merge a set and a duration
def merge_set_and_duration(i1, i2, i0):
	v1 = get_value(str(i2), "DURATION")
	att = get_attributes(str(i1), "SET")
	## delete old nodes
	#del_entry(str(i1), "SET")
	#del_entry(str(i2), "DURATION")
	## update dictionary
	set_entry(str(i0), "SET", v1)
	set_attributes(str(i0), "SET", att)
	return 1

## anchor a duration to a set
def anchor_duration_to_set(i1, i2, i0):
	## update dictionary
	update_entry(str(i1), "SET", "anchored_to", "DURATION" + str(i2))
	copy(i1, i0)
	return 1
	
## merge a duration and a set
def merge_duration_and_set(i1, i2, i0):
	copy(i2, i0)
	v1 = get_value(str(i1), "DURATION")
	## add freq and freq_unit attributes
	update_entry(str(i0), "SET", "freq", v1)
	## del old duration
	#del_entry(str(i1), "DURATION")
	return 1




## merge an anchor_date and a duration
def merge_anchor_date_and_duration(i1, i2, i0):
	## See if i2 contain a date with the duration
	##    (see add_duration, with "2 nights" for example)
	date_list = get_entries(str(i2), "DATE")
	set_list = get_entries(str(i2), "SET")
	if len(date_list) > 1:
		common_functions.err_msg("Problem in merge_anchor_date_and_duration, list of dates should not contain more than one date.")
		return 0
	## one or no date => builds the anchor_date normally
	else:
		v1 = get_value(str(i2), "DURATION")
		att = get_attributes(str(i1), "ANCHOR")
	        ## delete old nodes
		#del_entry(str(i1), "ANCHOR")
		#del_entry(str(i2), "DURATION")
	        ## update dictionary
		set_entry(str(i0), "ANCHOR", v1)
		set_attributes(str(i0), "ANCHOR", att)
	        ## one date => attach the anchor date to the date
		if len(date_list) == 1:
			pass
#			add_entry("DATE" + str(i0), date_list.items()[0][1])
#			ad1 = get_attribute(str(i1), "ANCHOR", "anchored_to")
#			if ad1 != None:
#				new_date = merge_date_formats(get_value_by_entire_key(ad1), get_value_by_entire_key("DATE" + str(i0)))
#				update_entry(str(i0), "DATE", "value", new_date)
#			update_entry(str(i0), "ANCHOR", "anchored_to", "DATE" + str(i0))
		elif len(set_list) == 1:
			copy(i2, i0, "SET")
##			set_key_list = set_list.keys()
##			## loop but only one in it
##			for set_key in set_key_list:
##				set_anchored_to = set_list[set_key]["anchored_to"]
##			if set_anchored_to != None and att["anchored_to"] != None:
##				date1 = get_value_by_entire_key(set_anchored_to)
##				date2 = get_value_by_entire_key(att["anchored_to"])
##                                ## merge
##				new_date = merge_date_formats(date1, date2)
##				if new_date == None:
##					non_consistent_error_message(date1, date2)
##					return None
##				set_entry(str(i0), "DATE", new_date)
##			new_att = get_attributes(str(i0), "ANCHOR")
##				update_entry(str(i0), "ANCHOR", "anchored_to", "DATE" + str(i0))
		if get_attribute(str(i2), "DURATION", "mod"):
			update_entry(str(i0), "ANCHOR", "mod", get_attribute(str(i2), "DURATION", "mod"))
		return 1



## merge an anchor date and a date
def merge_anchor_date_and_date(i1, i2, i0):
	date_list = get_entries(str(i2), "DATE")
	## if not only one date, problem
	if len(date_list) > 1:
		common_functions.err_msg("Problem in merge_anchor_date_and_date with the list of dates.")
		return 0
	if not update_entry(str(i1), "ANCHOR", "anchored_to", date_list.items()[0][0]):
		common_functions.err_msg("Error merge_anchor_date_and_date with attribute anchored_to.")
		return 0
	if not copy(i1, i0, "ANCHOR"):
		common_functions.err_msg("Error merge_anchor_date_and_date when copying.")
		return 0
	return 1


## merge two anchor dates
def merge_anchor_dates(i_main, i_aux, i0):
	if not get_value(str(i_aux), "ANCHOR"):
		common_functions.err_msg("Error merge_anchor_dates, key ANCHOR", str(i_aux), "not found")
		return 0		
	if not update_entry(str(i_main), "ANCHOR", "anchored_to", "ANCHOR" + str(i_aux)):
		common_functions.err_msg("Error merge_anchor_dates with attribute anchored_to.")
		return 0
	if not copy(i_main, i0, "ANCHOR"):
		common_functions.err_msg("Error merge_anchor_dates when copying.")
		return 0
	return 1



## merge an anchor date to a non temporal node
def merge_anchor_date_to_node(i1, i2, i0):
	if not get_value(str(i1), "ANCHOR"):
		common_functions.err_msg("Error merge_anchor_date_to_node, key ANCHOR", str(i1), "not found")
		return 0		
	if not update_entry(str(i1), "ANCHOR", "anchored_to", "NODE" + str(i2) + "-" + str(XipNode(i2).leftoffset) + "-" + str(XipNode(i2).rightoffset)):
		common_functions.err_msg("Error merge_anchor_date_to_node with attribute anchored_to.")
		return 0
	if not copy(i1, i0, "ANCHOR"):
		common_functions.err_msg("Error merge_anchor_date_to_node when copying.")
		return 0
	return 1



## add a MOD attribute to node 1 according to t_mod feature of node 2
def add_mod(i1, i2, i0, str_type):
	## get t_mod feature
	str_mod = get_feature(i2, "T_MOD")
	if str_mod == None:
		common_functions.err_msg("Error in add_mod, T_MOD feature not found")
		return 0
	if str_type.upper() == "DATE":
		type1 = get_feature(i1, "TEMP_LEVEL")
		if type1 == None:
			type1 = "";
	        ## add the t_mod value as MOD attribute of the node 1.
		return update_entry(str(i1) + type1.lower(), str_type, "mod", str_mod) and \
		       copy(i1, i0, str_type)
	else:
	        ## add the t_mod value as MOD attribute of the node 1.
		return update_entry(str(i1), str_type, "mod", str_mod) and \
		       copy(i1, i0, str_type)

## add a MOD attribute to node 1 according to str_mod
def add_mod_explicit(i1, i0, str_mod, str_type):
	## add the str_mod value as MOD attribute of the node 1.
	return update_entry(str(i1), str_type, "mod", str_mod) and \
	       copy(i1, i0, str_type)

## add a MOD attribute to node 1 according to its own t_mod feature
def add_mod_alone(i):
	## get t_mod feature
	str_mod = get_feature(i, "T_MOD")
	if str_mod == None:
		common_functions.err_msg("Error in add_mod_alone, T_MOD feature not found")
		return 0
	set_entry(str(i), "DATE", create_empty_date())
	return update_entry(str(i), "DATE", "mod", str_mod)



#####
## Adds a "coref" attribute to the node
##    same procedure as for add_mod
#####
def add_coref(i1, str_type):
	if str_type.upper() == "DATE":
		type1 = get_feature(i1, "TEMP_LEVEL")
		if type1 == None:
			type1 = "";
		return update_entry(str(i1) + type1.lower(), str_type, "coref", "TRUE")
	else:
		return update_entry(str(i1), str_type, "coref", "TRUE")




## returns 1 if duration 1 has a smaller unit than duration 2
def duration_lt(i1, i2):
	## MERGE VALUE
	## get durations to merge from node numbers
	d1 = get_value(str(i1), "DURATION")
	d2 = get_value(str(i2), "DURATION")
	s1 = re.search("[1-9]", d1)
	s2 = re.search("[1-9]", d2)
	if s1.start() > s2.start():
		return 1
	elif s1.start() == s2.start():
		ss1 = re.search("^[0-9]+", d1[s1.start()+1:])
		ss2 = re.search("^[0-9]+", d2[s2.start()+1:])
		if ss2 == None:
			return 0
		elif ss1 == None:
			return 1
		elif ss2.end() > ss1.end():
			return 1
		else:
			return 0
	else:
		return 0


#####
## Returns 1 if the temp_level granularity of the first node
##   is strictly lower than the granularity of the second node
#####
def granularity_lt(i1, i2):
	level1 = get_feature(i1, "TEMP_LEVEL")
	level2 = get_feature(i2, "TEMP_LEVEL")
	if level1 == None or level2 == None:
		common_functions.err_msg("Error in granularity_lt, TEMP_LEVEL feature not found")
	else:
		level1 = level1.lower()
		level2 = level2.lower()
		if level1 == "t" and level2 != "t":
			return 1
		elif level2 == "t":
			return 0
		elif (level1 == "d" or level1 == "o") and not (level2 == "d" or level2 == "o"):
			return 1
		elif (level2 == "d" or level2 == "o"):
			return 0
		elif level1 == "w" and not level2 == "w":
			return 1
		elif level2 == "w":
			return 0
		elif level1 == "m" and not level2 == "m":
			return 1
		elif level2 == "m":
			return 0
		elif level1 == "y" or level1 == "+" or level1 == "p":
			return 0
		else:
			return 0


	
	



	

## get_feature
##   gets the value of feature named 'feature_name'
##     in node i1
def get_feature(i1, feature_name):
	n=XipNode(i1)
	## n.features returns a string containing all features
	features=n.features
	## get the appropriate feature
	f=re.search("[,! \[]" + feature_name + ":[^, \]]+", features)
	if f != None:
		v = features[f.start()+1+len(feature_name)+1:f.end()]
	else:
		v = None
	return v


## has_feature
##   returns 1 if the node i1 has the feature 'feature_name'
##           0 otherwise
def has_feature(i1, feature_name):
	n=XipNode(i1)
	## n.features returns a string containing all features
	features=n.features
	## get the appropriate feature
	f=re.search("[, \[]" + feature_name + ":[^, \]]", features)
	if f != None:
		return 1
	else:
		return 0
	
		


## return the number (int) represented by the node
##   if the node is NUM or DIG
def get_number_from_node(i, other_node=None):
	n=XipNode(i)
	## the number is given by the lemma of the node
	str_number=n.lemma
	## remove "," (in 2,000 for example)
	str_number = str_number.replace(",", "")
	## if 'num' (number written in full letters) or "noun" (eg the sixties)
	##   get the int number
	if n.pos == "NUM" or n.pos == "NOUN":
		number = numbers_functions.words_to_number(str_number)
		if number == None:
			return 0
		else:
			str_number = str(number)
	elif n.pos == "DET":
		if get_feature(i, "SG") != None:
			str_number = "1"
		else:
			str_number = "X"
	elif n.pos == "QUANT" or (n.pos == "PREP" and has_feature(i, "C_SET")):
		## half or quarter
		if str_number == "half" or str_number == "quarter":
			return None
		if other_node == None:
			if get_feature(i, "FORM") == "FEACH":
				str_number = "1"
			else:
				str_number = "X"
		else:
			if get_feature(other_node, "SG") != None:
				str_number = "1"
			else:
				str_number = "X"
	elif n.pos != "DIG":
		return None
	if str_number != "X" and not common_functions.is_integer(str_number):
		if (common_functions.is_float(str_number)):
			dotpos = str_number.index(".")
			str_number = str_number[:dotpos]
		else:
			return None
	return str_number





## special multiplication
def mult(s1, s2):
	if common_functions.is_integer(s1) and common_functions.is_integer(s2):
		return str(int(s1) * int(s2))
	elif s1.upper() == "X" and common_functions.is_integer(s2):
		r = "" 
		for i in range(len(s2) - 1):
			r = r + "0"
		r = "X" + r
		return r
	elif s2.upper() == "X" and common_functions.is_integer(s1):
		r = "" 
		for i in range(len(s1) - 1):
			r = r + "0"
		r = "X" + r
		return r
	elif s1.upper() == "X" and s2.upper() == "X":
		return "X"
	else:
		return None



## get the lower unit in temporal granularity scale
def get_lower_unit(str_unit):
	if str_unit.lower() == "y":
		return "mo"
	elif str_unit.lower() == "mo":
		return "w"
	elif str_unit.lower() == "w":
		return "d"
	elif str_unit.lower() == "d":
		return "h"
	elif str_unit.lower() == "h":
		return "mi"
	elif str_unit.lower() == "mi":
		return "s"
	elif str_unit.lower() == "s":
		return "mi"
	else:
		return None
	

## return inverse temporal relation
def inverse_relation(temp_rel):
	if temp_rel.lower() == "before":
		return "after"
	elif temp_rel.lower() == "just_before":
		return "just_after"
	elif temp_rel.lower() == "after":
		return "before"
	elif temp_rel.lower() == "just_after":
		return "just_before"
	elif temp_rel.lower() == "during":
		return "during"
	elif temp_rel.lower() == "starts_with":
		return "after"
	elif temp_rel.lower() == "ends_with":
		return "before"
	elif temp_rel.lower() == "present":
		return "present"
	else:
		common_functions.err_msg("Temporal Relation not implemented yet in inverse_relation")
		return None

## for debuging
def fail():
	return 0


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
