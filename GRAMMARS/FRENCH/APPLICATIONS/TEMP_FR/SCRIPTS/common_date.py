#!/usr/local/bin/python
# coding: latin-1

#############################################################################
#
# Common use functions for accessing some specific temporal fields and
# normalize them
#
#
#############################################################################

import re
import string
import sys

import xips




## get value of a feature from a higher level node (absolute date)
##   gets the surface value of lexical unit having  feature named 'feature_name'
##     in node i1 - returns a string
def get_recur_val(n,trait):



    global val_retour
    continue_recursion=1
    traits = n.features
    val_retour = 0

    print "GET_RECUR_VAL ENTREE" + n.surface
    if (has_feature(traits,trait) ):
        continue_recursion=0
        print "AI TROUVE" 
        print n.surface
        val_retour = n.surface
        
        
    # have found a match
    if ( continue_recursion == 0 ):
        print "Pour retourner la valeur " + val_retour
        return val_retour



    if ( has_daughter(n) == -1)  :
        # stop the recursion
        print "DANS condition d'arret" + "+" , val_retour , "+" + str(has_grand_daughter(n))
        return 2
    
    print "TATATATATATATATA"


    # go to the daughter (leftmost) - recursive call on daughters
    new_n = xips.XipNode(n.daughter)
    print "TITITITITIT"
    ret = get_recur_val(new_n,trait)
    if ( type(ret) == type("") ):
        return ret

    # go to the sister - recursive call on sister
    while (new_n.next != -1):
        new_n = xips.XipNode(new_n.next)
        ret = get_recur_val(new_n,trait)
        if ( type(ret) == type("") ):
            return ret

 

    return 3


    
## has_feature
##   returns 1 if the node i1 has the feature 'feature_name'
##           0 otherwise
def has_feature(features, feature_name):

        print "TOTOTOTO HAS_FEATURE"
	## features is a string containing all features

	## get the appropriate feature
	f=re.search("[, \[]" + feature_name + ":[^, \]]", features)
	if f != None:
		return 1
	else:
		return 0
	
		

## test if a node is pre-terminal
def has_grand_daughter(node):
    if (node.daughter == -1):
        return -1
    new = xips.XipNode(node.daughter)
    return (new.daughter)


def has_daughter(node):
    return (node.daughter)


## normalisation of year numbers
def normalise_annee(chaine):

    if ( (len(chaine)) == 1):
        return "200" + chaine[0]
    elif ( (len(chaine) == 2) and (chaine[0] == "0") ):
        return "20" + chaine
    elif ( len(chaine) == 2):
        return "19" + chaine
    else: 
        return chaine



## normalisation of months
def normalise_mois(chaine):


    mois = {
      'janvier': '01',
      'février': '02',
      'mars':    '03',
      'avril':   '04',
      'mai':     '05',
      'juin':    '06',
      'juillet': '07',
      'août':    '08',
      'septembre': '09',
      'octobre':  '10',
      'novembre':  '11',
      'décembre':  '12',
      }

    if ( chaine.lower() in mois):
        return mois[chaine]

    return "--"



## normalisation of days
def normalise_jour(chaine):
    if ( len(chaine) == 1):
        return "0" + chaine

    return chaine


