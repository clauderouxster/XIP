"""
Python:

add_temporal_expression(#1, #2, string filename, int sentencenumber, string dct),
fill_temporal_database(string filename, int sentencenumber),
fill_xml_output(string filename, int sentencenumber).
//close_xml_output().

"""

import re
import common_functions
import temp_functions
import calendar_functions
#import pyerr

#import exceptions
#import MySQLdb
                    


## i1 : event, i2 : date
def add_temporal_expression(i1, i2, pathname, sentencenumber, dct="FALSE"):
    sentencenumber = sentencenumber + 1
    if dct == "":
        dct = "FALSE"
    n1 = XipNode(i1)
    oleft = n1.leftoffset
    oright = n1.rightoffset

    dates = temp_functions.get_entries(str(i2), "DATE")
    anchor_dates = temp_functions.get_entries(str(i2), "ANCHOR")

    if len(dates) > 0:
        key_list = dates.keys()
	for key in key_list:
            date = dates[key]["value"]
            if calendar_functions.is_absolute_date(date):
                connection_object = MySQLdb.connect(host='etretat', user='root', passwd='root', db='temp_en')
                cursor = connection_object.cursor()
                commande = "insert into tempexpr values(0, '" + date + "', '', '" + pathname + "', " + str(sentencenumber) + ", " + str(oleft) + ", " + str(oright) + ", " + str(i1) + ", ''," + dct + ")"
                cursor.execute(commande)
        return 1
    elif len(anchor_dates) > 0:
        dates = anchor_dates
        key_list = dates.keys()
        for key in key_list:
            anchor_date = dates[key]["anchored_to"]
            if not anchor_date == None:
                date = temp_functions.get_value(anchor_date.replace("DATE", ""), "DATE")
                if calendar_functions.is_absolute_date(date):
                    connection_object = MySQLdb.connect(host='etretat', user='root', passwd='root', db='temp_en')
                    cursor = connection_object.cursor()
                    commande = "insert into tempexpr values(0, '" + date + "', '', '" + pathname + "', " + str(sentencenumber) + ", " + str(oleft) + ", " + str(oright) + ", " + str(i1) + ", ''," + dct + ")"
                    cursor.execute(commande)
            return 1
            
    else:
        print "Date not found"
        return 0


def fill_temporal_database(pathname, sentencenumber):
    sentencenumber = sentencenumber + 1
    temp_dict = temp_functions.get_dict()
    key_list = temp_dict.keys()
    connection_object = MySQLdb.connect(host='etretat', user='root', passwd='root', db='temporal_nodes')
    cursor = connection_object.cursor()
#    commande = "TRUNCATE TABLE temp_nodes"
#    cursor.execute(commande)


    for key in key_list:
        print "key: " + key
        node = temp_dict[key]
        match = re.search("[0-9]+", key)
        node_number = key[match.start():match.end()]
        commande = "insert into temp_nodes values(0, '" + pathname + "', " + str(sentencenumber) + ", " + node_number + ", '" + key + "', '" + node["type"] + "', "
        if node.has_key("value"):
            commande = commande + "'" + node["value"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("quant"):
            commande = commande + "'" + node["quant"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("freq"):
            commande = commande + "'" + node["freq"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("mod"):
            commande = commande + "'" + node["mod"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("coref"):
            commande = commande + "'" + node["coref"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("temp_rel"):
            commande = commande + "'" + node["temp_rel"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("temp_ref"):
            commande = commande + "'" + node["temp_ref"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ", "
        if node.has_key("anchored_to"):
            commande = commande + "'" + node["anchored_to"] + "'"
        else:
            commande = commande + "''"
        commande = commande + ")"

        print commande
        
        cursor.execute(commande)
    return 1







def fill_xml_output(pathname, sentencenumber):
    sentencenumber = sentencenumber + 1
    filename='/home/xtannier/tmp/temporal_dict.txt'
    if (sentencenumber == 1):
        f=open(filename, 'w')
##        f.write('<?xml version="1.0" encoding="iso-8859-1"?>\n')
##        f.write('\n')
##        f.write('<TEMPORAL_DICTIONARY>\n')
    else:
        f=open(filename, 'a')

    str_text = temp_functions.dict_to_string(pathname, sentencenumber)

    f.write(str_text)

    f.close()
    
    return 1



##def close_xml_output():
##    filename='/home/xtannier/tmp/temporal_dict.xml'
##    f=open(filename, 'a')
##    f.write('</TEMPORAL_DICTIONARY>\n')
##    f.close()
##    print filename, "written"


