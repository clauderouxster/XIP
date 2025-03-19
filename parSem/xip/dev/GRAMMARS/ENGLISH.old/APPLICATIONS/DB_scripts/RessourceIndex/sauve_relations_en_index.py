Python:



sauve_relations(int idLab,int idEnt,int idDep,string nfile,int nsentence,int left_EN,int right_EN,string db).


def sauve_relations(idLab,idEnt,idDep,nfile,nsentence,left_EN,right_EN,db):
    cursor=connexion(db)
    idLab=str(idLab)
    idEnt=str(idEnt)
    idDep=str(idDep)
    nsentence=str(nsentence)
#    node_EN=str(node_EN)
    left_EN=str(left_EN)
    right_EN=str(right_EN)
    nfile=str(nfile)
    try:
      commande='select NoRelation from relations where NoLabel="'+idLab+'" AND NoEntite="'+idEnt+'" AND NoDependance="'+idDep+'" AND Fichier="'+nfile+'" AND IdSentence="'+nsentence+'";'
      nb=cursor.execute(commande)
      if nb<1:
       commande='insert into relations values(0,'+idLab+','+idEnt+','+idDep+',1,"'+nfile+'",'+nsentence+','+left_EN+','+right_EN+');'
#       print(commande)
       nb=cursor.execute(commande)
    except MySQLdb.Error, e:
      print "Error %d: %s" % (e.args[0], e.args[1])





