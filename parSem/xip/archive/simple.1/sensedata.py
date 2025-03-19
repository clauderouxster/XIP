# -*- coding: utf8 -*-
######################################################
#Author: Claude Roux
#Date: 2008/01/01
#E-mail: mekalinga@free.fr
#File: sensedata.py
#Comment: This file implements the data for sense manipulation
#Copyright: This file can be freely copied and used as long as the original author (Claude Roux) is present

senses={}
#The following words in lingata can be used as semantics variables
#rete --> place
#reta --> thing
#reti --> time
#reto --> person
#huta --> human

#We translate the key by another word, if the sentence is recognized
#The sentence is stored in a wordata structure which is parsed on the fly and for which
#a graph is computed and stored back into the structure.
#We use this graph to propose a better translation, when needed
#The "#x" matches a word in the wordata
#The "$" means destruction of all related dependencies
#The "^" means that this new word should replace the current word

#senses["abitaiag"] = [{"wordata":"inar reted abitaiag","francais":["habiter"],"english":["dwell"]}]##senses["abitaiag"] = [{"wordata":"inar reted abitaiag","francais":["habiter"],"english":["dwell"]}]senses["abitaiag"] = [{"wordata":"inar reted abitaiag","francais":["habiter"],"english":["dwell"]}]

senses["uder"] = [{"wordata":"uder$ reieg helpeiag","francais":["à"],"english":["to"]},
                  {"wordata":"recan uder reieg beiag","francais":["à"],"english":["to"]}]
senses["reiag"] = [{"wordata":"iagno$ reiag#1","francais":["^verb(être),prep(en train),prep(de),verb(#1)"]}]


senses["udar"] = [{"wordata":"udar imageiagad","francais":["de"],"english":["to"]},
                  {"wordata":"recan udar reiagad beiag","francais":["à"],"english":["to"]},
                  {"wordata":"retan udar reiagad beiag","francais":["à"],"english":["to"]},
                  {"wordata":"retan udinars reiagad beiag","francais":["à"],"english":["to"]},
                  {"wordata":"recan udinars reiagad beiag","francais":["à"],"english":["to"]},
                  {"wordata":"retan udars reiagad bezeneiag","english":["to"]},
                  {"wordata":"recan udar reiagad beiag","francais":["à"],"english":["to"]}]

senses["poveiag"] = [{"wordata":"reta#1 recan#2 reiagas poveiag","francais":["accordadj","#1","#2"]}]
senses["deveiag"] = [{"wordata":"reta#1 reiagas#2 beiagas deveiag","francais":["accordadj","#1","#2"]}]

senses["kontentidjeiag"] = [{"wordata":"retan kontentidjeiag","francais":["pron(se),verb(contenter),prep(de)"]}]

senses["drinkedeiag"] = [{"wordata":"retan drinkedeiag#1", "francais":["verb(#1),prep(de)"]}]
senses["drinkeiag"] = [{"wordata":"retan drinkeiag#1", "francais":["verb(#1),prep(avec),prep(de)"]}]

senses["mula"] = [{"wordata":"rear mulad retad", "francais":["des"]}]

senses["paroleiag"] = [{"wordata":"kaers#2 paroleieg#1", "english":["prep(of),pron(#2),verb(#1)"]},
                       {"wordata":"koers#2 paroleieg#1", "english":["prep(of),pron(#2),verb(#1)"]}]

