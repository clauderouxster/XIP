Python:



sauve_dependance($1,string db).

import re

def sauve_dependance(refdep,db):
    xdep=XipDependency(refdep)
    rel=str(xdep.name)
    dep=rel
    featval=str(xdep.features)
    traits=featval
    sample=re.compile("LINKED").search(traits,1)
    if sample:
        val='periph'
    else:
        val='pur'

   # print("TRAITS")
    #print(traits)
   # features=featval.split(":")[0]
   # feature=features.replace('[','')
   # feat=feature.replace(']','')
   # dep=rel+'_'+feat
   # print (dep)
    cursor=connexion(db)
    try:
      commande='select NoDependance,Frequence from dependances where Dependance="'+dep+'" AND Traitdep="'+traits+'";'
      nb=cursor.execute(commande)
      
      if nb>0:
        res=cursor.fetchone()
        #fr=res[1]+1
        #ind=str(res[0])
        #f=str(fr)
        commande='update dependances set Frequence=Frequence+1 where Dependance="'+dep+'" AND Traitdep="'+traits+'" ;'
        nb=cursor.execute(commande)
        return int(res[0])
      else:  
        commande='insert into dependances values(0,"'+dep+'","'+traits+'","'+val+'","1");'
        nb=cursor.execute(commande)
        commande='select NoDependance from dependances where Dependance="'+dep+'" AND Traitdep="'+traits+'" ;'
        nb=cursor.execute(commande)
        res=cursor.fetchone()
        return int(res[0])
    except MySQLdb.Error, e:
      print "Error %d: %s" % (e.args[0], e.args[1])


# AND where Traitdep="'+traits+'"
# AND where Traitdep="'+traits+'"
#,"'+traits='"
