import time
import xips


def lance():
    log=open("H:\\parSem\\xip\\dev\\GRAMMARS\\English\\APPLICATIONS\\COREF\\DB_scripts\\log.txt","w")
    xips.loadgrm("H:\\parSem\\xip\\dev\\GRAMMARS\\English\\APPLICATIONS\\COREF\\coref_db.grm")
    print xips.whoami()
    xips.setdisplay(0,xips.XIP_DISPLAY_NONE)

    path="H:\\corpus\\reuters\\REUTERSEXPANDED\\news"
    debut=time.time()
    toto=""
    
    for i in range(0,5):
	nom=path+"-%05d"%i+".txt"
	xips.setstring(0,"path",nom)
	xips.parsefile(0,nom,toto)
	fin=time.time()
	s= "fichier:"+nom+"  Temps ecoule:"+str(fin-debut)+"s\n"
	log.write(s)
	print "Temps ecoule:",fin-debut

    i=5
    print "FINAL..........................."
    nom=path+"-%05d"%i+".txt"
    xips.setstring(0,"path",nom)
    xips.setint(0,"_finalsave",1)
    xips.parsefile(0,nom,toto)
    fin=time.time()
    print "Temps final:",fin-debut
    s= "fichier:"+nom+"  Temps final:"+str(fin-debut)+"s\n"
    log.write(s)
	
lance()
