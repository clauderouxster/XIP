Python:

sauve_entites(#1,string db).


def sauve_entites(i,db):
    #print("bonjour")
    n=XipNode(i)
    lem=n.lemma
    lem=lem.split("|")[0]
    pos=n.pos
    if (lem[0].isupper()):
      entites=pos+':'+lem
      #entites=lem
      cursor=connexion(db)
      try:
        commande='select NoEntite from entites where Texte="'+entites+'";'
        nb=cursor.execute(commande)
        if nb>0:
          res=cursor.fetchone()
          commande='update entites set Frequence=Frequence+1 where Texte="'+entites+'";'
          nb=cursor.execute(commande)
          return int(res[0])
        else:
          commande='insert into entites values(0,"'+entites+'","1");'
          nb=cursor.execute(commande)
          commande='select NoEntite from entites where Texte="'+entites+'";'
          nb=cursor.execute(commande)
          res=cursor.fetchone()
          return int(res[0])
      except MySQLdb.Error, e:
         print "Error %d: %s" % (e.args[0], e.args[1])
    else:
        return 0
