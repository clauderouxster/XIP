Python:

sauve_relations(int idLab,int idEnt,int idDep,string db).

def sauve_relations(idLab,idEnt,idDep,db):
    cursor=connexion(db)
    idLab=str(idLab)
    idEnt=str(idEnt)
    idDep=str(idDep)
    try:
      commande='select NoRelation,Frequence from relations where NoLabel="'+idLab+'" AND NoEntite="'+idEnt+'" AND NoDependance="'+idDep+'";'
      nb=cursor.execute(commande)
      if nb>0:
        res=cursor.fetchone()
        fr=res[1]+1
        ind=str(res[0])
        f=str(fr)
        commande='update relations set Frequence='+f+' where NoRelation='+ind+';'
        nb=cursor.execute(commande)
      else:
        commande='insert into relations values(0,"'+idLab+'","'+idEnt+'","'+idDep+'","1");'
        nb=cursor.execute(commande)
        
    except MySQLdb.Error, e:
      print "Error %d: %s" % (e.args[0], e.args[1])
