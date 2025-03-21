#!/usr/local/bin/python
# coding: latin-1

##############################################################################
# L2F - Spoken Language Systems Laboratory / INESC-ID Lisboa                 #
#                                                                            #
# Copyright (c) 2008 INESC-ID Lisboa.  All Rights Reserved.                  #
#                                                                            #
# --- CONTENTS ---                                                           #
#   Project    : Portuguese XIP grammar                                      #
#   filename   : lemmasScript.py                                             #
#   Date       : 2008/Feb                                                    #
#   Author     : Nuno Mamede                                                 #
#                                                                            #
##############################################################################


import xips


##############################################################################
# Calcula o factor associado a unidades temporais como semestre
##############################################################################
def convert_factor(unit,node):

   #DEBUG print 'convert_factor = ' + unit + '       node =' + (xips.XipNode(node)).lemma


   fact_absoluto = {
      'semestre':       '6',
      'década':         '10',
      'século':         '100',
      'quinzena':       '2',
      'trimestre':      '3',
      'fim-de-semana':  '2',
   }

   # A previous version of the code when it had more structure
   #if (node.next == -1): return -1
   #new = xips.XipNode(node.next)
   #node_lemma = new.lemma.lower()

   node_lemma = unicode((xips.XipNode(node)).lemma, 'utf-8').encode('latin-1')

   # when meia is used without a previous number (eg., 2 anos e meio)
   if (node_lemma == 'NONE'):
      node_lemma= node.lemma
   #DEBUG print 'convert_factor node_lemma = ' + node_lemma

   # A unidade de medida é onde o valor inteiro tem de ser guardado
   # Exemplo:   "meio ano" corresponde a 6 meses, logo "mêsmeio"=6
   fact_relativo = {
       # expressa uma parte do ano  (1 ano = 12 meses)
      'monthmeio':       '6',
      'monthterço':      '4',
      'monthquarto':     '3',
      'monthsexto':      '2',
      'monthdécimo':     '1',

       # expressa uma parte do mes (1 mes = 30 dias)
      'monthdaymeio':    '15',
      'monthdayterço':   '10',
      'monthdayquarto':  '7',
      'monthdaysexto':   '5',
      'monthdaydécimo':  '3',

       # expressa uma parte do dia (1 dia = 24 horas)
      'hourmeio':       '12',
      'hourterço':      '8',
      'hourquarto':     '6',
      'hoursexto':      '4',
      'houroitavo':     '3',

       # expressa uma parte da hora (1 hora = 60 minutos)
      'minutemeio':     '30',
      'minuteterço':    '20',
      'minutequarto':   '15',
      'minutesexto':    '10',
      'minutedécimo':   '6',

       # expressa uma parte do minuto (1 minuto = 60 segundos)
      'secondmeio':     '30',
      'secondterço':    '20',
      'secondquarto':   '15',
      'secondsexto':    '10',
      'seconddécimo':   '6',
   }

   if (node_lemma in fact_absoluto):
      return int(fact_absoluto[node_lemma])

   new_lemma = unit.lower() + node_lemma
   if (new_lemma in fact_relativo):
      #DEBUG print ' fact rel = ' + fact_relativo[new_lemma]
      return int(fact_relativo[new_lemma])

   # como não sabe o que fazer, não faz nada...

   return -1

##############################################################################
#
##############################################################################

