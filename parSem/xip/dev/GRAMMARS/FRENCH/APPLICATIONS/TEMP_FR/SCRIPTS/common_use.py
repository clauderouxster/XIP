#!/usr/local/bin/python
# coding: latin-1

#############################################################################
#
# Common use functions for accessing linguistic information
#
#
#############################################################################

import re
import string

import xips



## get_feature
##   gets the value of feature named 'feature_name'
##     in node i1
def get_feature(i1, feature_name):


    nperiod = xips.XipNode(i1)
## feature_list is a string containing all features
    feature_list = nperiod.features

# TRACE
#    print feature_list
#

## get the appropriate feature
    v = get_feat_from_string(feature_name,feature_list)

    return v


def get_feat_from_string(feature_name, string_feat):
    
    f=re.search("[,! \[]" + feature_name + ":[^, \]]+", string_feat)
    if f != None:
        v = string_feat[f.start()+1+len(feature_name)+1:f.end()]
    else:
        v = None
    return v
    




def print_node(i):
    n = xips.XipNode(i)
    print "Node", str(i), ":", n.lemma
    return 1

