#! /usr/bin/env python
from random import randrange
#from os import path
import re
import os 
import os.path
import string
import sys
import random
import math 

def extractfrombalise(txt,balise):
    if  txt.find(balise) != -1:
        balisefin=balise.replace("<","</")
        i1=txt.index(balise)
        i2=txt.index(balisefin)
        chaine=txt[i1+len(balise):i2]
        return chaine
    else:
        return "NULL-EMPTY"


ref = open(sys.argv[1],'r')
test = open(sys.argv[2],'r')

f1=ref.read()
f2=test.read()
f1.replace("</sentences>","")
f2.replace("<sentences>","")
f2.replace("</sentences>","")
f1.replace("<sentences>","")

t1=extractfrombalise(f1,"<sentences>")
t2=extractfrombalise(f2,"<sentences>")

allrefs=t1.split("</sentence>")
alltests=t2.split("</sentence>")

terms_ref={}
categ_ref={}
terms_test={}
categ_test={}
text={}
for e in allrefs:
   # print e
    chaqueligne=e.split(">")
    id_sent=chaqueligne[0].replace("<sentence id=","")
   # print "ID:" + id_sent
    sentence=extractfrombalise(e,"<text>")
  #  print sentence
    if sentence !=  "NULL-EMPTY" : 
        clefphrase=  id_sent + "@@@" + sentence
     #   print  clefphrase
        list_term=[]
        list_cats=[]
        terms=extractfrombalise(e,"<aspectTerms>")
        categ=extractfrombalise(e,"<aspectCategories>")
     #   print "TERMS" + terms 
     #   print "CATEG" + categ
        if terms != "NULL-EMPTY" : 
            allterms=terms.split("/>")
            for t in allterms:
                if t.find("\"") != -1 :
                    partterm=t.split("\"")
              #      print partterm[1]
               #     print partterm[3]
                    offset=partterm[5] + "-" + partterm[7]
                    termFinal=(partterm[1], partterm[3], offset) 
                    list_term.append(termFinal)
        if categ != "NULL-EMPTY" :
            allcats=categ.split("/>")
            for c in allcats:
                if c.find("\"") != -1 :
                    partcat=c.split("\"")
            #        print partcat[1]
               #     print partcat[3]
                    catfinal=(partcat[1], partcat[3]) 
                    list_cats.append(catfinal)
        terms_ref[clefphrase]=list_term
        categ_ref[clefphrase]=list_cats


for e in alltests:
   # print e
    chaqueligne=e.split(">")
    id_sent=chaqueligne[0].replace("<sentence id=","")
   # print "ID:" + id_sent
    sentence=extractfrombalise(e,"<text>")
  #  print sentence
    if sentence !=  "NULL-EMPTY" : 
        clefphrase=  id_sent + "@@@" + sentence
      #  print  clefphrase
        list_term=[]
        list_cats=[]
        terms=extractfrombalise(e,"<aspectTerms>")
        categ=extractfrombalise(e,"<aspectCategories>")
      #  print "TERMS" + terms 
       # print "CATEG" + categ
        if terms != "NULL-EMPTY" : 
            allterms=terms.split("/>")
            for t in allterms:
                if t.find("\"") != -1 :
                    partterm=t.split("\"")
                 #   print partterm[1]
                 #   print partterm[3]
                    offset=partterm[5] + "-" + partterm[7]
                    termFinal=(partterm[1], partterm[3], offset) 
                    list_term.append(termFinal)
        if categ != "NULL-EMPTY" :
            allcats=categ.split("/>")
            for c in allcats:
                if c.find("\"") != -1 :
                    partcat=c.split("\"")
               #     print partcat[1]
                #    print partcat[3]
                    catfinal=(partcat[1], partcat[3]) 
                    list_cats.append(catfinal)
        terms_test[clefphrase]=list_term
        categ_test[clefphrase]=list_cats

for c,s in categ_ref.iteritems():
    if categ_test.has_key(c):
        ss=c.split("@@@")
        ph=ss[1]
       # print "@phrase : " + ph
        lref=s
        ltest=categ_test[c]
        toprint=""
        catjustes=1
        for cat in ltest:
            if cat in lref:
                toprint = toprint + "\t @ cat correct:" +cat[0] + "|" +cat[1] + "\n" 
            else:
                toprint = toprint +  "\t @ cat  en trop:" +cat[0] + "|" +cat[1] + "\n"
                catjustes=0
        for cat in lref:
            if cat in ltest:
                done=1
            else:
                catjustes=0
                toprint = toprint + "\t @ cat manquante:" +cat[0] + "|" +cat[1] + "\n"
        if catjustes==1:
            print "@ CAT-JUSTE phrase : " + ph + "\n" + toprint
        else:
             print "@ CAT-FAUX  phrase : " + ph + "\n" + toprint
    else:
        print "PAS VI" + c

for c,s in terms_ref.iteritems():
    if terms_test.has_key(c):
        ss=c.split("@@@")
        ph=ss[1]
        lref=s
        ltest=terms_test[c]
        toprint = ""
        tojuste=1
        for terms in ltest:
            if terms in lref:
                toprint = toprint + "\t @ term correct:" + terms[0] + "|" + terms[1] + "|" + terms[2] +"\n" 
            else:
                toprint = toprint + "\t @ term en trop:" + terms[0] + "|" + terms[1] + "|" + terms[2]  + "\n" 
                tojuste=0
        for terms in lref:
            if terms in ltest:
                done=1
            else:
                toprint = toprint +"\n\t @ term manquant:" + terms[0] + "|" + terms[1] + "|" + terms[2] + "\n" 
                tojuste=0
        if tojuste==1:
            print "@ TOJUSTE phrase : " + ph + "\n" + toprint
        else:
             print "@ FAUX  phrase : " + ph + "\n" + toprint
 
    else:
        print "PAS VI" + c
