
"""
Python:


init_dict(dictionary tdict),
print_xip_dict(dictionary tdict),
range_date_relat(#1, $1, int sentence_num),
range_date_absolue(#1, int sentence_num),
set_attrib_relative_date(#2,#3,string _res).


"""
#!/usr/bin/python



##############################################################################
#                                                                            #
#   Project    : French Temporal Grammar                                     #
#   filename   : ScriptTemp.py                                               #
#   Date       : June 2009                                                   #
#   Author     : Caroline Hagege	                                     #
#                                                                            #
##############################################################################


import sys
import os
import re
import string

#import xips

from common_use import get_feature
from common_use import get_feat_from_string
from common_date import get_recur_val
from common_date import normalise_annee
from common_date import normalise_mois
from common_date import normalise_jour







##############################################################################
## VARIABLES INITIALISATION
##############################################################################

temp_dict={}

##############################################################################


##############################################################################
## Dictionary functions
##############################################################################

def init_dict(tdict):
	global temp_dict
	temp_dict = tdict
	return 1

## print the dictionary
def print_xip_dict(tdict):
	print "-----------------------DICTIONARY---------------"
	print tdict


## add an entry to the dictionary
def add_entry(str_key, str_type, str_value):
	global temp_dict
	
	if str_value == None:
		return 0
	temp_dict[str_type.upper() + str_key] = {}
	temp_dict[str_type.upper() + str_key]["type"] = str_type
	temp_dict[str_type.upper() + str_key]["value"] = str_value
	print temp_dict

	return 1


##############################################################################
##############################################################################
##############################################################################
#
#                      FONCTIONS D'AJOUTS DE TRAITS 
#
##############################################################################
##############################################################################
##############################################################################

# add features on relative tempexpr when there are in the form
# "next/previous <time period>
# feature is then associated on the lhs node according

# eg la semaine/mois/WE/annee prochain(e)|dernier(e)|passe
# eg la semaine suivante

def set_attrib_relative_date(node2,node3): 

	global _res


	print 'fonction attribution de traits pour dates relatives'


	nouvInfo = ""


	# get the direction of the temporal delta
	direct = get_feature(node3,"DIREC")
	
	#get the unit of temporal delta
	t = get_feature(node2,"VAL_DUREE")

## concatenation of period (W, M) etc
	nouvInfo = direct + "1" + t
	_res = nouvInfo

	return 1


#### rangement des dates - appeles dans les dependances finales

def range_date_absolue(i1,sentence_num):

	n = XipNode(i1)

	num_ph = sentence_num
	num_g = n.leftoffset
	num_d = n.rightoffset

	chaine_temporelle = n.surface

	y = get_recur_val(n,"ANNEE")

	norm_an = normalise_annee(str(y))
	print "NORM_ANNEE" + norm_an

	m = get_recur_val(n,"MOIS")
	print "MOIS" + str(m)
	norm_mois = normalise_mois(str(m))
	
	d = get_recur_val(n,"NUMJOURPOT")
	print "JOUR" + str(d)
	norm_jour = normalise_jour(str(d))

	val_d_norm = norm_an + norm_mois + norm_jour

	ident_chaine = str(num_ph) + "_" + str(num_g) + "_" + str(num_d)

	final1 = ident_chaine + "|" + chaine_temporelle

	final = final1 + "|" + val_d_norm

	add_entry(str(i1) , "DATE_ABSOLUE", final)

	write_file(final)
	return 1


def range_date_relat(i1,refDep,sentence_num):

	n = XipNode(i1)


	xdep = XipDependency(refDep)



	num_ph = sentence_num;
	num_g = n.leftoffset;
	num_d = n.rightoffset;

	chaine_traits = xdep.features

	print "TOTO"
	print chaine_traits

	chaine_cal = ""
	
	ident_chaine = str(num_ph) + "_" + str(num_g) + "_" + str(num_d)
	chaine_temporelle = n.surface

	final1 = ident_chaine + "|" + chaine_temporelle

	v1 = get_feat_from_string("TEMP_REF",chaine_traits)

	v2 = get_feat_from_string("DELTA_T",chaine_traits)

	if (v1 == "ST"):

		if (v2 != None):

			# DEBUG
			print "-------------------" + v2
			chaine_cal = "T0" + "+" + v2

		else:
			chaine_cal = "T0" + "+" + "---"

	else:
		if ( v1 == "TT"):

			if ( v2 != None)  :
				chaine_cal = "TREF" + "+" + v2

			else:
				chaine_cal = "TREF" + "+"  + v2

		else:

			chaine_cal = "--+---"
	

	final = final1 + "|" + chaine_cal

	# information indexed to the nodes added to temp_dict of the form 
	# 'DATE_RELAT14': 
	#     {'type': 'DATE_RELAT', 
        #      'value': '0_12_28|le mois prochain|T0++1M'}

	add_entry(str(i1) , "DATE_RELAT", final)

	# also creates a file with the results 
	write_file(final)
	return 1



#### Fichier resultat

def write_file(valeur):
	file = open('./temps_results.txt', 'a')
	file.write(valeur+"\n")
	file.close()


def clean_temp_file():
	if os.path.exists('./temps_results.txt'):
		    os.remove('./temps_results.txt')
	return 1





###############################################################################
###############################################################################
