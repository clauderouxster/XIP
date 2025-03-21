
"""
Python:
clean_dictionary(),
write_to_file(string value),
normalize_date(#1, int sentence_id),
normalize_hour(#1, int sentence_id),
normalize_duration(#1, int sentence_id),
normalize_tdelta(#1, int sentence_id),
eval(#1).
"""
#!/usr/local/bin/python
# coding: latin-1

##############################################################################
# L2F - Spoken Language Systems Laboratory / INESC-ID Lisboa                 #
#                                                                            #
# Copyright (c) 2008 INESC-ID Lisboa.  All Rights Reserved.                  #
#                                                                            #
# --- CONTENTS ---                                                           #
#   Project    : Portuguese XIP grammar                                      #
#   filename   : timeScript.py                                               #
#   Date       : 2007/May, 2008/Feb                                          #
#   Author     : Joao Loureiro & Nuno Mamede                                 #
#                                                                            #
##############################################################################


import sys
import os
import re
import string

from numbersScript   import  ext2num
from lemmasScript    import  convert_factor

from processScript   import  process_hour_complex
from processScript   import  process_date_complex
from processScript   import  process_fixed_delta
from processScript   import  process_fixed_absolute_date
from processScript   import  process_complex_delta
from processScript   import  process_hour_special
from processScript   import  process_fixed_meio

from auxiliaryScript import  get_feature_val
from auxiliaryScript import  has_feature
from auxiliaryScript import  year_norm
from auxiliaryScript import  season_norm
from auxiliaryScript import  month_norm
from auxiliaryScript import  int_norm
from auxiliaryScript import  int_norm_int


##############################################################################
# Renove ficheiro com resultados (inicializacao)
##############################################################################
def clean_dictionary():
   if os.path.exists('./timeDictionary.txt'):
      os.remove('./timeDictionary.txt')
      #print "ficheiro removido"
   return 1



##############################################################################
# escreve as datas normalizadas para um ficheiro
##############################################################################
def write_to_file(value):
   file = open('./timeDictionary.txt', 'a')
   file.write(value)
   file.close()



##############################################################################
# Has grand-daughter
##############################################################################
def has_grand_daughter(node):
   if (node.daughter == -1):
      return -1
   new = XipNode(node.daughter)
   return (new.daughter)



##############################################################################
# Dado um nodeID devolve o (primeiro) no' (interno) da categoria NUM
##############################################################################
def search_num(node):
   if ((node.pos == 'NUM') or (has_feature(node.features, 'NUM'))):
      return node

   node = XipNode(node.daughter)
   while (True):
      if (node.pos == 'NUM'):
         return node
      if (node.next != -1):
         node = XipNode(node.next)
      else:
         break

   # nao encontrou o no'
   return 0



##############################################################################
# Dado um nodeID e uma feature devolve o no' que contem essa feature
##############################################################################
def search_node(node, feat):

   lemma = (unicode(node.lemma, 'utf-8').encode('latin-1')).split()
   lemma_size = len(lemma)
   # o que se procura esta' neste no' se o lema for composto por apenas 1 palavra
   if (lemma_size == 1):
      return node

   # se o que se procura nao esta' neste no', procura-se nos sub-nos
   node = XipNode(node.daughter)

   while(True):
      if (has_feature(node.features, feat)):
         lemma = (unicode(node.lemma, 'utf-8').encode('latin-1')).split()
         lemma_size = len(lemma)
         if (lemma_size == 1):
            return node
      if (node.next != -1):
         node = XipNode(node.next)
      else:
         break

   # nao encontrou o no'
   return 0



##############################################################################
##############################################################################
##############################################################################
#
#                       NORMALIZA DATAS
#
##############################################################################
##############################################################################
##############################################################################
def normalize_date(node_id, sentence_id):
   #DEBUG print 'normalize_date'

   # This object keeps the values (strings)
   date = {
      'era':       "+",
      'year':      "----",
      'month':     "--",
      'monthday':  "--",
      'hour':      "--",
      'minute':    "--",
      'second':    "--",
      'season':    "--",
      'boundary':  "-"
   }

   Xipnode = XipNode(node_id)
   if (has_feature(Xipnode.features, 'NO_NORMALIZATION')): return

   fill_in_date(Xipnode, date)
   fill_in_boundary(Xipnode, date)
   process_fixed_absolute_date(unicode(Xipnode.lemma, 'utf-8').encode('latin-1'), date)
   process_hour_special(unicode(Xipnode.lemma, 'utf-8').encode('latin-1'), date)
   escreve_date(date, sentence_id, str(Xipnode.index), str(Xipnode.leftoffset)+':'+str(Xipnode.rightoffset), Xipnode.surface)
   return 1



##############################################################################
# Preenche uma estrutura do tipo data
##############################################################################
def fill_in_date(Xipnode, date):

   #DEBUG debug_date(date)

   continue_recursion = 1

   if (has_feature(Xipnode.features, 'NO_NORMALIZATION')):
      #DEBUG print 'Processing a NO_NORMALIZATION'
      return

   if (has_feature(Xipnode.features, 'TIME_FOCUS')):
      #DEBUG print 'Processing a TIME_FOCUS'
      continue_recursion = 0

   if (has_feature(Xipnode.features, 'PART_OF_DAY')):
      #DEBUG print 'Processing a PART_OF_DAY'
      continue_recursion = 0

   # processa estacao do ano
   elif (has_feature(Xipnode.features, 'SEASON')):
      #DEBUG print 'Processing a SEASON'
      continue_recursion = 0
      node_aux = search_node(Xipnode, 'SEASON')
      node_val = get_feature_val(node_aux.features, 'SEASON')
      if (node_val != '+'):
         date['season'] = season_norm(node_val)

   # processa o ano
   elif (has_feature(Xipnode.features, 'YEAR')):
      #DEBUG print 'Processing a YEAR'
      continue_recursion = 0
      node_aux = search_num(Xipnode)
      node_lemma = unicode(node_aux.lemma, 'utf-8').encode('latin-1')
      if (node_lemma.find('/')   == -1):
         if (node_lemma.isdigit()):
            date['year'] = year_norm(node_lemma)
         else:
            node_num = ext2num(node_lemma)
            date['year'] = year_norm(str(node_num))

      if (has_feature(Xipnode.features, 'ERA')):
         #print 'Processing a ERA'

         if ((unicode(Xipnode.lemma, 'utf-8').encode('latin-1')).find('aC')  != -1): date['era'] = '-'

   # processa o mes
   elif (has_feature(Xipnode.features, 'MONTH')):
      #DEBUG print 'Processing a MONTH'
      continue_recursion = 0
      node_aux = search_node(Xipnode, 'MONTH')
      # se nao encontrou um mes entao procura por um numero
      if (not node_aux):
         node_aux = search_num(Xipnode)
      node_lemma = unicode(node_aux.lemma, 'utf-8').encode('latin-1')
      if (node_lemma.isdigit()):
         date['month'] = int_norm(node_lemma)
      else:
         node_val = get_feature_val(node_aux.features, 'MONTH')
         if (node_val != '+'):
            date['month'] = int_norm(node_val)


   # process complex hours (string has to be parsed)
   elif (has_feature(Xipnode.features, 'HORA_COMPLEX')) :
      #DEBUG print 'Processing a HORA_COMPLEX'
      continue_recursion = 0
      process_hour_complex(unicode(Xipnode.lemma, 'utf-8').encode('latin-1'),date)


   # process complex dates (string has to be parsed)
   elif (has_feature(Xipnode.features, 'DATA_COMPLEX')):
      #DEBUG print 'Processing a DATA_COMPLEX'
      continue_recursion = 0
      process_date_complex(unicode(Xipnode.lemma, 'utf-8').encode('latin-1'),date)


   # processa dias, horas, minutos e segundos
   else:
      teste = ['MONTHDAY', 'HOUR', 'MINUTE', 'SECOND']
      for index, value in enumerate(teste):
         if ((has_feature(Xipnode.features, value))):
            #DEBUG print 'Processing a ' + value
            continue_recursion = 0
            node_aux = search_num(Xipnode)
            if (node_aux == 0): continue
            node_lemma = unicode(node_aux.lemma, 'utf-8').encode('latin-1')
            if (not node_lemma.isdigit()):
               node_lemma = str(ext2num(node_lemma))
            if (has_feature(node_aux.features, 'H_TO_COMPUTE')):
               #DEBUG print 'H_TO_COMPUTE'
               factor = convert_factor(value.lower(),node_aux)
               #DEBUG print 'factor =' + str(factor)
               node_lemma = str(factor * int(node_lemma))
            date[value.lower()] = int_norm(node_lemma)




   ###############################################################
   # When the node contains relevant information stops recursion
   # otherwise, processes all the daugther nodes (if they exist)
   #
   # To avoid processing lexical nodes, only daughter nodes that
   # have another daughter are explored
   ###############################################################
   if ((continue_recursion == 0) or (has_grand_daughter(Xipnode) == -1)):
      return 1

   ####################################
   # Starts recursion on daughter nodes
   ####################################
   new_Xipnode = XipNode(Xipnode.daughter)
   fill_in_date(new_Xipnode, date)

   while (new_Xipnode.next != -1):
      new_Xipnode = XipNode(new_Xipnode.next)
      fill_in_date(new_Xipnode, date)

   return 1


##############################################################################
# Preenche o campo Boundary de uma estrutura do tipo data
##############################################################################
def fill_in_boundary(Xipnode, date):

   #DEBUG print 'Processing boundary'

   if (has_feature(Xipnode.features, 'OPEN_RIGHT')):
      date['boundary'] = 'P'
   if (has_feature(Xipnode.features, 'OPEN_LEFT')):
      date['boundary'] = 'A'
   return


##############################################################################
#
##############################################################################
def normalize_hour(node_id, sentence_id):
   normalize_date(node_id, sentence_id)


##############################################################################
# Imprime uma estrutura DATE
##############################################################################
def escreve_date(date, sentence_id, indice, node_id, node_surface):
   if ((date['era']       == '+')    and
       (date['year']      == '----') and
       (date['month']     == '--')   and
       (date['monthday']  == '--')   and
       (date['hour']      == '--')   and
       (date['minute']    == '--')   and
       (date['season']    == '--' )  and
       (date['boundary']  == '-' )): return

   if (len(date['year']) > 4):
      date['year']      = '9999'
      date['month']     = '99'
      date['monthday']  = '99'
      date['hour']      = '99'
      date['minute']    = '99'
      date['season']    = '--'
      date['boundary']  = '-'

   final_string = 'SENTENCE: ' + str(sentence_id) + ', INDEX: ' + str(indice) + ', NODE: ' + str(node_id) + ', SURFACE: ' + node_surface + ', VALUE: '
   final_string += date['era'] + date['year'] + date['month'] + date['monthday']
   final_string += "T" + date['hour'] + date['minute']
   final_string += "E" + date['season']
   final_string += "LM" + date['boundary'] + '\n'
   write_to_file(final_string)

def debug_date(date):
   print "+++++++++++++++++++++++"
   final_string = date['era'] + date['year'] + date['month'] + date['monthday']
   final_string += "T"  + date['hour'] + date['minute']
   final_string += "E"  + date['season']
   final_string += "LM" + date['boundary'] + '\n'
   print final_string
   print "+++++++++++++++++++++++"






##############################################################################
##############################################################################
##############################################################################
##############################################################################
#
#                           DURATIONS and DELTAS
#
##############################################################################
##############################################################################
##############################################################################
##############################################################################




##############################################################################
# Preenche uma estrutura do tipo data
##############################################################################
def fill_in_delta(node,delta,headnode):
   fill_in_duration(node,delta,0,headnode)



##############################################################################
# Normaliza periodos de tempo
##############################################################################
def normalize_duration(node_id, sentence_id):
   #DEBUG print 'normalize_duration'

   # This object keeps the values (strings)
   duration = {
      'year':      0,
      'month':     0,
      'week':      0,
      'monthday':  0,
      'hour':      0,
      'minute':    0,
      'second':    0
   }

   Xipnode = XipNode(node_id)
   if (has_feature(Xipnode.features, 'NO_NORMALIZATION')):
      #DEBUG print 'Processing a NO_NORMALIZATION (normalize_tdelta)'
      return

   fill_in_duration(Xipnode, t-duration, 0, Xipnode)
   reduce(duration)
   escreve_duration(duration, sentence_id, str(Xipnode.index), str(Xipnode.leftoffset)+':'+str(Xipnode.rightoffset), Xipnode.surface)




##############################################################################
# Preenche uma estrutura do tipo data
##############################################################################
def fill_in_duration(node, t-duration, skip, headnode):

   #DEBUG print 'fill_in_duration IN node=' + node.surface

   continue_recursion = 1

   skip = skip or (has_feature(node.features, 'NO_NORMALIZATION'))

   if (not skip):

      teste = ['YEAR', 'MONTH', 'WEEK', 'MONTHDAY', 'HOUR', 'MINUTE', 'SECOND']
      for index, value in enumerate(teste):
         if (has_feature(node.features, value)):
            #print 'Processing a ' + value
            continue_recursion = 0
            node_aux = search_num(node)
            #print 'search_num ' + str(node_aux)
            if (node_aux == 0):
               #print 'MEIO----------H_TO_COMPUTE  surface=' + node.surface
               if (((node.surface == 'meio') or (node.surface == 'meia')) and (has_feature(node.features, 'H_TO_COMPUTE'))):
                  factor = convert_factor(value.lower(),node)
                  #print 'MEIO----------H_TO_COMPUTE  factor=' + str(factor)
                  if (factor != -1):
                     duration[value.lower()] += factor
                  process_fixed_meio(headnode.surface,duration)
                  #debug_delta(duration)
               break
            node_lemma = unicode(node_aux.lemma, 'utf-8').encode('latin-1')
            if (node_lemma.isdigit()):
               node_lemma = int(node_lemma)
            else:
               node_lemma = ext2num(node_lemma)
            #DEBUG print 'node lemma =' + str(node_lemma)
            if (has_feature(node_aux.features, 'H_TO_COMPUTE')):
               factor = convert_factor(value.lower(),node_aux.next)
               #print 'H_TO_COMPUTE  factor=' + str(factor)
               duration[value.lower()] += factor * node_lemma
            else:
               duration[value.lower()] += node_lemma
            #debug_delta(duration)
            break

   # process complex dates (string has to be parsed)
   if (has_feature(node.features, 'HORA_COMPLEX')):
      continue_recursion = 0
      process_hour_complex(unicode(node_aux.lemma, 'utf-8').encode('latin-1'),duration)


   ###############################################################
   # When the node contains relevant information stops recursion
   # otherwise, processes all the daugther nodes (if they exist)
   #
   # To avoid processing lexical nodes, only daughter nodes that
   # have another daughter are explored
   ###############################################################
   if ((continue_recursion == 0) or (has_grand_daughter(node) == -1)):
      return 1

   ####################################
   # Starts recursion on daughter nodes
   ####################################
   new_node = XipNode(node.daughter)
   fill_in_duration(new_node,t-duration,skip,headnode)

   while (new_node.next != -1):
      new_node = XipNode(new_node.next)
      #DEBUG print 'node surface =' + new_node.surface
      fill_in_duration(new_node, t-duration,skip,headnode)

   return 1



##############################################################################
#
##############################################################################
def normalize_tdelta(node_id, sentence_id):

   #DEBUG print 'normalize_tdelta'
   # This object keeps the values (strings)
   delta = {
      'year':      0,
      'month':     0,
      'week':      0,
      'monthday':  0,
      'hour':      0,
      'minute':    0,
      'second':    0
   }

   Xipnode = XipNode(node_id)
   if (has_feature(Xipnode.features, 'NO_NORMALIZATION')):
      #DEBUG print 'Processing a NO_NORMALIZATION (normalize_tdelta)'
      return

   node_surface = Xipnode.surface
   process_fixed_delta(node_surface,delta)
   if (has_feature(Xipnode.features, 'H_TO_COMPUTE')):
      process_complex_delta(node_surface,delta)
   else:
      fill_in_delta(Xipnode,delta,Xipnode)

   reduce(delta)
   escreve_duration(delta, sentence_id, str(Xipnode.index), str(Xipnode.leftoffset)+ ':' + str(Xipnode.rightoffset), node_surface)
   return 1




##############################################################################
# Imprime uma estrutura DURATION
##############################################################################
def escreve_duration(duration, sentence_id, indice, node_id, node_surface):
   if ((duration['year']      == 0) and
       (duration['month']     == 0) and
       (duration['week']      == 0) and
       (duration['monthday']  == 0) and
       (duration['hour']      == 0) and
       (duration['minute']    == 0) and
       (duration['second']    == 0) and
       (node_surface != 'hoje')): return

   final_string = 'SENTENCE: ' + str(sentence_id) + ', INDEX: ' + str(indice) + ', NODE: ' + str(node_id) + ', SURFACE: ' + node_surface + ', VALUE: '
   final_string += 'A' + str(duration['year']) + 'M' + str(duration['month']) + 'S' + str(duration['week'])
   final_string += 'D' + str(duration['monthday'])  + 'H' + str(duration['hour'])  + 'M' + str(duration['minute'])
   final_string += 'S' + str(duration['second']) + '\n'
   write_to_file(final_string)

def debug_delta(duration):
   print "+++++++++++++++++++++++"
   final_string  = 'A' + str(duration['year']) + 'M' + str(duration['month']) + 'S' + str(duration['week'])
   final_string += 'D' + str(duration['monthday'])  + 'H' + str(duration['hour'])  + 'M' + str(duration['minute'])
   final_string += 'S' + str(duration['second']) + '\n'
   print final_string
   print "+++++++++++++++++++++++"



##############################################################################
# Calcula o factor associado a unidades temporais como semestre
##############################################################################
def reduce(duration):

   value = duration['second']
   if (value >= 60):
      part_int = value / 60
      if (part_int > 0):
         duration['second'] = value - (part_int * 60)
         duration['minute'] += part_int

   value = duration['minute']
   if (value >= 60):
      part_int = value / 60
      if (part_int > 0):
         duration['minute'] = value - (part_int * 60)
         duration['hour'] += part_int

   value = duration['hour']
   if (value >= 24):
      part_int = value / 24
      if (part_int > 0):
         duration['hour'] = value - (part_int * 24)
         duration['monthday'] += part_int

   duration['monthday'] += duration['week'] * 7
   duration['week'] = 0

   value = duration['monthday']
   if (value >= 30):
      part_int = value / 30
      if (part_int > 0):
         duration['monthday'] = value - (part_int * 30)
         duration['month']   += part_int

   value = duration['monthday']
   if (value >= 7):
      part_int = value / 7
      if (part_int > 0):
         duration['monthday'] = value - (part_int * 7)
         duration['week'] += part_int

   value = duration['month']
   if (value >= 12):
      part_int = value / 12
      if (part_int > 0):
         duration['month'] = value - (part_int * 12)
         duration['year']  += part_int




##############################################################################
##############################################################################
##############################################################################
#
#             Tests recursion on a node with daughters
#
##############################################################################
##############################################################################
##############################################################################
def test_recursion(Xipnode):

   continue_recursion = 1
   print "Staring. Node_id=" + str(Xipnode.index)

   # processa o ano
   if (has_feature(Xipnode.features, 'YEAR')):
      continue_recursion = 0

   # processa o mes
   if (has_feature(Xipnode.features, 'MONTH')):
      continue_recursion = 0

   # processa o dia do mes
   if (has_feature(Xipnode.features, 'day')):
      continue_recursion = 0

   # When the node contains relevant information stops recursion
   # otherwise, processes all the daugther nodes (if they exist)
   if ((continue_recursion == 0) or (has_grand_daughter(Xipnode) == -1)):
      print "  Finishing (no daughter). Node_id=" + str(Xipnode.index)
      return 1

   # Starts recursion on daughter nodes
   new_Xipnode = XipNode(Xipnode.daughter)
   print "  First daughter. New_node_id=" + str(new_Xipnode.index)
   test_recursion(new_Xipnode)

   while (new_Xipnode.next != -1):
      new_Xipnode = XipNode(new_Xipnode.next)
      print "  Next daughter" + str(new_Xipnode.index)
      test_recursion(new_Xipnode)

   print "Finishing (with daughter). Node_id=" + str(Xipnode.index)
   return 1



##############################################################################
#
##############################################################################
def eval(node_id):
   n = XipNode(node_id)
   value = n.surface + '\n'
   write_to_file(value)



