#! /usr/bin/env python

import re
import os 
import string
import sys

#input : a file tagged with XIP HMM
#output : (more or less) equivalent file with penn tree bank tags

inputfile = open(sys.argv[1],'r')
doc = inputfile.read()

doc.replace("\n\n</review>\n<review>","")
doc=doc.replace("\n\n","@@@@")
doc=doc.replace("\n","|||")
#doc=doc.replace("@@","\n\n")
#print doc
lines=doc.split("@@@@")
streamout=""
for index in range(len(lines)):
 #   print "????" + lines[index]
    current_line=lines[index]
    normalcase=0
    if current_line.find("|||") != -1: #ambiguity in the HMM output
        current_line=current_line.replace("+countable","")
        current_line=current_line.replace("+c_person","")
        current_line=current_line.replace("+s_sc_pwith","")
        current_line=current_line.replace("+Country","")
        current_line=current_line.replace("+s_p_back","")
        current_line=current_line.replace("+s_p_through","")
        ambiglist=current_line.split("|||")
        if (len(ambiglist)==2 ):
            analyse1 = ambiglist[0].split("\t")
            analyse2 = ambiglist[1].split("\t")
            if analyse1[2] == analyse2[2] : #false ambiguity : both tags are exactly the same
                normalcase=1
                current_line=ambiglist[0]
        if normalcase == 0:
            ambigtag=""
            word=""
            for elem in ambiglist:
                eachword=elem.split("\t")
                word=eachword[0] #always the same anyway
                ambigtag = ambigtag + "|||" + eachword[2]
            treated=0
            if ambigtag.find("+VBPAST") !=  -1:
                if ambigtag.find("=not") !=  -1:
                    init=word.index("n't")
                    print word[0:init] + "/VBD\n" + "n't/RB"
                else:
                     print word + "/VBD"
                treated=1
            if ( ambigtag.find("+MEAS") !=-1 and ambigtag.find("+NOUN") !=-1 ):
                print  word + "/NN"
                treated=1
            if ( ambigtag.find("+Sg+NOUN") !=-1 and ambigtag.find("+For+NOUN") !=-1 and treated==0):
                print  word + "/NN"
                treated=1
            if ( ambigtag.find("+Verb+Trans+PastBoth+123SP+VPAP") !=-1 and ambigtag.find("+Verb+PastBoth+123SP+VPAP") !=-1  and treated==0  ):
                print  word + "/VBN"
                treated=1
            if ( ambigtag.find("+COORD") !=-1 and ambigtag.find("+COTHAN") !=-1  and treated==0):
                print  word + "/CC|IN"
                treated=1  
            if ( ambigtag.find("+Title") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print  word + "/NNP"
                treated=1
            if ( ambigtag.find("+int_verb") !=-1 and ambigtag.find("+VERB") !=-1 and ambigtag.find("+Non3sg") and treated==0 ):
                print  word + "/VBP"
                treated=1
            if ( ambigtag.find("+ADV") !=-1 and ambigtag.find("+COSUB") !=-1 and treated==0 ):
                print  word + "/RB|IN"
                treated=1
            if ( ambigtag.find("+VERB") !=-1 and ambigtag.find("+ADJ") !=-1  and treated==0):
                print  word + "/VB|JJ"
                treated=1
            if ( ambigtag.find("+VPAST") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print  word + "/NN|VBD"
                treated=1
            if ( ambigtag.find("+ADJPAP") !=-1 and ambigtag.find("+VPAP") !=-1  and treated==0):
                print  word + "/VBN|JJ"
                treated=1
            if ( ambigtag.find("+PRON") !=-1 and ambigtag.find("+DET") !=-1 and treated==0 ):
                print  word + "/DT|PRP"
                treated=1
            if ( ambigtag.find("+Prop") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print  word + "/NNP"
                treated=1
            if ( ambigtag.find("+QUANT") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print  word + "/NN|DT"
                treated=1
            if ( ambigtag.find("+COORD") !=-1 and ambigtag.find("+QUANT") !=-1  and treated==0):
                print  word + "/COORD|DT"
                treated=1
            if ( ambigtag.find("+PREP") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print  word + "/NN|IN"
                treated=1
            if  ( ambigtag.find("+PREPADVAS") !=-1  and treated==0):
                print word + "/IN"
                treated=1
            if ( word[0] in "ABCDEFGHIJKLMNOPQRSTUVWXYZ" and ambigtag.find("+Init") !=-1 and treated==0 ):
                print word + "/NNP"
                treated=1
            if  ( ambigtag.find("+WPRON") !=-1 and ambigtag.find("+PRONREL") !=-1  and treated==0):
                print word + "/WDT"
                treated=1
            if  ( ambigtag.find("+ADJ") !=-1 and ambigtag.find("+ADV") !=-1  and treated==0):
                print word + "/JJ|RB"
                treated=1
            if  ( ambigtag.find("+VERB") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print word + "/VB|NN"
                treated=1
            if  ( ambigtag.find("+int_verb") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print word + "/VB|NN"
                treated=1
            if  ( ambigtag.find("+int_noun") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print word + "/NN"
                treated=1
            if  ( ambigtag.find("+int_adj") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print word + "/NN"
                treated=1
                treated=1
            if  ( ambigtag.find("+NOUNING") !=-1 and ambigtag.find("+NOUN") !=-1 and treated==0 ):
                print word + "/NN"
                treated=1
            if  ( ambigtag.find("+HAVE") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print word + "/VBZ|NN"
                treated=1
            if  ( ambigtag.find("+NADJ") !=-1 and ambigtag.find("+VERB") !=-1  and treated==0):
                print  word + "/NN|JJ|VB"
                treated=1
            if  ( ambigtag.find("+NADJ") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print  word + "/NN|JJ"
                treated=1
            if  ( ambigtag.find("+NADJ") !=-1 and ambigtag.find("+ADV") !=-1  and treated==0):
                print  word + "/JJ|RB"
                treated=1
            if  ( ambigtag.find("+NADJ") !=-1 and ambigtag.find("+int_adj") !=-1 and treated==0):
                print  word + "/NN|JJ"
                treated=1
            if  ( ambigtag.find("+Pl+NOUN") !=-1 and ambigtag.find("+Sg+NOUN") !=-1  and treated==0):
                print word + "/NN"
                treated=1
            if  ( ambigtag.find("+SP+NOUN") !=-1 and ambigtag.find("+Pl+NOUN") !=-1  and treated==0):
                print word + "/NN"
                treated=1
            if  ( ambigtag.find("+Sg+NOUN") !=-1 and ambigtag.find("+SP+NOUN") !=-1  and treated==0):
                print word + "/NN"
                treated=1
            if  ( ambigtag.find("+VPAST") !=-1 and ambigtag.find("VPAP") !=-1  and treated==0):
                print word + "/VBD|VBN"
                treated=1
            if  ( ambigtag.find("+prof") !=-1 and ambigtag.find("+Title") !=-1  and treated==0):
                print word + "/NNP"
                treated=1
            if  ( ambigtag.find("+PREP") !=-1 and ambigtag.find("+ADV") !=-1  and treated==0):
                print word + "/IN|RB"
                treated=1
            if  ( ambigtag.find("+PRON") !=-1 and ambigtag.find("+PREDET") !=-1  and treated==0):
                print word + "/PRP|PDT"
                treated=1
            if  ( ambigtag.find("+NOUN") !=-1 and ambigtag.find("+CARD") !=-1  and treated==0):
                print word + "/NN|CD"
                treated=1
            if  ( ambigtag.find("+DET") !=-1 and ambigtag.find("+NOUN") !=-1  and treated==0):
                print word + "/DT"
                treated=1
            if  ( ambigtag.find("+COADV") !=-1 and ambigtag.find("+Coord") !=-1 and ambigtag.find("+Adv") and treated==0):
                print word + "/CC|RB"
                treated=1
            if  ( ambigtag.find("Trans+Prog+VPROG") !=-1 and ambigtag.find("Verb+Prog+VPROG") !=-1  and treated==0):
                print word + "/VBG"
                treated=1
            if  ( ambigtag.count("Trans+Prog+VPROG") > 1  and treated==0):
                print word + "/VBG"
                treated=1
            if  ( ambigtag.count("Noun+Sg+NOUN") > 2  and treated==0):
                 print word + "/NN"
                 treated=1
            if  ( word=="-" and ambigtag.find("+Symbol+PUNCT") !=-1 and ambigtag.find("Punct+PUNCT") !=-1  and treated==0):
                print word + "/-"
                treated=1
            if  ( ambigtag.find("+Elid") !=-1 and treated==0):
                print word + "/POS"
                treated=1
            if  ( ambigtag.find("+TIT") !=-1 and treated==0):
                print word + "/NNP"
                treated=1
            if treated == 0:
                print  word + ambigtag 
    if current_line.find("|||") == -1 :
        normalcase=1
    if  ( normalcase==1 and current_line !=""):
        listw=current_line.split("\t")
        if len(listw) > 1 :
            tag=listw[2] +"\n" 
            word=listw[0]
            wordtag2=""
            done=0
            if (word=="RT"):
                 wordtag2= word + "/RT"
                 done =1
            if tag.find("+TweetUser") !=  -1: 
                wordtag2= word + "/USR" 
                done =1
            if tag.find("+HashTag") !=  -1: 
                wordtag2= word + "/HT" 
                done = 1
            if tag.find("+MEAS\n") !=  -1:
                wordtag2= word + "/NN"
            if tag.find("+Symbol\n") !=  -1:
                wordtag2= word + "/SYM"
            if (tag.find("+Symbol\n")==-1 and  tag.find("+COORD\n") !=  -1):
                wordtag2= word + "/CC"
            if (tag.find("+Coord") !=-1 and  tag.find("+COADV\n") !=  -1):
                wordtag2= word + "/CC"
            if tag.find("+DET\n") !=  -1:
                wordtag2= word + "/DT"
            if tag.find("+QUANT\n") !=  -1:
                wordtag2= word + "/DT"
            if tag.find("+INFTO\n") !=  -1:
                wordtag2= word + "/TO"
            if tag.find("+ADJ\n") !=  -1:
                wordtag2= word + "/JJ"
            if tag.find("+int_adj\n") !=  -1:
                wordtag2= word + "/JJ"
            if tag.find("+ADJPRON\n") !=  -1:
                wordtag2= word + "/JJ"
            if tag.find("+ADJING\n") !=  -1:
                wordtag2= word + "/JJ"
            if tag.find("+ADJPAP\n") !=  -1:
                wordtag2= word + "/JJ"
            if tag.find("+ADJCMP\n") !=  -1:
                wordtag2= word + "/JJR"
            if tag.find("+URL") !=  -1:
                wordtag2= word + "/URL"
                done=1
            if tag.find("+ADJSUP\n") !=  -1:                wordtag2= word + "/JJS"
            if ( ( tag.find("+Prop") !=-1  or tag.find("+Init") !=-1 ) and tag.find("+NOUN\n")!= -1 and done !=1):
                toprint=""
                if  word.find(" ") != -1:
                    eachw=word.split(" ")
                    for elem in eachw:
                        toprint=toprint + elem + "/NNP\n"
                    wordtag2=   toprint +"@"
                    wordtag2=wordtag2.replace("\n@","")
                    wordtag2=wordtag2.replace("\"R\"/NNP","``/``\nR/NNP\n''/''")
                else:
                    wordtag2=  word + "/NNP"
            if ( tag.find("+prof") != -1 and tag.find("+NOUN\n")!= -1 and word[0].isupper() and done !=1 ):
                wordtag2= word + "/NNP"
            else:
                if ( tag.find("+Prop") == -1 and tag.find("+NOUN\n")!= -1 and done !=1):
                     wordtag2= word + "/NN"
            if (tag.find("+int_noun\n")!= -1  and done !=1 ):
                wordtag2= word + "/NN"
            if (tag.find("+int_noadv\n")!= -1  ):
                wordtag2= word + "/RB"
            if (tag.find("+int_prop\n")!= -1 and done !=1 ):
                wordtag2= word + "/NNP"
            if tag.find("+TIT\n") !=  -1:
                wordtag2= word + "/NNP"
            if tag.find("+NOUNING\n") !=  -1 and done !=1:
                wordtag2= word + "/NN"
            if tag.find("+ITJ\n") !=  -1:
                wordtag2= word + "/UH"
            if tag.find("+CARD\n") !=  -1:
                if word.find("%")!=-1:
                    init=word.index("%")
                    wordtag2= word[0:init]+ "/CD\n%/NN"
                else:
                    wordtag2= word + "/CD"
            if tag.find("+CARDONE\n") !=  -1:
                wordtag2= word + "/CD"
            if tag.find("+PRONPERS\n") !=  -1:
                wordtag2= word + "/PRP"
            if ( tag.find("+Poss") != -1 and tag.find("+PRON\n")!= -1  ):
                wordtag2 = word + "/PRP$"
            if ( tag.find("+Poss") == -1 and tag.find("+PRON\n")!= -1  ):
                wordtag2 = word + "/PRP"
            if tag.find("+PRONONE\n") !=  -1:
                wordtag2= word + "/PRP"
            if tag.find("+PRONREFL\n") !=  -1:
                wordtag2= word + "/PRP"
            if tag.find("+ADV\n") !=  -1:
                wordtag2= word + "/RB"
            if tag.find("+int_adv\n") !=  -1:
                wordtag2= word + "/RB"
            if tag.find("+NOT\n") !=  -1:
                wordtag2= word + "/RB"
            if tag.find("+QUANTADV\n") !=  -1:
                wordtag2= word + "/RB"
            if tag.find("+ADVCMP\n") !=  -1:
                wordtag2= word + "/RBR"
            if tag.find("+ADVSUP\n") !=  -1:
                wordtag2= word + "/RBS"
            if tag.find("+QUANTSUP\n") !=  -1:
                wordtag2= word + "/RBS"
            if tag.find("+PREP\n") !=  -1:
                wordtag2= word + "/IN"
            if tag.find("+COSUB\n") !=  -1:
                wordtag2= word + "/IN"
            if tag.find("+COTHAN\n") !=  -1:
                wordtag2= word + "/IN"
            if tag.find("+VBI\n") !=  -1:
                wordtag2= word + "/VB"
            if tag.find("+VBPAP\n") !=  -1:
                wordtag2= word + "/VBN"
            if tag.find("+VBPAST\n") !=  -1:
                wordtag2= word + "/VBD"
            if ( tag.find("+3sg") != -1 and tag.find("+VBPRES\n")!= -1  ):
                if tag.find("=not") != -1 :
                    #print "FYF" + word
                    if word.find("n't") != -1:
                        init=word.index("n't")
                        wordtag2 = word[0:init] + "/VBZ\n" + "n't/RB"
                    if word.find("N'T") != -1:
                        init=word.index("N'T")
                        wordtag2 = word[0:init] + "/VBZ\n" + "N'T/RB"
                else:
                    wordtag2= word + "/VBZ"
            if ( tag.find("+3sg") == -1 and tag.find("+VBPRES\n")!= -1  ):
                if tag.find("=not") != -1 :
                    init=word.index("n't")
                    wordtag2 = word[0:init] + "/VBP\n" + "n't/RB"
                else:
                    wordtag2= word + "/VBP"
            if tag.find("+VBPROG\n") !=  -1:
                wordtag2= word + "/VBG"
            if tag.find("+VDPAP\n") !=  -1:
                wordtag2= word + "/VBN"
            if tag.find("+VDPAST\n") !=  -1:
                if tag.find("=not") != -1 :
                    init=word.index("n't")
                    wordtag2 = word[0:init] + "/VBD\n" + "n't/RB"
                else:
                    wordtag2= word + "/VBD"
            if tag.find("+VDPROG\n") !=  -1:
                wordtag2= word + "/VBG"
            if tag.find("+VHPAP\n") !=  -1:
                wordtag2= word + "/VBN"
            if tag.find("+VHPAST\n") !=  -1:
                if tag.find("=not") != -1 :
                    init=word.index("n't")
                    wordtag2 = word[0:init] + "/VBD\n" + "n't/RB"
                else:
                    wordtag2= word + "/VBD"
            if tag.find("+VHPROG\n") !=  -1:
                wordtag2= word + "/VBG"
            if tag.find("+VPAP\n") !=  -1:
                wordtag2= word + "/VBN"
            if tag.find("+VPAST\n") !=  -1:
                wordtag2= word + "/VBD"
            if tag.find("+VPROG\n") !=  -1:
                wordtag2= word + "/VBG"
            if ( tag.find("+3sg") !=-1 and tag.find("+VERB\n")!= -1  ):
                wordtag2= word + "/VBZ"
            if ( tag.find("+Non3sg") !=-1 and tag.find("+VERB\n")!= -1  ):
                wordtag2= word + "/VBP"
            if tag.find("+int_verb\n")!= -1:
                wordtag2= word + "/VBP"
            if ( tag.find("+3sg") !=-1 and tag.find("+HAVE\n")!= -1  ):
                if tag.find("=not") != -1 :
                    init=word.index("n't")
                    wordtag2 = word[0:init] + "/VBZ\n" + "n't/RB"
                else:
                    wordtag2= word + "/VBZ"
            if ( tag.find("+Non3sg") !=-1 and tag.find("+HAVE\n")!= -1  ):
                if tag.find("=not") != -1 :
                    init=word.index("n't")
                    wordtag2 = word[0:init] + "/VBP\n" + "n't/RB"
                else:
                    wordtag2= word + "/VBP"
            if ( tag.find("+3sg") !=-1 and tag.find("+DO\n")!= -1  ):
                if tag.find("=not") != -1 :
                    init=word.index("n't")
                    wordtag2 = word[0:init] + "/VBZ\n" + "n't/RB"
                else:
                    wordtag2= word + "/VBZ"
            if ( tag.find("+Non3sg") !=-1 and tag.find("+DO\n")!= -1  ):
                if tag.find("=not") != -1 :
                    if word.find("n't") != -1:
                        init=word.index("n't")
                        wordtag2 = word[0:init] + "/VBP\n" + "n't/RB"
                    if  word.find("N'T") != -1:
                        init=word.index("N'T")
                        wordtag2 = word[0:init] + "/VBP\n" + "N'T/RB"
                else:
                    wordtag2= word + "/VBP"
            if tag.find("+VS\n") !=  -1:
                wordtag2= word + "/VBZ"
            if tag.find("+VAUX\n") !=  -1:
                if tag.find("=not") != -1 :
                    if ((word!="cannot")):
                        init=word.index("n't")
                        wordtag2 = word[0:init] + "/MD\n" + "n't/RB"
                    else:
                        wordtag2 = "can/MD\nnot/RB"
                else:
                    wordtag2= word + "/MD"
            if tag.find("+WDET\n") !=  -1:
                wordtag2= word + "/WDT" 
            if tag.find("+WPRON\n") !=  -1:
                wordtag2= word + "/WP"
            if tag.find("+DETREL\n") !=  -1:
                wordtag2= word + "/WP$"
            if tag.find("+PRONREL\n") !=  -1:
                wordtag2= word + "/WDT"
            if tag.find("+NADJ\n") !=  -1 and done != 1: #ONLY IN XIP
                wordtag2= word + "/NN|JJ"
            if tag.find("+PREDET\n") !=  -1:
                wordtag2= word + "/PDT"
            if tag.find("+ORD\n") !=  -1:
                wordtag2= word + "/JJ"
            if tag.find("+PREPADVAS\n") !=  -1:
                wordtag2= word + "/IN"
            if tag.find("+POSS\n") !=  -1:
                wordtag2= word + "/POS"
            if tag.find("+QUANTCMP\n") !=  -1:
                wordtag2= word + "/JJR"
            if ( tag.find("+COADV\n") !=  -1 and tag.find("+Adv\n")) :
                wordtag2= word + "/RB"
            if ( tag.find("+WADV\n") !=  -1) :
                wordtag2= word + "/WRB"
            if ( tag.find("+QUANTSUP\n") !=  -1) :
                wordtag2= word + "/WRB"
        ### Money
            if ( tag.find("+MONEY\n") !=  -1) :
                toprint=""
                if  word[1:].find(" ") != -1:
                    eachw=word[1:].split(" ")
                    for elem in eachw:
                        toprint=toprint + elem + "/CD\n"
                    wordtag2= word[0]+ "/" + word[0] + "\n" + toprint +"@"
                    wordtag2=wordtag2.replace("\n@","")
                else:
                    wordtag2 = word[0]+ "/" + word[0] + "\n" + word[1:]+"/CD"
        ### Punctuation
            if tag.find("+CM\n") !=  -1:
                wordtag2= word + "/,"
            if ( word == "`"):
                 wordtag2= word + "/``"
            if ( word == "." and tag.find("+SENT\n") !=  -1):
                wordtag2= word + "/.\n"
            if ( word == "." and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/."
            if ( word == ":" and tag.find("+SENT\n") !=  -1):
                wordtag2= word + "/:"
            if ( word == "?" and tag.find("+SENT\n") !=  -1):
                wordtag2= word + "/?\n"
            if ( word == "!" and tag.find("+SENT\n") !=  -1):
                wordtag2= word + "/!\n"
            if (word =="\"" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/\""
            if (word =="\'" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/\""
            if (word =="(" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/("
            if (word ==")" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/)"
            if (word =="{" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/{"
            if (word =="}" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/}"
            if (word =="--" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/:"
            if (word =="-" and tag.find("+PUNCT\n") !=  -1 and tag.find("+Right+Left\n")):
                wordtag2= word + "/-RL"
            else:
                if (word =="-" and tag.find("+PUNCT\n") !=  -1 ):
                    wordtag2= word + "/-"
            if (word ==";" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/:"
            if (word ==":" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= word + "/:"
            if (word =="..." and tag.find("+SENT\n") !=  -1):
                wordtag2= word + "/:"
            if (word =="...." and tag.find("+SENT\n") !=  -1):
                wordtag2= word + "/:"
            if (word =="/" and tag.find("+PUNCT\n") !=  -1):
                 wordtag2= word + "//RL"
            if (word =="\\" and tag.find("+PUNCT\n") !=  -1):
                 wordtag2= word + "/\RL"
            if (word =="``" and tag.find("+PUNCT\n") !=  -1):
                 wordtag2= word + "/``"
            if (word =="\"" and tag.find("+PUNCT\n") !=  -1):
                 wordtag2= word + "/\""
            if (word =="@" and tag.find("+PUNCT\n") !=  -1):
                wordtag2= "@" + "/IN"
            if (word =="#" and tag.find("+PUNCT\n") !=  -1):
                wordtag2=  "#/#"
            if (word ==".)" and tag.find("+SENT\n") !=  -1):
                wordtag2=  "./.\n)/)"
            if (word =="---" and tag.find("+PUNCT\n") !=  -1):
                wordtag2=  "---/:"
            if (word == "=" and tag.find("+PUNCT\n") !=  -1):
                wordtag2=  "=/SYM"
            if (word == "|" and tag.find("+PUNCT\n") !=  -1):
                wordtag2=  "|/:"
            if (word == ".'.\""):
                wordtag2= "./.\n'/''\n./.\n''/''"
            if (word == ".\")"):
                wordtag2= "./.\n''/''\n)/)"
            print   wordtag2
