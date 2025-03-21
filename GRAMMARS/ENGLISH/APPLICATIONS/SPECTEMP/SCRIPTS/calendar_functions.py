"""
Python:

//next(string orig, string wheretogo).

"""

##import datetime
##import calendar
import temp_functions
import common_functions


#def my_date_to_calendar_date(str_date):


##def format(date, granularity = None):
##    str_date = None
##    str_date_y = temp_functions.create_date_year(str(date.year))
##    if granularity == "y":
##        str_date = str_date_y
##    else:
##        str_date_mo = temp_functions.merge_strings(str_date_y, \
##                                                   temp_functions.create_date_month(str(date.month)))
##        if granularity == "mo":
##            str_date = str_date_mo
##        else:
##            str_date_d = temp_functions.merge_strings(str_date_mo, \
##                                                      temp_functions.create_date_day(str(date.day)))
##            if granularity == "d":
##                str_date = str_date_d
##            else:
##                common_functions.err_msg("Error in format, other granularities not implemented.")
##    return str_date
    

##def next(str_date_orig, str_date_togo):
##    granularity = None
##    if not is_complete_date(str_date_orig):
##        common_functions.err_msg("Error in next(), original date not complete")
##        return None
##    date_orig = datetime.date(int(temp_functions.get_date_year(str_date_orig)), \
##                              int(temp_functions.get_date_month(str_date_orig)), \
##                              int(temp_functions.get_date_day(str_date_orig)))
##    date_aux = date_orig
##    ## month
##    togo_month = temp_functions.get_date_month(str_date_togo)
##    togo_day = temp_functions.get_date_day(str_date_togo)
##    if common_functions.is_integer(togo_month):
##        orig_month = date_aux.month
##        if int(togo_month) > orig_month or \
##               (common_functions.is_integer(togo_day) and \
##                int(togo_month) == orig_month and \
##                int(togo_day) > date_aux.day):
##            date_aux = datetime.date(int(temp_functions.get_date_year(str_date_orig)), \
##                                      int(togo_month), \
##                                      1)
##        else:
##            date_aux = datetime.date(int(temp_functions.get_date_year(str_date_orig)) + 1, \
##                                      int(togo_month), \
##                                      1)
##        granularity = "mo"
##    ## day
##    if common_functions.is_integer(togo_day):
##        orig_day = date_aux.day
##        delta = (calendar.monthrange(date_aux.year, date_aux.month)[1] + \
##                 (int(togo_day) - orig_day)) % calendar.monthrange(date_aux.year, date_aux.month)[1]
##        if delta == 0:
##            delta = calendar.monthrange(date_aux.year, date_aux.month)[1]
##        date_aux = date_aux + datetime.timedelta(days=delta)
##        granularity = "d"
##    ## day of the week
##    togo_day_of_the_week = temp_functions.get_date_day_of_the_week(str_date_togo)
##    if common_functions.is_integer(togo_day_of_the_week):
##        orig_day_of_the_week = date_aux.isoweekday()
##        delta = (7 + (int(togo_day_of_the_week) - orig_day_of_the_week)) % 7
##        if delta == 0:
##            delta = 7
##        date_aux = date_aux + datetime.timedelta(days=delta)
##        granularity = "d"
##    ## week
##    ##   not implemented
##    ## ord
##    ##   not implemented
##    return format(date_aux, granularity)


##def last(str_date_orig, str_date_togo):
##    granularity = None
##    if not is_complete_date(str_date_orig):
##        common_functions.err_msg("Error in next(), original date not complete")
##        return None
##    date_orig = datetime.date(int(temp_functions.get_date_year(str_date_orig)), \
##                              int(temp_functions.get_date_month(str_date_orig)), \
##                              int(temp_functions.get_date_day(str_date_orig)))
##    date_aux = date_orig
##    ## month
##    togo_month = temp_functions.get_date_month(str_date_togo)
##    togo_day = temp_functions.get_date_day(str_date_togo)
##    if common_functions.is_integer(togo_month):
##        orig_month = date_aux.month
##        if int(togo_month) < orig_month or \
##               (common_functions.is_integer(togo_day) and \
##                int(togo_month) == orig_month and \
##                int(togo_day) < date_aux.day):
##            date_aux = datetime.date(int(temp_functions.get_date_year(str_date_orig)), \
##                                     int(togo_month), \
##                                     calendar.monthrange(int(temp_functions.get_date_year(str_date_orig)), int(togo_month))[1])
##        else:
##            date_aux = datetime.date(int(temp_functions.get_date_year(str_date_orig)) - 1, \
##                                     int(togo_month), \
##                                     calendar.monthrange(int(temp_functions.get_date_year(str_date_orig)), int(togo_month))[1])
##            granularity = "mo"
##    ## day
##    if common_functions.is_integer(togo_day):
##        orig_day = date_aux.day
##        delta = (orig_day - int(togo_day)) % calendar.monthrange(date_aux.year, date_aux.month -1)[1]
##        if delta == 0:
##            delta = calendar.monthrange(date_aux.year, date_aux.month -1)[1]
##        date_aux = date_aux - datetime.timedelta(days=delta)
##        granularity = "d"
##    ## day of the week
##    togo_day_of_the_week = temp_functions.get_date_day_of_the_week(str_date_togo)
##    if common_functions.is_integer(togo_day_of_the_week):
##        orig_day_of_the_week = date_aux.isoweekday()
##        delta = (orig_day_of_the_week - int(togo_day_of_the_week)) % 7
##        if delta == 0:
##            delta = 7
##        date_aux = date_aux - datetime.timedelta(days=delta)
##        granularity = "d"
##    ## week
##    ##   not implemented
##    ## ord
##    ##   not implemented
##    return format(date_aux, granularity)



def is_absolute_date(date):
    if common_functions.is_integer(temp_functions.get_date_year(date)):
        return 1
    else:
        return 0

def is_complete_date(date):
    if common_functions.is_integer(temp_functions.get_date_year(date)) and \
       common_functions.is_integer(temp_functions.get_date_month(date)) and \
       common_functions.is_integer(temp_functions.get_date_day(date)):
        return 1
    else:
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
	



#print is_complete_date("-20061027T-------O---W---")
#print next("-20061108T-------O---W---", "-------08T-------O---W---")
#print last("-20061108T-------O---W---", "-------08T-------O---W---")

##date = datetime.datetime(2000, 1, 1, 13, 29, 45)
##delta = datetime.timedelta(days=1)
##print date
##print delta
##new_date = date + delta
##print new_date
##print calendar.monthrange(2006, 8)[1]

##date = datetime.date(2006, 11, 8)
##print format(date, "y")
##print format(date, "mo")
##print format(date, "d")
##print format(date, "h")



