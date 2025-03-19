

lesindexes={}
vecteurs={}
idx=0
lesphrases=0
comptes={}
nbphrase=0
lesextensions={}
lextension=0


def lire(nom):
	global idx
	global lesindexes
	global comptes
	global vecteurs
	global lesphrases
	global nbphrase
	global lesextensions
	global lextension
	f=open(nom,"r")
	for i in f:
		j=i.split(" ")
		ph=[]
		nbphrase+=1
		for p in j:
			try:								
				if p[-1] in ",;:.\"":
					ph.append(p[:-1])
					ph.append(p[-1])
				else:
					ph.append(p)
			except:
				pass
		#on traite alors la phrase
		nbph=1<<idx		
		lesphrases|=nbph
		idx+=1
		for m in ph:
			p=idx
			vale=0
			for lg in range(5):
				pl=m[len(m)-lg-1:]
				try:
					lesextensions[pl]+=1
				except:
					lesextensions[pl]=1
					idx+=1
				try:
					vp=lesindexes[pl]
				except:
					lesindexes[pl]=idx
					vp=idx
					idx+=1
				vale|=1<<vp
				lextension|=vale
			try:
				p=lesindexes[m]
				comptes[m]+=1
			except:
				lesindexes[m]=idx
				comptes[m]=1
				idx+=1
			v=1<<p
			v|=nbph
			v|=vale
			try:
				vecteurs[m]|=v
			except:
				vecteurs[m]=v
		#On recopie les bits de la droite vers la gauche
		n=0
		for i in range(len(ph)-1):
			mc=ph[i]
			m=ph[i+1]
			n=1<<lesindexes[mc]		
			vecteurs[m]|=n


for i in range(500):
	n="C:\\XIP\\patron_reuters\\concat\\news-%(#)00005d.txt" % {"#": i}
	print n
	lire(n)

ithe=1<<lesindexes["the"]
for v in vecteurs:
	if (ithe & vecteurs[v])!=0:
		if v!="the":
			imot=1<<lesindexes[v]
			for vv in vecteurs:
				if (imot & vecteurs[vv]) !=0:
					if v!=vv:
						print v,vv
	
	


