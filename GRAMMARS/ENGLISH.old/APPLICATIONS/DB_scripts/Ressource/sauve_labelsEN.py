Python:

sauve_labels(#2,string db).


def sauve_labels(i,db):
    n=XipNode(i)
    lem=n.lemma
    lem=lem.split("|")[0]
    pos=n.pos
    label=pos+':'+lem
    cursor=connexion(db)
    try:
      commande='select NoLabel from labels where Texte="'+label+'";'
      nb=cursor.execute(commande)
      if nb>0:
        res=cursor.fetchone()
        return int(res[0])
      else:
        commande='insert into labels values(0,"'+label+'");'
        nb=cursor.execute(commande)
        commande='select NoLabel from labels where Texte="'+label+'";'
        nb=cursor.execute(commande)
        res=cursor.fetchone()
        return int(res[0])
    except MySQLdb.Error, e:
       print "Error %d: %s" % (e.args[0], e.args[1])
