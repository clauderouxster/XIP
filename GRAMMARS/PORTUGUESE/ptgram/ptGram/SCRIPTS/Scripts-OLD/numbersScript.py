#!/usr/local/bin/python
# coding: latin-1

##############################################################################
# L2F - Spoken Language Systems Laboratory / INESC-ID Lisboa                 #
#                                                                            #
# Copyright (c) 2008 INESC-ID Lisboa.  All Rights Reserved.                  #
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
numbers = {
   'zero':            '0',
   'um':              '1',
   'dois':            '2',
   'três':            '3',
   'quatro':          '4',
   'cinco':           '5',
   'seis':            '6',
   'sete':            '7',
   'oito':            '8',
   'nove':            '9',
   'dez':             '10',
   'dezembro':        '10',     # it is not a mistake (dez. is also December...)
   'dezena':          '10',
   'onze':            '11',
   'doze':            '12',
   'dúzia':           '12',
   'treze':           '13',
   'catorze':         '14',
   'quinze':          '15',
   'dezasseis':       '16',
   'dezassete':       '17',
   'dezoito':         '18',
   'dezanove':        '19',
   'vinte':           '20',
   'vintenas':        '20',
   'trinta':          '30',
   'trintena':        '30',
   'quarenta':        '40',
   'quarentena':      '40',
   'cinquenta':       '50',
   'cinquentena':     '50',
   'sessenta':        '60',
   'setenta':         '70',
   'oitenta':         '80',
   'noventa':         '90',
   'cem':             '100',
   'cento':           '100',
   'centena':         '100',
   'centenas':        '100',
   'duzentos':        '200',
   'trezentos':       '300',
   'quatrocentos':    '400',
   'quinhentos':      '500',
   'seiscentos':      '600',
   'setecentos':      '700',
   'oitocentos':      '800',
   'novecentos':      '900',
   'mil':             '1000',
   'milena':          '1000',
   'milhar':          '1000',
   'milhão':          '1000000',
   'milhões':         '1000000',
}



##############################################################################
# recebe uma lista e devolve o resultado da soma dos elementos dessa lista
##############################################################################
def add_num(number_list):
   new_num = 0
   for n in number_list:
      new_num += int(n)
   return new_num



##############################################################################
# recebe uma string e devolve um vector de strings de números e sem "e"
##############################################################################
def select_num(cadeia):
   # Para determinar uma entidade unicamente constituída por números
   pat = re.compile('[0-9]+')

   #DEBUG print 'select_num cadeia =' + cadeia

   number = cadeia.split()
   number_size = len(number)

   i = 0
   num = []
   while (i < number_size):
      n = number[i]
      if (n in numbers):
         num.append(numbers[n])
      elif (pat.match(n) != None):
          num.append(n)
      i = i + 1

   #DEBUG print 'select_num num =' + str(num)

   return num



##############################################################################
# le o numero e retira as conjuncoes "e"
##############################################################################
def ext2num(number):

   #DEBUG print 'ext2num number =' + number

   number = number.lower()
   number_str = number.replace('º','')      # º
   number_str = number_str.replace('ª','')  # ª
   if (number_str.isdigit()): return number_str2

   num = select_num(number_str)

   milhoes  = []
   milhares = []
   unidades = []
   num_size = len(num)
   pointer  = 0

   # verifica se '1000000' faz parte do numero, e processa
   if (('1000000' in num) and (num.index('1000000') > 0)):
      stop = num.index('1000000')
      while(pointer < stop):
         milhares.append(num[pointer])
         pointer += 1
      pointer += 1

   # verifica se '1000' faz parte do numero, e processa
   if (('1000' in num) and (num.index('1000') > 0)):
      stop = num.index('1000')
      while(pointer < stop):
         milhares.append(num[pointer])
         pointer += 1
      pointer += 1

   # processa unidades, dezenas e
   while(pointer < num_size):
      unidades.append(num[pointer])
      pointer += 1

   num1 = add_num(milhoes)  # junta a parte do numero que esta' antes do milhao
   num2 = add_num(milhares) # junta a parte do numero que esta' antes do mil e depois do milhao
   num3 = add_num(unidades) # junta a parte do numero que esta' depois do mil

   return num1 * 1000000 + num2 * 1000 + num3