# -*- coding: utf-8 -*-
f=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperento\esperanto_deutsch.txt","r")
ecrire=open(r"C:\ANL\Mekalinga\ANL\Mekalinga\simple\esperento\allemand.txt","w")


remplace={"jx":"ĵ", "cx":"ĉ", "gx":"ĝ", "hx":"ĥ", "sx":"ŝ", "ux":"ŭ",'Hx':'Ĥ' ,'Sx':'Ŝ' ,'Jx':'Ĵ' ,'Ux':'Ŭ' ,'Gx':'Ĝ' ,'Cx':'Ĉ'}
lettres=['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']

for i in f:
    mot=i.strip().split(";")
    esperento=mot[0]
    allemand=mot[1]
    domaine=""
    if "~" in esperento:
        continue
    if "." in esperento:
        continue
    if esperento.strip() in lettres:
        continue
    
    while "[" in allemand:
        ps=allemand.find("[")
        psf=ps
        for a in allemand[ps+1:]:
            psf+=1
            if a=="]":
                break
        if domaine!="":
            domaine+=","
        domaine+=allemand[ps:psf+1]
        allemand=allemand[:ps]+allemand[psf+1:]
    while "(" in allemand:
        ps=allemand.find("(")
        psf=ps
        for a in allemand[ps+1:]:
            psf+=1
            if a==")":
                break        
        allemand=allemand[:ps]+allemand[psf+1:]

    for r in remplace:
        esperento=esperento.replace(r,remplace[r])
    esperento=unicode(esperento,"utf8").encode("utf8")        
    allemand=unicode(allemand,"utf8").encode("utf8")
    esperento=esperento.strip()
    allemand=allemand.strip()
    domaine=domaine.strip()
    nbblanc=50-len(unicode(esperento,"utf8"))
    blanc="".zfill(nbblanc).replace("0"," ")
    if domaine=="":
        ecrire.write(esperento+blanc+allemand+"\n")
    else:
        ecrire.write(esperento+blanc+allemand+","+domaine+"\n")
ecrire.close()
f.close()

        
    
