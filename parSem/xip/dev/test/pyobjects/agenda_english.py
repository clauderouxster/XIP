
# coding: iso-8859-1
# -*- coding: iso-8859-1 -*-

import time
import datetime
import xips
import win32com.client


olAppointmentItem=1


"""
Pour obtenir la liste des constantes OUTLOOK
http://www.winscripter.com/WSH/MSOffice/93.aspx
olAppointmentItem=1
"""
leschifs={}
leschifs["un"]=1
leschifs["une"]=1
leschifs["deux"]=2
leschifs["trois"]=3
leschifs["quatre"]=4
leschifs["cinq"]=5
leschifs["six"]=6
leschifs["sept"]=7
leschifs["huit"]=8
leschifs["neuf"]=9
leschifs["dix"]=10
leschifs["onze"]=11
leschifs["douze"]=12
leschifs["midi"]=12
leschifs["treize"]=13
leschifs["quatorze"]=14
leschifs["quinze"]=15
leschifs["seize"]=16
leschifs["dix-sept"]=17
leschifs["dix-huit"]=18
leschifs["dix-neuf"]=19
leschifs["vingt"]=20
leschifs["trente"]=30
leschifs["quarante"]=40
leschifs["cinquante"]=50
leschifs["soixante"]=60
leschifs["soixante-dix"]=70
leschifs["quatre-vingt"]=80
leschifs["quatre-vingt-dix"]=90
leschifs["cent"]=100
leschifs["mille"]=1000

lesdurees={}
lesdurees["mn"]=1
lesdurees["minute"]=1
lesdurees["s"]=0
lesdurees["seconde"]=0
lesdurees["heure"]=2
lesdurees["h"]=2

lesjours={}
lesjours["lundi"]=0
lesjours["mardi"]=1
lesjours["mercredi"]=2
lesjours["jeudi"]=3
lesjours["vendredi"]=4
lesjours["samedi"]=5
lesjours["dimanche"]=6
lesjours["aujourd'hui"]=-1
lesjours["demain"]=-2
lesjours["après-demain"]=-3
lesjours["hier"]=-4
lesjours["avant-hier"]=-5

lesmois={}
lesmois["janvier"]=1
lesmois["février"]=2
lesmois["fevrier"]=2
lesmois["mars"]=3
lesmois["avril"]=4
lesmois["mai"]=5
lesmois["juin"]=6
lesmois["juillet"]=7
lesmois["août"]=8
lesmois["aout"]=8
lesmois["septembre"]=9
lesmois["octobre"]=10
lesmois["novembre"]=11
lesmois["décembre"]=12
lesmois["decembre"]=12

lesheures={}
lesheures["après-midi"]=[14,00]
lesheures["matin"]=[9,30]
lesheures["soir"]=[18,00]

calendrier={}
calendrierindex={}


def chargeMessages():
    oOutlook = win32com.client.Dispatch("Outlook.Application")
    onMAPI = oOutlook.GetNamespace("MAPI")
    oMail = onMAPI.GetDefaultFolder(6) # 6=outlook inbox folder
    fld = onMAPI.Folders(1)
    oItems = oMail.Items
    print oItems.Count
    for i in range(oItems.Count):
        print oItems.Item(i+1).Body




def chargeroutlook(debut,fin,unan):
    #oOutlook = win32com.client.gencache.EnsureDispatch("Outlook.Application")
    oOutlook = win32com.client.Dispatch("Outlook.Application")
    onMAPI = oOutlook.GetNamespace("MAPI")
    oCalendar = onMAPI.GetDefaultFolder(9) # 9=outlook calendar folder
    oItems = oCalendar.Items
    strFind = "[Start] > "+'"'+debut+'"'+" AND [End] <"+'"'+fin+'"'
    oItems = oItems.Restrict(strFind)
    print 'Number of appointments:', str(oItems.Count)
    rdv=[]
    calendrier={}
    for i in range(oItems.Count):
        rdv.append(oItems.Item(i+1))
    for r in rdv:
        majindex(r)


def ajoutecalend(u,rdv):
    global calendrierindex
    global calendrier
    s=u.strip().lower()
    if s=="#" or s=="." or s=="," or s=="?" or len(s)==1 or s=="":
        return
    
    cle=str(rdv.Start)
    try:
        if cle not in calendrierindex[s]:
            calendrierindex[s].append(cle)
    except:
        calendrierindex[s]=[cle]



def litcalend(u,rep,maxocc,avecdate):
    global calendrierindex
    global calendrier
    s=u.strip().lower()
    if s=="#" or s=="." or s=="," or s=="?" or len(s)==1 or s=="":
        return
    
    try:
        dd=calendrierindex[s]
        for d in dd:
            e=calendrier[d]
            for j in e:
                ind=str(j.Start)
                try:
                    rep[ind]+=1
                    if maxocc[0]<rep[ind]:
                        maxocc[0]=rep[ind]
                except:
                    if not avecdate:
                        rep[ind]=1
    except:
        pass

def ajoutecal(s,dt,rdv):
    global calendrier
    
    i=s+str(dt)
    try:
        if rdv not in calendrier[i]:
            calendrier[i].append(rdv)
    except:
        calendrier[i]=[rdv]

def majindex(rdv):
    
    dt=rdv.Start
    cle=str(dt)
    try:
        if rdv not in calendrier[cle]:
            calendrier[cle].append(rdv)
    except:
        calendrier[cle]=[rdv]

    s="A"
    ajoutecal(s,dt.year,rdv)
    s+=str(dt.year)+"M"
    ajoutecal(s,dt.month,rdv)
    s+=str(dt.month)+"J"
    ajoutecal(s,dt.day,rdv)
    s+=str(dt.day)+"H"
    ajoutecal(s,dt.hour,rdv)
    
    
    sujet=rdv.Subject.split(' ')
    for u in sujet:
        ajoutecalend(u,rdv)

    sujet=rdv.Body.split(' ')
    for u in sujet:
        ajoutecalend(u,rdv)
    sujet=rdv.Location.split(',')
    for u in sujet:
        ajoutecalend(u,rdv)


def comparedate(d1,d2):
    if d1<d2:
        return 1
    if d1==d2:
        return 0
    return -1

def compareocc(e1,e2):
    if e1[1]<e2[1]:
        return 1
    if e1[1]==e2[1]:
        return comparedate(e2[0],e1[0])
    return -1

def comparesurdate(e1,e2):
    res=comparedate(e1[0],e2[0])
    if res==0:
        if e1[1]<e2[1]:
            return 1
        if e1[1]==e2[1]:
            return 0
        return -1    
    return res

def recherche(rdv,avecdate,doubledate,avecjour,avecheure):
    global calendrierindex
    global calendrier
    rep=[]
    reponse={}
    maxocc=[0]
    if avecdate:
        dt=rdv.Start
        if avecheure:
            if dt in calendrier:
                rep.append(calendrier[dt])
            else:
                s="A"+str(dt.year)+"M"+str(dt.month)+"J"+str(dt.day)+"H"+str(dt.hour)
                if s in calendrier:
                    rep.append(calendrier[s])                
        else:
            if avecjour and doubledate!=2:
                s="A"+str(dt.year)+"M"+str(dt.month)+"J"+str(dt.day)
                if s in calendrier:
                    rep.append(calendrier[s])
            else:
                s="A"+str(dt.year)+"M"+str(dt.month)
                if s in calendrier:
                    rep.append(calendrier[s])
    #On elimine les doublons et on met la liste a plat
        maxocc=[10]
        for r in rep:
            for rr in r:
                #le premier chiffre indique le nombre d'occurence
                #dans le cas d'une date, on lui donne un poids tres fort
                if doubledate==2:
                    #on verifie si la date est dans l'interval
                    print rr.Start,":",rdv.Start,rdv.End
                    if rr.Start>=rdv.Start and rr.Start<=rdv.End:
                        reponse[str(rr.Start)]=10
                else:
                    reponse[str(rr.Start)]=10
    
    sujet=rdv.Subject.split(' ')
    for u in sujet:
        litcalend(u,reponse,maxocc,avecdate)
    sujet=rdv.Body.split(' ')
    for u in sujet:
        litcalend(u,reponse,maxocc,avecdate)
    sujet=rdv.Location.split(',')
    for u in sujet:
        litcalend(u,reponse,maxocc,avecdate)

    maxocc=maxocc[0]
    reponses=[]
    rep=[]
    for r in reponse:
        rep.append([r,reponse[r]])
    
    ongardetout=False
    if not avecdate or maxocc==10:
        ongardetout=True
        
    rep.sort(compareocc)
    for r in rep:
        force=0
        if r[1]==maxocc:
            force=3
        elif  ongardetout: 
            if r[1]>= (maxocc-maxocc/3):
                force=2
            else:
                force=1
        elif r[1]>10:
            force=2
        if force:
            for rr in calendrier[r[0]]:
                reponses.append([rr,force])
    return reponses





def cree(datedepart,datearrivee,lieu,sujet,phrase):
    global calendrier
    outlook = win32com.client.Dispatch("Outlook.Application")
    #namespace = outlook.GetNamespace("MAPI")
    #app=namespace.GetDefaultFolder(9).Items
    rdv= outlook.CreateItem(olAppointmentItem)    
    rdv.Subject=sujet
    rdv.Start=datedepart.strftime("%d/%m/%Y %H:%M")
    rdv.End=datearrivee.strftime("%d/%m/%Y %H:%M")
    rdv.Body=phrase
    rdv.Location=lieu    
    majindex(rdv)
    rdv.Save()

def questionnement(datedepart,datearrivee,lieu,sujet,phrase,avecdate,doubledate,avecjour,avecheure):
    global calendrier
    outlook = win32com.client.Dispatch("Outlook.Application")
    #namespace = outlook.GetNamespace("MAPI")
    #app=namespace.GetDefaultFolder(9).Items
    rdv= outlook.CreateItem(olAppointmentItem)    
    rdv.Subject=sujet
    rdv.Start=datedepart.strftime("%d/%m/%Y %H:%M")
    rdv.End=datearrivee.strftime("%d/%m/%Y %H:%M")
    rdv.Body=phrase
    rdv.Location=lieu    
    return recherche(rdv,avecdate,doubledate,avecjour,avecheure)

def init():
    xips.loadgrm("C:\\eclipse\\adresse\\src\\adresse.grm");

def rechercheajdpasse(u,n):
    fils=n
    while fils!=None:
        if fils.pos[0]=="ADJ" and "PASSE:+" in fils.features[0]:
            filsbis=u.fils(fils)
            while filsbis!=None:
                if filsbis.pos[0]=="NUM":
                    val=-1
                    if filsbis.lemma[0].isdigit():
                        val=int(filsbis.lemma[0])
                    else:
                        try:
                            val=leschifs[filsbis.lemma[0]]
                        except:
                            val=0               
                    #petite variation bizarre
                    if val==15:
                        val=7
                    elif val==8:
                        val=0     
                    return val
                filsbis=u.frere(filsbis)
        fils=u.frere(fils)
                
                
#nous avons le codage suivant pour les traits de date:
# mois, jour, annee, moisdec, dec
def calculdate(u,d):
    #plusieurs cas
    #ce n'est qu'un chiffre
    n=u.nodes[d.parameters[0]]
    auj=datetime.datetime.now()
    ladate=auj
    f=d.features
    lejour=-1
    lemois=-1
    lannee=-1
    jourdiff=None
    lefils=u.fils(n)
    avecjour=False
    if "SLASHE:+" in f:
        listejours=lefils.surface.split("/")
        lejour=int(listejours[0])
        lemois=int(listejours[1])
        if len(listejours)==3:
            lannee=int(listejours[2])
    else:
        if "DEC:+" in f:
            #on recupere la date du jour
            #Il s'agit du premier num
            fils=lefils
            while fils!=None:
                if fils.pos[0]=="NUM":                
                    lejour=int(fils.surface)
                    avecjour=True
                    if lejour <= auj.day:
                        lemois=auj.month+1
                        if lemois>12: #si on est en décembre, faut en tenir compte
                            lemois=1
                    break
                fils=u.frere(fils)
        if "MOIS:+" in f:
            #on recupere la date du jour
            #Il s'agit du premier num
            fils=lefils
            while fils!=None:
                if fils.pos[0]=="NOUN" and fils.lemma[0] in lesmois:
                    lemois=lesmois[fils.lemma[0]]
                    break
                fils=u.frere(fils)
        if "ANNEE:+" in f:
            #on recupere la date du jour
            #Il s'agit du premier num
            fils=lefils
            while fils!=None:
                if fils.pos[0]=="NUM" and "BASE:4" in fils.features:
                    lannee=int(fils.surface)
                    break
                fils=u.frere(fils)
        if "JOUR:+" in f:
            #on recupere la date du jour
            #Il s'agit du premier num
            fils=lefils
            increm=0
            multi=1
            avecjour=True
            if "ILYA:+" in f:
                #On recherce le ADJ avec ce trait
                if "PASREF:+" in f:
                    increm=rechercheajdpasse(u,n)
                    increm-=1
                else:
                    increm=rechercheajdpasse(u,lefils)
                multi=-1
            if "PASSE:+" in f:
                #dans ce cas l'increment est negatif
                multi=-1
            if "DECALAGE:+" in f:
                increm=deltaduree(lefils.lemma[0])
            else:                
                if "ENHUIT:+" in f:
                    increm=7
                if "ENQUINZE:+" in f:
                    increm=14
                while fils!=None:
                    if fils.lemma[0] in lesjours:
                        ind=lesjours[lefils.lemma[0]]
                        indauj=auj.weekday()
                        if ind==-1:
                            jourdiff=0
                        elif ind==-2:
                            jourdiff=1
                        elif ind==-3:
                            jourdiff=2
                        elif ind==-4:
                            jourdiff=-1
                        elif ind==-5:
                            jourdiff=-2
                        else:
                            if multi==1:
                                jourdiff=ind-indauj
                                if jourdiff<=0:
                                    jourdiff=7+jourdiff           
                            else:
                                #dans le cas inverse
                                jourdiff=ind-indauj-7
                                         
                        break
                    fils=u.frere(fils)
            #si la date est incomplete, on renvoie None
            if jourdiff==None:
                return None
            jourdiff+=increm*multi
     
    #On regarde ce qui peut manquer
    #Si l'on a le jour, le mois et l'annee, c'est bon
    if lejour!=-1 and lemois!=-1 and lannee!=-1:
       #On retourne une date
       ladate=datetime.datetime(lannee,lemois,lejour)
    elif lejour!=-1 and lemois!=-1:
       ladate=datetime.datetime(auj.year,lemois,lejour)
    elif lejour!=-1:
       ladate=datetime.datetime(auj.year,auj.month,lejour)
    elif lemois!=-1:
        ladate=datetime.datetime(auj.year,lemois,1)
    elif jourdiff!=None:
       ladate=auj+datetime.timedelta(jourdiff)
    return [ladate,avecjour]


def extraitheure(u,n,dfeat):
    lefils=u.fils(n)
    hm=None
    try:
        hmm=lesheures[lefils.lemma[0]]
        return hmm
    except:
        pass
    
    if "DEUXPOINT:+" in lefils.features[0]:
        hm=lefils.surface.split(":")
    
    if "UNH:+" in lefils.features[0]:
        hm=lefils.surface.split("h")
    
    if hm==None:
        if lefils.surface.isdigit():
            hm=[int(lefils.surface),0]        
        else:
            try:
                hm=[leschifs[lefils.surface],0]
            except:
                hm=[12,0]
        if hm[0]<8 and "DECALAGE:+" not in dfeat:
            hm[0]+=12
    try:
        hmm=[int(hm[0]),int(hm[1])]
    except:
        hmm=[int(hm[0]),0]
    return hmm
    
def calculheure(u,d):
    n=u.nodes[d.parameters[0]]
    return extraitheure(u,n,d.features)

def deltaduree(val):
    if not val.isdigit():
        try:
            val=leschifs[val]
        except:
            val=0
    return val

def calculduree(u,d,daterepere):
    n=u.nodes[d.parameters[0]]
    lefils=u.fils(n)
    duree=None
    
    if "SEMAINE:+" in n.features[0]:
        auj=datetime.datetime.now()
        #cas horriblement particulier
        if lefils.lemma[0]=="la semaine prochaine":
            duree=datetime.timedelta(days=7)
            return [duree,0]
        elif daterepere!=None:
            #la semaine du 14 avril, par exemple
            #On calcule le numero de la semaine courante
            numsem=daterepere.isocalendar()[1]
            numsemcourante=auj.isocalendar()[1]
            #la duree est x7 ce nombre, a condition que cela ne soit pas l'annee suivante
            if numsem>=numsemcourante:
                duree=datetime.timedelta(days=7*(numsem-numsemcourante))
            else:
                #c'est un peu plus complique...
                #il nous faut la semaine du dernier jour de l'annee
                dernierjour=datetime.datetime(daterepere.year,12,31)
                dersem=dernierjour.isocalendar()[1]
                duree=datetime.timedelta(days=7*(dersem-numsemcourante+numsem))
            #On va se positionner sur le lundi
            datesuivante=auj+duree
            js=datesuivante.weekday()
            duree=datetime.timedelta(days=duree.days-js)
            return [duree,auj]
    
    if "MEASURE:+" in lefils.features[0]:
        lemme=lefils.lemma[0]
        #Plusieurs cas: 
        i=lemme.rfind("mn")
        if i!=-1:
            val=deltaduree(lemme[:i])
            duree=datetime.timedelta(minutes=int(val))
    
        i=lemme.rfind("s")
        if i!=-1:
            val=deltaduree(lemme[:i])
            duree=datetime.timedelta(seconds=int(val))
        i=lemme.rfind("h")
        if i!=-1:
            val=extraitheure(u,lefils,d.features)
            duree=datetime.timedelta(hours=val[0],minutes=val[1])
        if duree!=None:
            return duree
    
    lemme=n.lemma[0]
    i=lemme.rfind(" minute")
    if i!=-1:
        val=deltaduree(lemme[:i])
        duree=datetime.timedelta(minutes=int(val))

    i=lemme.rfind(" seconde")
    if i!=-1:
        val=deltaduree(lemme[:i])
        duree=datetime.timedelta(seconds=int(val))


    i=lemme.rfind(" heure")
    if i!=-1:
        val=deltaduree(lemme[:i])
        duree=datetime.timedelta(hours=int(val))
        
    return [duree,0]


def retire(f,s):
    try:
        f.remove(s)
        retire(f,s)
    except:
        pass
    
def listetraits(f):
    ongarde=["REUNION", "DEPART","ARRIVEE","SALLE","GEO","COURS","VOYAGE"]
    f=f[1:len(f)-1]
    f=f.replace(":+","")
    f=f.replace("!","")
    f=f.split(",")
    res=[]
    for e in f:
        if e in ongarde:
            res.append(e.lower())
    return res
    

def calcullieu(u,d):
    n=u.nodes[d.parameters[0]]
    return n.surface

def nomadjver(u,n,lesmots,listecles,suivant):
    
    if n.pos[0] in ["VERB","NOUN","ADJ"]:
        m=n.lemma[0].lower()
        if m not in lesmots:
            lesmots.append(m)
        for f in n.features:
            tr=listetraits(f)
            for t in tr:
                if t not in listecles:
                    listecles.append(t)
    
    if n.daughter!=-1:
        nxt=u.fils(n)
        nomadjver(u,nxt,lesmots,listecles,True)
    
    if suivant and n.next!=-1:
        nxt=u.frere(n)
        nomadjver(u,nxt,lesmots,listecles,True)
        
    
def comparelen(x,y):
    return len(y)-len(x)

def analyse(s,quest):
    #try:
        res=objects.XipResult()
        objects.parse(s,res)
        ladate=datetime.datetime.now()
        aujourdui=ladate
        heuredebut=None
        heurefin=None
        lieuorigine=""
        lieuarrivee=""
        sujets={}
        duree=datetime.timedelta(minutes=60)
        enregistrer=False
        question=quest
        avecdate=False
        avecheure=False
        avecjour=False
        datedebut=None
        datefin=None
        lesmots=[]
        listecles=[]
        deporigine=None
        deparrivee=None
        doubledate=0
        lesdurees=[]
        for i in res.units:
            for d in i.dependencies:
                if d.name=="QUESTION":
                    question=True              
                    continue      
                if d.name=="DATE":
                    avecdate=True
                    enregistrer=True
                    rdj=calculdate(i,d)
                    if rdj!=None:
                        ladate=rdj[0]
                        avecjour=rdj[1]
                        if "ORIGINE:+" in d.features:
                            datedebut=ladate
                            deporigine=d                       
                            doubledate+=1 
                        if "ARRIVEE:+" in d.features:
                            datefin=ladate
                            deparrivee=d
                            doubledate+=1
                    continue
                        
                if d.name=="HEURE":
                    avecdate=True
                    avecheure=True
                    enregistrer=True
                    hm=calculheure(i,d)     
                    if "DECALAGE:+" in d.features:
                        heuredebut=ladate
                        heuredebut+=datetime.timedelta(hours=hm[0],minutes=hm[1])  
                        heuredebut=[heuredebut.hour,heuredebut.minute]
                    else:
                        if "ARRIVEE:+" in d.features:
                            heurefin=hm
                        else:
                            heuredebut=hm
                    continue
                if d.name=="DUREE":
                    lesdurees.append(d)
                    continue
                if d.name=="LIEU":
                    enregistrer=True
                    if "ARRIVEE:+" in d.features:
                        if lieuarrivee!="":
                            lieuarrivee+=","
                        lieuarrivee+=calcullieu(i,d)
                    else:
                        if lieuorigine!="":
                            lieuorigine+=","
                        lieuorigine+=calcullieu(i,d)
                    continue
                if d.name=="SUJET":
                    n=i.nodes[d.parameters[0]]
                    sujets[int(n.leftoffset)]=calcullieu(i,d)
                    nomadjver(i,n,lesmots,listecles,False)
        
        for d in lesdurees:
            enregistrer=True
            if avecdate:
                resduree=calculduree(i,d,ladate)
                duree=resduree[0]
                if resduree[1]!=0:
                    ladate=aujourdui+duree
                    #On va alors du lundi au vendredi
                    duree=datetime.timedelta(days=4)
            else:
                resduree=calculduree(i,d,None)
                avecheure=True
                duree=resduree[0]
                


        if not enregistrer and not question:
            app.erreur()
            return
        
        #Verification du mois: du 15 au 20 mars
        if deporigine!=None and deparrivee!=None:
            if "MOIS:+" in deparrivee.features and "MOIS:+" not in deporigine.features:
                #On recopie le mois de datefin dans datedebut
                datedebut=datetime.datetime(datedebut.year,datefin.month,datedebut.day)
                
        sujet=""
        listesujet=sujets.keys()
        listesujet.sort()
        if datedebut==None:
            datedebut=ladate
        for su in listesujet:
            sujet+=" "+sujets[su]
            
        if heuredebut!=None:
            datedebut=datetime.datetime(datedebut.year,datedebut.month,datedebut.day,heuredebut[0],heuredebut[1])
            if datefin==None:
                datefin=datedebut+duree
        
        if heurefin!=None:
            datefin=datetime.datetime(datefin.year,datefin.month,datefin.day,heurefin[0],heurefin[1])
        
        if not avecheure:
            #par defaut on prend l'heure du matin
            heuredebut=lesheures["matin"]
            datedebut=datetime.datetime(datedebut.year,datedebut.month,datedebut.day,heuredebut[0],heuredebut[1])
            if datefin==None:
                datefin=datedebut+duree
            else:
                heurefin=lesheures["soir"]
                datefin=datetime.datetime(datefin.year,datefin.month,datefin.day,heurefin[0],heurefin[1])
            
        print "DATE=",datedebut,datefin
        lieu=""
        if lieuorigine!="" and lieuarrivee!="":
            lieu="from "+lieuorigine+" to "+lieuarrivee
        else:
            if lieuorigine!="":
                lieu=lieuorigine
            if lieuarrivee!="":
                if lieu!="":
                    lieu+=", "
                lieu+=lieuarrivee
        #On construit alors un corps qui a une certaine allure
        #On rajoute nos traits sur venant de nos noeuds
        lecorps=s+" # "
        laquestion=""
        for l in lesmots:
            if l not in laquestion:
                laquestion+=l+" "
        for l in listecles:
            if l not in laquestion:
                laquestion+=l+" "
        laquestion=laquestion.strip()
        lecorps+=laquestion
        
        print "CORPS=",lecorps
        if not question:
            if app.acceptation(datedebut,datefin,lieu.strip(),sujet.strip(),lecorps)!=0:
                print "REFUS"
                return                   
            cree(datedebut,datefin,lieu.strip(),sujet.strip(),lecorps)
        else:
            #c'est une question
            app.majcalendar()
            lesmots.sort(comparelen)
            #on fabrique notre question, uniquement avec les mots qui vont bien
            rep=questionnement(datedebut,datefin,lieu.strip(),sujet.strip(),laquestion,avecdate,doubledate,avecjour,avecheure)
            for ef in rep:
                a=ef[0]
                x=a.Start
                deb=datetime.datetime(x.year,x.month,x.day,x.hour,x.minute)
                x=a.End
                fin=datetime.datetime(x.year,x.month,x.day,x.hour,x.minute)
                if app.afficher(deb,fin,a.Location,a.Subject,ef[1])==1:
                    break
    #except:
    #    app.erreur()
        

import Tkinter
import Dialog

class App:

    def __init__(self, master):

        self.maitre=master
        self.frame = Tkinter.Frame(master)
        self.frame.pack()
        self.loading=False

        self.hi_there = Tkinter.Button(self.frame, text="Store", default=Tkinter.ACTIVE, command=self.analysis)
        self.hi_there.pack(side=Tkinter.LEFT)

        self.quest = Tkinter.Button(self.frame, text="Question", fg="red", command=self.question)
        self.quest.pack(side=Tkinter.LEFT)

        self.quest = Tkinter.Button(self.frame, text="Reload", fg="green", command=self.recharger)
        self.quest.pack(side=Tkinter.LEFT)


        self.button = Tkinter.Button(self.frame, text="Quit", fg="red", command=self.frame.quit)
        self.button.pack(side=Tkinter.LEFT)

        police = ('Tahoma', 10, 'normal')
        self.text = Tkinter.Text(master, width=80, height=20, font=police,background='white',spacing1=1, spacing2=1, tabs='24')
        self.text.pack(side=Tkinter.TOP, expand=Tkinter.YES, fill=Tkinter.BOTH)

    def erreur(self):
        d=Dialog.Dialog(self.maitre,title="Erreur...",text="Votre phrase ne comprend aucune information temporelle",strings=["Quitter"],bitmap="",default=0)

    def afficher(self,datedebut,datefin,lieu,sujet,force):
        chaine="Importance:"+str(force)+"\n\n"
        chaine+="Subject:\t"+sujet+"\n"
        chaine+="------------------------------\n"
        print "In:",str(datedebut)
        chaine+="Start:\t"+datedebut.strftime("%d/%m/%Y %H:%M")+"\n"
        chaine+="End:\t"+datefin.strftime("%d/%m/%Y %H:%M")+"\n"
        chaine+="------------------------------\n"
        chaine+="Location:\t"+lieu+"\n"
        d=Dialog.Dialog(self.maitre,title="Calendar Input",text=chaine,strings=["Next","    Ok  "],bitmap="",default=0)
        return d.num


    def acceptation(self,datedebut,datefin,lieu,sujet,corps):
        chaine="Subject:\t"+sujet+"\n"
        chaine+="------------------------------\n"
        chaine+="Start:\t"+datedebut.strftime("%d/%m/%Y %H:%M")+"\n"
        chaine+="End:\t"+datefin.strftime("%d/%m/%Y %H:%M")+"\n"
        chaine+="------------------------------\n"
        chaine+="Location:\t"+lieu+"\n"
        chaine+="Keywords:"+corps[corps.find('#')+2:]+"\n"
        d=Dialog.Dialog(self.maitre,title="Calendar Input",text=chaine,strings=["Accept","Refuse"],bitmap="",default=0)
        return d.num
    
    def majcalendar(self):
        if self.loading:
            return
        self.loading=True
        aujourd=datetime.datetime(2007,1,1)
        unan=aujourd+datetime.timedelta(365)
        debut=aujourd.strftime("%d/%m/%Y %H:%M")
        fin=unan.strftime("%d/%m/%Y %H:%M")        
        chargeroutlook(debut,fin,unan)

    def recharger(self):
        self.loading=False
        self.majcalendar()
        
    def question(self):
        s=self.text.get(1.0,Tkinter.END)
        analyse(s,True)

    def analysis(self):
        s=self.text.get(1.0,Tkinter.END)
        analyse(s,False)


init()



#analyse("Meeting avec Pierre, mardi ?")
#analyse("Reunion mardi dernier.")
#analyse("Réunion en Mont Blanc dans une heure")
#analyse("Voir Christine demain à 10h00 pour le voyage à Turin")
#analyse("Voyage à Turin pour décrire le fonctionnement de GoldMiner, demain.")
#analyse("Réunion avec Frédérique sur la présentation de GoldMiner, à Turin, lundi à 9h00, aujourd'hui à 14h00, dans son bureau.")
#analyse("Présentation de GoldMiner, lundi à Turin de 11h00 à 12h00")
#analyse("Meeting de 15 à 16h00, lundi")
root=Tkinter.Tk()
root.title("Agenda")


app=App(root)

#analyse("rendez-vous la semaine du 25",False)
#analyse("repas en mai",T)

#app.recharger()

root.mainloop()
    
   







    
    
