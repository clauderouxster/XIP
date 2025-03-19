#! /usr/bin/env python

import re
import os 
import string
import operator
import sys
mots=[]
frequences=[]
coco=[3,4,1,6,1]
dict_of_words={}


def qsort1(list):
    if list == []: 
        return []
    else:
        pivot = list[0]
        lesser = qsort1([x for x in list[1:] if x < pivot])
        greater = qsort1([x for x in list[1:] if x >= pivot])
        return lesser + [pivot] + greater


inputfile = open(sys.argv[1],'r')
lines = inputfile.readlines()
c=0
total=0
for index in range(len(lines)):
    current_line=lines[index]
    if current_line not in dict_of_words:
        dict_of_words[current_line] = 1
        total=total + 1
    else:
        dict_of_words[current_line] = dict_of_words[current_line] +1
        total=total + 1


freq_list= dict_of_words.items()

# sort

sorted_dict = sorted(dict_of_words.iteritems(), key=operator.itemgetter(1), reverse=True)

for word, freq in sorted_dict:
    percentage= freq * 100 / float(total)
    print word[0:len(word)-1] + " - : frequence " + str(freq)
    #+ " percentage: %f" % percentage

#print "TOTAL COUNTED:" + str(total)


#indice=0

#for k, v in sorted_dict.iteritems():
  #  mot=sorted
    #print "mes mots" + k[0:len(k)-1] + "frequence : " + str(v)

#for k, v in dict_of_words.iteritems():
  #  print "mots :" + k[0:len(k)-1]  + "frequence : " + str(v)
    #frequences.insert(indice,v)
 #   mots.insert(indice,k[0:len(k)-1])
 #   indice=indice+1


#sortedcount=qsort1(frequences)

#for index, val in enumerate(sortedcount):
#    print "index:" + str(index)+ " " + mots[index] + " : sorted:" + str(val)
    
#for k, v in dict_of_words.iteritems():
#    toprint=k[0:len(k)-1]
 #   if v >=500 :
  #      print "@@@@ " + toprint + " : nb occurrences : " + str(v)

 #   if (v >=5 and v < 500):
  #      print  toprint + " : nb occurrences : " + str(v)
#
 #   if (v >1 and v < 5):
  #      print  toprint + " : nb occurrences : " + str(v)

