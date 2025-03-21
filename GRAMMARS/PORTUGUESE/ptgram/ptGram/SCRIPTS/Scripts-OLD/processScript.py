#!/usr/local/bin/python
# coding: latin-1

##############################################################################
# L2F - Spoken Language Systems Laboratory / INESC-ID Lisboa                 #
#                                                                            #
# Copyright (c) 2008 INESC-ID Lisboa.  All Rights Reserved.                  #
#                                                                            #
# --- CONTENTS ---                                                           #
#   Project    : Portuguese XIP grammar                                      #
#   filename   : processScript.py                                            #
#   Date       : 2008/Feb                                                    #
#   Author     : Nuno Mamede                                                 #
#                                                                            #
##############################################################################


import re
import string

from auxiliaryScript   import  int_norm
from auxiliaryScript   import  int_norm_int
from auxiliaryScript   import  month_norm
from auxiliaryScript   import  month_convert
from auxiliaryScript   import  year_norm



##############################################################################
# Faz correcções às horas, tratando as expressões não numéricas:
#    - meio-dia, meia-noite
#    - da tarde, da noite
#    - um quarto para a, menos um quarto
#    - três quartos
#    - para o
#    - menos
#    - meia
##############################################################################
def process_hour_special(node,data):
   #DEBUG print 'PROCESS_HOUR_SPECIAL: node = ' + node
   #DEBUG print 'date = ' + str(data)

   ################################################
   # Meio-dia
   ################################################
   pat1 = re.compile('meio-dia', re.I)
   if (pat1.search(node.lower() ) != None): data['hour'] = '12'

   ################################################
   # Meia-noite
   ################################################
   pat1 = re.compile('meia-noite', re.I)
   if (pat1.search(node.lower() ) != None): data['hour'] = '00'

   ################################################
   # da noite, da tarde
   ################################################
   pat1 = re.compile('(de o noite)|(de o tarde)', re.I)
   if ((data['hour'] != '--') and (pat1.search(node.lower() ) != None) and (int(data['hour']) < 12)):
      data['hour'] = int_norm_int(int(data['hour']) + 12)

   ################################################
   # quartos
   ################################################
   pat0 = re.compile('^para o', re.I)

   pat1 = re.compile('(um quarto para o)|(um quarto para um)|(menos um quarto)', re.I)
   pat2 = re.compile('(para o)|(para um)|(menos)', re.I)
   pat3 = re.compile('um quarto', re.I)
   pat4 = re.compile('três quartos', re.I)

   if (pat0.search(node.lower()) == None): # Do nothing when "Para as doze horas e trinta minutos"
      if ((data['hour'] != '--') and (pat1.search(node.lower()) != None)):
         if (int(data['hour']) == 0):
            data['hour'] = '23'
         else:
            data['hour'] = int_norm_int(int(data['hour']) - 1)
         data['minute'] = '45'
      elif ((data['hour'] != '--') and (data['minute'] != '--') and (pat2.search(node.lower()) != None)):
         if (int(data['hour']) == 0):
            data['hour'] = '23'
         else:
            data['hour'] = int_norm_int(int(data['hour']) - 1)
         data['minute'] = int_norm_int(60 - int(data['minute']))
      elif (pat3.search(node.lower()) != None):
         data['minute'] = '15'
      elif (pat4.search(node.lower()) != None):
         data['minute'] = '45'

   ################################################
   # meias
   ################################################
   pat0 = re.compile('(desde meio de\s)', re.I)
   if (pat0.search(node.lower()) != None):
      return


   pat0 = re.compile('(a meio\s)', re.I)
   pat1 = re.compile('(meio para o)|(meio para um)|(menos meio$)|(menos meio\s)', re.I)
   pat2 = re.compile('(meio\s)|(meio$)', re.I)
   if (pat0.search(node.lower()) != None):
      j = 1
   elif ((data['hour'] != '--') and (pat1.search(node.lower()) != None)):
      if (int(data['hour']) == 0):
         data['hour'] = '23'
      else:
         data['hour'] = int_norm_int(int(data['hour']) - 1)
      data['minute'] = '30'
   elif (pat2.search(node.lower()) != None):
      data['minute'] = '30'


   #DEBUG print 'date = ' + str(data)


##############################################################################
# Calcula o factor associado a unidades temporais não padrão
##############################################################################
def process_fixed_delta(node,delta):
   #DEBUG print 'process_fixed_delta: node = ' + node
   #DEBUG print 'process_fixed_delta INPUT date = ' + str(delta)

   # ATENTION: node is the <surface> string

   if (node.lower() == 'amanhã'): delta['monthday'] = 1
   elif (node.lower() == 'em a véspera'): delta['monthday'] = 1
   elif (node.lower() == 'em as vésperas'): delta['monthday'] = 1
   elif (node.lower() == 'ontem'): delta['monthday'] = 1
   elif (node.lower() == 'de ontem'): delta['monthday'] = 1
   elif (node.lower() == 'depois de amanhã'): delta['monthday'] = 2
   elif (node.lower() == 'de depois de amanhã'): delta['monthday'] = 2
   elif ((node.lower() == 'anteontem') or (node.lower() == 'antes de ontem')): delta['monthday'] = 2
   elif ((node.lower() == 'de anteontem') or (node.lower() == 'de antes de ontem')): delta['monthday'] = 2
   else:

      pat1 = re.compile('(de ontem a)|(em as? vésperas? de)', re.I)
      pat3 = re.compile('(de anteontem a)|(de antes de ontem a)', re.I)
      if (pat3.search(node.lower()) != None): delta['monthday'] += 2
      elif (pat1.search(node.lower()) != None): delta['monthday'] += 1

      pat2 = re.compile('de amanhã a', re.I)
      pat4 = re.compile('de depois de amanhã a', re.I)
      if (pat4.search(node.lower()) != None): delta['monthday'] -= 2
      elif (pat2.search(node.lower()) != None): delta['monthday'] -= 1

      pat5 = re.compile('(8 dias)|(15 dias)|(oito dias)|(quinze dias)|(uma quinzena de dias)', re.I)
      if (pat5.search(node.lower() ) != None):
         delta['monthday'] -= 1

   #DEBUG print 'process_fixed_delta OUTPUT date = ' + str(delta)
   return



##############################################################################
# Calcula o factor associado a unidades temporais não padrão
##############################################################################
def process_fixed_absolute_date(node,date):
   #DEBUG print 'process_fixed_absolute_date: node = ' + node
   #DEBUG print 'date = ' + str(date)

   #if ((date['year'] == "----") and (date['month'] == "--") and (date['monthday'] == "--")):
   #   return

   # O ano está especificado
   #    e é possível determinar o mês e/ou dia
   if (date['year'] != "----"):
      pat0 = re.compile(' féria de ', re.I)
      pat1 = re.compile('[Nn]atal', re.I)
      if (pat0.search(node.lower()) == None):

         if (pat1.search(node.lower()) != None):
            date['month'] = '12'
            date['monthday'] = '25'

   if (date['monthday'] != "--"):
      pat1 = re.compile('o véspera de', re.I)
      pat2 = re.compile('(\d+)(?= dia antes)', re.I)
      pat3 = re.compile('(\d+)(?= dia depois)', re.I)
      pat4 = re.compile('(\d+)(?= semana antes)', re.I)    ############## em falta
      pat5 = re.compile('(\d+)(?= semana depois)', re.I)   ############## em falta

      # Véspera
      if (pat1.search(node.lower()) != None):
         date['monthday'] = int_norm_int(int(date['monthday'])-1)

      # 2 dias antes
      time = pat2.search(node.lower())
      if (time != None):
         date['monthday'] = int_norm_int(int(date['monthday'])-(int(time.group(0))))

      # 2 dias depois
      time = pat3.search(node.lower())
      if (time != None):
         date['monthday'] = int_norm_int(int(date['monthday'])+(int(time.group(0))))

   if (date['month'] != "--"):
      pat2 = re.compile('(\d+)(?= mês antes)', re.I)
      pat3 = re.compile('(\d+)(?= mês depois)', re.I)

      # 2 meses antes
      time = pat2.search(node.lower())
      if (time != None):
         date['month'] = int_norm_int(int(date['month'])-(int(time.group(0))))

      # 2 meses depois
      time = pat3.search(node.lower())
      if (time != None):
         date['month'] = int_norm_int(int(date['month'])+(int(time.group(0))))


   if (date['year'] != "--"):
      pat2 = re.compile('(\d+)(?= ano antes)', re.I)
      pat3 = re.compile('(\d+)(?= ano depois)', re.I)

      # 2 anos antes
      time = pat2.search(node.lower())
      if (time != None):
         date['year'] = int_norm_int(int(date['year'])-(int(time.group(0))))

      # 2 anos depois
      time = pat3.search(node.lower())
      if (time != None):
         date['year'] = int_norm_int(int(date['year'])+(int(time.group(0))))

   return



##############################################################################
# Calcula o factor associado a unidades temporais não padrão
##############################################################################
def process_complex_delta(node,delta):
   #DEBUG print 'process_complex_delta: node = ' + node

   pat1 = re.compile('(em o dia (seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo dia)', re.I)
   if (pat1.search(node.lower()) != None): delta['monthday'] += 1

   pat1 = re.compile('(em a semana (seguinte|anterior|passada|(que (vem|passou|(há-de de vir)))))|(em a próxima semana)', re.I)
   if (pat1.search(node.lower()) != None): delta['week'] += 1

   pat1 = re.compile('(em a quinzena (seguinte|anterior|passada|(que (vem|passou|(há-de de vir)))))|(em a próxima quinzena)', re.I)
   if (pat1.search(node.lower()) != None): delta['week'] += 2

   pat1 = re.compile('(em o mês (seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo mês)', re.I)
   if (pat1.search(node.lower()) != None): delta['month'] += 1

   pat1 = re.compile('(em o trimestre (seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo trimestre)', re.I)
   if (pat1.search(node.lower()) != None): delta['month'] += 3

   pat1 = re.compile('(em o semestre s(seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo semestre)', re.I)
   if (pat1.search(node.lower()) != None): delta['month'] += 6

   pat1 = re.compile('(em o ano (seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo ano)', re.I)
   if (pat1.search(node.lower()) != None): delta['year'] += 1

   pat1 = re.compile('(em a década (seguinte|anterior|passada|(que (vem|passou|(há-de de vir)))))|(em a próxima década)', re.I)
   if (pat1.search(node.lower()) != None): delta['monthday'] += 10

   pat1 = re.compile('(em o século (seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo século)', re.I)
   if (pat1.search(node.lower()) != None): delta['year'] += 100

   pat1 = re.compile('(em o milénio (seguinte|anterior|passado|(que (vem|passou|(há-de de vir)))))|(em o próximo milénio)', re.I)
   if (pat1.search(node.lower()) != None): delta['year'] += 1000

   if ((node.lower() == 'amanhã') or (node.lower() == 'ontem')): delta['monthday'] = 1

   return



##############################################################################
#
##############################################################################
def process_fixed_meio(node,duration):

   mynode = node.lower()
   #DEBUG print 'process_fixed_meio: node = ' + mynode

   if (mynode.find('um segundo e meio')   != -1): duration['second']   += 0
   elif (mynode.find('segundo e meio')    != -1): duration['second']   += 1
   elif (mynode.find('um minuto e meio')  != -1): duration['second']   += 0
   elif (mynode.find('minutos e meio')    != -1): duration['second']   += 0
   elif (mynode.find('minuto e meio')     != -1): duration['second']   += 60
   elif (mynode.find('uma hora e meia')   != -1): duration['minute']   += 0
   elif (mynode.find('horas e meia')      != -1): duration['minute']   += 0
   elif (mynode.find('hora e meia')       != -1): duration['minute']   += 60
   elif (mynode.find('um dia e meio')     != -1): duration['hour']     += 0
   elif (mynode.find('dias e meio')       != -1): duration['hour']     += 0
   elif (mynode.find('dia e meio')        != -1): duration['hour']     += 24
   elif (mynode.find('uma semana e meia') != -1): duration['hour']     += (3 *24) + 12
   elif (mynode.find('semanas e meia')    != -1): duration['hour']     += (3 *24) + 12
   elif (mynode.find('semana e meia')     != -1): duration['hour']     += (10*24) + 12
   elif (mynode.find('um mês e meio')     != -1): duration['monthday'] += 0
   elif (mynode.find('meses e meio')      != -1): duration['monthday'] += 0
   elif (mynode.find('mês e meio')        != -1): duration['monthday'] += 30
   elif (mynode.find('uma década e meia') != -1): duration['year']     += 5
   elif (mynode.find('décadas e meia')    != -1): duration['year']     += 5
   elif (mynode.find('década e meia')     != -1): duration['year']     += 15
   elif (mynode.find('um século e meio')  != -1): duration['year']     += 50
   elif (mynode.find('séculos e meio')    != -1): duration['year']     += 50
   elif (mynode.find('século e meio')     != -1): duration['year']     += 150
   elif (mynode.find('um milénio e meio') != -1): duration['year']     += 500
   elif (mynode.find('milénios e meio')   != -1): duration['year']     += 500
   elif (mynode.find('milénio e meio')    != -1): duration['year']     += 1500

   return



##############################################################################
# Calcula o factor associado a unidades temporais não padrão
##############################################################################
def process_hour_complex(node,date):
   #DEBUG print 'HOUR: node = ' + node
   #DEBUG print 'date = ' + str(date)

   number_str  = node.replace(':',' ')
   number_str  = number_str.replace('H',' ')
   number_str  = number_str.replace('h',' ')
   number_str  = number_str.replace('m',' ')
   number_str  = number_str.replace(',',' ')
   number_str  = number_str.split()
   number_size = len(number_str)

   number_int = []
   for x in number_str:
      number_int.append(int(x))

   # is a decimal  number
   pat1 = re.compile(',', re.I)
   if (pat1.search(node) != None):
      date['hour'] = int_norm(number_str[0])
      date['minute'] = int_norm(str(number_int[1] * 6))
      return

   if ((number_size >= 1) and (number_int[0] <= 24)):
      date['hour'] = int_norm(number_str[0])

   if ((number_size >= 2) and (number_int[1] <= 60)):
      date['minute'] = int_norm(number_str[1])

   if ((number_size == 3) and (number_int[2] <= 60)):
      date['second'] = int_norm(number_str[2])



##############################################################################
#
##############################################################################
def process_date_complex(node,date):
   #DEBUG print '***************************************\nDATE: node = ' + node
   #DEBUG print 'date = ' + str(date)

   number_str = node.replace('/',' ')
   number_str = number_str.replace("."," ")
   number_str = number_str.replace(':',' ')
   number_str = number_str.replace('-',' ')
   number_str = number_str.split()
   number_size = len(number_str)
   #DEBUG print 'number = ' + str(number)

   number_int = []
   for x in number_str:
      mes = month_convert(x)
      if (mes == '00'):
        number_int.append(int(x))
      else:
        number_int.append(int(mes))

   # if letters are present and has 3 fields
   pat = re.compile('[A-Za-z]+')
   if ((pat.search(node.lower()) != None) and (number_size == 3)):
      # If Month is the first token
      #  prefers Day as the second token
      if (pat.match(number_str[0]) != None):
         date['month'] = month_convert(number_str[0])
         if (number_int[1] <= 31):
            date['monthday'] = int_norm(number_str[1])
            date['year']     = year_norm(number_str[2])
         else:
            date['monthday'] = int_norm(number_str[2])
            date['year']     = year_norm(number_str[1])

      # If Month is the second token
      #  prefers Day as the first token
      elif (pat.match(number_str[1]) != None):
         date['month'] = month_convert(number_str[1])
         if (number_int[0] <= 31):
            date['monthday'] = int_norm(number_str[0])
            date['year']     = year_norm(number_str[2])
         else:
            date['monthday'] = int_norm(number_str[2])
            date['year']     = year_norm(number_str[0])

      # If Mont is the third token
      #  prefers Day as the first token
      elif (pat.match(number_str[2]) != None):
         date['month'] = month_convert(number_str[2])
         if (number_int[0] <= 31):
            date['monthday']  = int_norm(number_str[0])
            date['year']      = year_norm(number_str[1])
         else:
            date['monthday']  = int_norm(number_str[1])
            date['year']      = year_norm(number_str[0])
      return


   # if letters are present and only has 2 fields
   if ((pat.search(node.lower()) != None) and (number_size == 2)):
      #if Month is the first token
      #  prefers Day as the second token
      if (pat.match(number_str[0]) != None):
         date['month'] = month_convert(number_str[0])
         if (number_int[1] <= 31):
            date['monthday'] = int_norm(number_str[1])
         else:
            date['year'] = year_norm(number_str[1])

      #if Month is the second token
      #  prefers Day as the first token
      elif (pat.match(number_str[1]) != None):
         date['month'] = month_convert(number_str[1])
         if (number_int[0] <= 31):
            date['monthday'] = int_norm(number_str[0])
         else:
            date['year'] = year_norm(number_str[0])
      return


   # If letters are not present (only digits)
   if (number_size == 3):
      # When Year is the first token
      #  prefers Month as the second token
      if ((len(number_str[0]) >= 3) or (number_int[0] > 31)):
         date['year']  = year_norm(number_str[0])
         if (number_int[1] <= 12):
            date['month']    = month_norm(number_str[1])
            date['monthday'] = int_norm(number_str[2])
         else:
            date['month']    = month_norm(number_str[2])
            date['monthday'] = int_norm(number_str[1])

      # When Year is the second token
      #  prefers Month as the first token
      elif ((len(number_str[1]) >= 3) or (number_int[1] > 31)):
         date['year']  = year_norm(number_str[1])
         if (number_int[0] <= 12):
            date['month']    = month_norm(number_str[0])
            date['monthday'] = int_norm(number_str[2])
         else:
            date['month']    = month_norm(number_str[2])
            date['monthday'] = int_norm(number_str[0])

      # When Year is the third token
      #  prefers Month as the second token
      elif ((len(number_str[2]) >= 3) or (number_int[2] > 31)):
         date['year']  = year_norm(number_str[2])
         if (number_int[1] <= 12):
            date['month']    = month_norm(number_str[1])
            date['monthday'] = int_norm(number_str[0])
         else:
            date['month']    = month_norm(number_str[0])
            date['monthday'] = int_norm(number_str[1])

      # When Month is the first token
      #  prefers monthday as the second token
      elif ((number_int[1] > 12) and (number_int[2] > 12)):
         date['month']  = month_norm(number_str[0])
         if (number_int[1] <= 31):
            date['monthday'] = int_norm(number_str[1])
            date['year']    = year_norm(number_str[2])
         else:
            date['monthday'] = int_norm(number_str[2])
            date['year']    = year_norm(number_str[1])

      # When Month is the second token
      #  prefers monthday as the first token
      elif ((number_int[0] > 12) and (number_int[2] > 12)):
         date['month']  = month_norm(number_str[1])
         if (number_int[0] <= 31):
            date['monthday'] = int_norm(number_str[0])
            date['year']    = year_norm(number_str[2])
         else:
            date['monthday'] = int_norm(number_str[2])
            date['year']    = year_norm(number_str[0])

      else:
         date['monthday'] = int_norm(number_str[0])
         date['month']  = month_norm(number_str[1])
         date['year']    = year_norm(number_str[2])

   return
