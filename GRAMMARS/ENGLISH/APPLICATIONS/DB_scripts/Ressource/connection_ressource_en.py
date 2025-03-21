Python:

connexion(string dbname).


import MySQLdb

connection=0
cursor='vide'


def nettoie(cursor):
  commande="delete from entites;"
  cursor.execute(commande)
  commande="delete from labels;"
  cursor.execute(commande)
  commande="delete from relations;"
  cursor.execute(commande)
  commande="delete from dependances;"
  cursor.execute(commande)


def connexion(dbname):
   global cursor,connection
   if (connection==1):
      return cursor
   dbname=str(dbname)
   print "La connexion est:",dbname
   mydb = MySQLdb.connect(db=dbname, user="root",host="etretat",passwd="root")
   cursor=mydb.cursor()
   connection=1
   nettoie(cursor)
   return cursor


