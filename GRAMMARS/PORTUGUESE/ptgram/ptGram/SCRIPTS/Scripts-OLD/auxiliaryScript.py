#!/usr/local/bin/python
# coding: latin-1

##############################################################################
# L2F - Spoken Language Systems Laboratory / INESC-ID Lisboa                 #
#                                                                            #
# Copyright (c) 2007 INESC-ID Lisboa.  All Rights Reserved.                  #
#                                                                            #
# --- CONTENTS ---                                                           #
#   Project    : Portuguese XIP grammar                                      #
#   filename   : auxiliaryScript.py                                          #
#   Date       : 2008/Feb                                                    #
#   Author     : Nuno Mamede                                                 #
#                                                                            #
##############################################################################


import re
import string


##############################################################################
# Dados uma feature e uma lista de features de um no', devolve o valor da respectiva feature
##############################################################################
def get_feature_val(features, feat):
   #DEBUG print "get_feature_val IN  features: " + features
   features = features.replace('[', '')
   features = features.replace(']', '')
   features = features.replace(':', ' ')
   features = features.replace(',', ' ')
   features = features.split()
   feat_index = features.index(feat)
   #DEBUG  "get_feature_val OUT features: " + features
   return features[(feat_index+1)]



##############################################################################
# Dada uma lista de features, devolve True se uma determinada feature se encontrar nessa lista
##############################################################################
def has_feature(features, feat):
   features = features.replace('[', '')
   features = features.replace(']', '')
   features = features.replace(':', ' ')
   features = features.replace(',', ' ')
   features = features.split()

   try:
      features.index(feat)
      x = 1
   except ValueError:
      x = 0

   return x



##############################################################################
# Converte as ANOS para a representacao desejada
##############################################################################
def year_norm(year):
   if ((len(year) == 1)):
      return "200" + year[0]
   if ((len(year) == 2) and (year[0] == "0")):
      return "20" + year[0] + year[1]
   elif (len(year) == 2):
      return "19" + year[0] + year[1]
   elif (len(year) == 3):
      return "0" + year[0] + year[1]+ year[2]
   else:
      return year



##############################################################################
# Converte as ESTACOES para a representacao desejada
##############################################################################
def season_norm(season):
   months = ['PR', 'VE', 'OU', 'IN']
   new_season = int(season)
   return months[(new_season-1)]



##############################################################################
# Converte os MESES para a representacao desejada
##############################################################################
def month_norm(month):
   months = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12']
   new_month = int(month)
   return months[(new_month-1)]



##############################################################################
# Converte os MESES para a representacao desejada
##############################################################################
def month_convert(month):
   #DEBUG print 'month = ' + month

   meses = {
      'jan':        '01',
      'fev':        '02',
      'mar':        '03',
      'abr':        '04',
      'mai':        '05',
      'jun':        '06',
      'jul':        '07',
      'ago':        '08',
      'set':        '09',
      'out':        '10',
      'nov':        '11',
      'dez':        '12',
      'janeiro':    '01',
      'fevereiro':  '02',
      'março':      '03',
      'abril':      '04',
      'maio':       '05',
      'junho':      '06',
      'julho':      '07',
      'agosto':     '08',
      'setembro':   '09',
      'outubro':    '10',
      'novembro':   '11',
      'dezembro':   '12',
   }

   if (month.lower() in meses):
      return meses[month]
   return '00'



##############################################################################
# Converte uma string que representa um INTEIRO para dois digitos
# (usado para converter Dias, Horas, Minutos e Segundos)
##############################################################################
def int_norm(day):
   if (len(day) == 1):
      return '0' + day
   return day



##############################################################################
# Converte um INTEIRO para uma string de dois digitos
# (usado para converter Dias, Horas, Minutos e Segundos)
##############################################################################
def int_norm_int(i):
   num = str(i)
   if (len(num) == 1):
      return '0' + num
   return num



##############################################################################
# Converte número ROMANO (string) para inteiro
##############################################################################
def roman_to_int(input):
   """
   Convert a roman numeral to an integer.

   >>> r = range(1, 4000)
   >>> nums = [int_to_roman(i) for i in r]
   >>> ints = [roman_to_int(n) for n in nums]
   >>> print r == ints
   1

   >>> roman_to_int('VVVIV')
   Traceback (most recent call last):
    ...
   ValueError: input is not a valid roman numeral: VVVIV
   >>> roman_to_int(1)
   Traceback (most recent call last):
    ...
   TypeError: expected string, got <type 'int'>
   >>> roman_to_int('a')
   Traceback (most recent call last):
    ...
   ValueError: input is not a valid roman numeral: A
   >>> roman_to_int('IL')
   Traceback (most recent call last):
    ...
   ValueError: input is not a valid roman numeral: IL
   """
   if type(input) != type(""):
      return 0
      #raise TypeError, "expected string, got %s" % type(input)
   input = input.upper()
   nums = ['M', 'D', 'C', 'L', 'X', 'V', 'I']
   ints = [1000, 500, 100, 50,  10,  5,   1]
   places = []
   for c in input:
      if not c in nums:
         return 0
         #raise ValueError, "input is not a valid roman numeral: %s" % input
   for i in range(len(input)):
      c = input[i]
      value = ints[nums.index(c)]
      # If the next place holds a larger number, this value is negative.
      try:
         nextvalue = ints[nums.index(input[i +1])]
         if nextvalue > value:
            value *= -1
      except IndexError:
         # there is no next place.
         pass
      places.append(value)
   sum = 0
   for n in places: sum += n
   # Easiest test for validity...
   if int_to_roman(sum) == input:
      return sum
   else:
      return 0;
      #raise ValueError, 'input is not a valid roman numeral: %s' % input