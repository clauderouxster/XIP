
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



def cree(datedepart,datearrivee,lieu,sujet,phrase):
    outlook = win32com.client.Dispatch("Outlook.Application")
    #namespace = outlook.GetNamespace("MAPI")
    #app=namespace.GetDefaultFolder(9).Items
    rdv= outlook.CreateItem(olAppointmentItem)    
    rdv.Subject=sujet
    rdv.Start=datedepart.strftime("%d/%m/%Y %H:%M")
    rdv.End=datearrivee.strftime("%d/%m/%Y %H:%M")
    rdv.Body=phrase
    rdv.Location=lieu
    rdv.Save()


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
            jourdiff+=increm*multi
     
    #On regarde ce qui peut manquer
    #Si l'on a le jour, le mois et l'annee, c'est bon
    if lejour!=-1 and lemois!=-1 and lannee!=-1:
       #On retourne une date
       ladate=datetime.datetime(lannee,lemois,lejour)
    else:
       if lejour!=-1 and lemois!=-1:
           ladate=datetime.datetime(auj.year,lemois,lejour)
       else:
           if lejour!=-1:
               ladate=datetime.datetime(auj.year,auj.month,lejour)
           else:
               if jourdiff!=None:
                   ladate=auj+datetime.timedelta(jourdiff)
    return ladate


def extraitheure(u,n,dfeat):
    lefils=u.fils(n)
    hm=None
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

def calculduree(u,d):
    n=u.nodes[d.parameters[0]]
    lefils=u.fils(n)
    duree=None
    
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
        
    return duree


def calcullieu(u,d):
    n=u.nodes[d.parameters[0]]
    return n.surface

def analyse(s):
    #try:
        res=objects.XipResult()
        objects.parse(s,res)
        ladate=datetime.datetime.now()
        heuredebut=None
        heurefin=None
        lieuorigine=""
        lieuarrivee=""
        sujets={}
        duree=datetime.timedelta(minutes=30)
        enregistrer=False
        for i in res.units:
            for d in i.dependencies:
                if d.name=="DATE":
                    enregistrer=True
                    ladate=calculdate(i,d)
                if d.name=="HEURE":
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
                if d.name=="DUREE":
                    enregistrer=True
                    duree=calculduree(i,d)
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
                if d.name=="SUJET":
                    n=i.nodes[d.parameters[0]]
                    sujets[int(n.leftoffset)]=calcullieu(i,d)
        
        if not enregistrer:
            app.erreur()
            return
        sujet=""
        listesujet=sujets.keys()
        listesujet.sort()
        for su in listesujet:
            sujet+=" "+sujets[su]
        datedebut=ladate
        datefin=ladate
        if heuredebut!=None:
            datedebut=datetime.datetime(ladate.year,ladate.month,ladate.day,heuredebut[0],heuredebut[1])
            datefin=datedebut+duree
            print datefin
        if heurefin!=None:
            datefin=datetime.datetime(datedebut.year,datedebut.month,datedebut.day,heurefin[0],heurefin[1])
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
        if app.acceptation(datedebut,datefin,lieu.strip(),sujet.strip())!=0:
            print "REFUS"
            return           
        cree(datedebut,datefin,lieu.strip(),sujet.strip(),s)
    #except:
    #    app.erreur()
        

import Tkinter
import Dialog

class App:

    def __init__(self, master):

        self.maitre=master
        self.frame = Tkinter.Frame(master)
        self.frame.pack()

        self.hi_there = Tkinter.Button(self.frame, text="Enregistrer", command=self.analysis)
        self.hi_there.pack(side=Tkinter.LEFT)

        self.button = Tkinter.Button(self.frame, text="Quitter", fg="red", command=self.frame.quit)
        self.button.pack(side=Tkinter.LEFT)

        police = ('Tahoma', 10, 'normal')
        self.text = Tkinter.Text(master, width=80, height=20, font=police,background='white',spacing1=1, spacing2=1, tabs='24')
        self.text.pack(side=Tkinter.TOP, expand=Tkinter.YES, fill=Tkinter.BOTH)

    def erreur(self):
        d=Dialog.Dialog(self.maitre,title="Erreur...",text="Votre phrase ne comprend aucune information temporelle",strings=["Quitter"],bitmap="",default=0)


    def acceptation(self,datedebut,datefin,lieu,sujet):
        chaine="Sujet:\t"+sujet+"\n"
        chaine+="------------------------------\n"
        chaine+="Début:\t"+datedebut.strftime("%d/%m/%Y %H:%M")+"\n"
        chaine+="Fin:\t"+datefin.strftime("%d/%m/%Y %H:%M")+"\n"
        chaine+="------------------------------\n"
        chaine+="Lieu:\t"+lieu+"\n"
        d=Dialog.Dialog(self.maitre,title="Entrée du calendrier",text=chaine,strings=["Accepter","Refuser"],bitmap="",default=0)
        return d.num
        
    def analysis(self):
        s=self.text.get(1.0,Tkinter.END)
        analyse(s)



init()
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
root.mainloop()
    
   







    
    
