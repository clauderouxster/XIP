************************************************
ANNOTATIONS DES EXPRESSIONS TEMPORELLES DANS XIP

Informations sur les fichiers concernés

Maud Ehrmann - Mai 2009

************************************************

------------------
A/ Point de départ 
------------------


règles écrites par Aude pour les EN.
Il y avait la reconnaissance de dates, dans le fichier localgram_dates.xip. 
Ce fichier existe toujours, il a été bcp modifié mais il reste des règles de Aude.


----------------------------------
B/ Ce qui est fait (théoriquement)
----------------------------------

(Large) reconnaissance et annotation des expressions temporelles selon différents types.

a) Différences avec l'article

Pour ce qui est de la définition et du typage des expressions temporelles, il existe quelques différences avec ce qui a été défini dans l'article (Ehrmann & Hagège, TALN 2009). Ces différences sont explicitées :

Définition :

- identification = ce qui est fait dans XIP respecte ce qui est dit dans l'article, modulo qlqs unités lexicales non prises en compte (par ex., non prise en compte de "tard, tôt" ou autres UL pour lesquelles il a été décidé tardivement de les prendre en compte)

- délimitation = ce qui est fait dans XIP respecte ce qui est dit dans l'article (théoriquement)

- segmentation = ce qui est fait dans xip ne permet pas de gérer correctement la segmentation des ET, telle qu'elle a été définie dans l'article. Il faut intervenir au niveau du chunking.

Typage :

Suite à des devs pour Infomagic et Ester, les grammaires ne respectent pas forcément le typage défini dans l'article.
Pour plus de précisions voir le descriptif des types employés ci-après.


b) Typage réalisé

Listing:

DATE
DATEPOT
DATEMOD
HEURE
HEUREPOT
HEUREMOD
DUREE
DUREEMOD
SET
INTERVAL-TEMP

****** TYPE DATE ******

Sous-types :

** DATE-absolue complète 

= pour les ET comprenant la mention du jour (le chiffre et/ou le nom du jour), du mois et de l'année, i.e. pour lesquelles l'ancrage est directement calculable à partir de l'expression.
Le trait "complete" est présent mais n'est pas affiché.

ex : le (lundi) 15 juin 2008


** DATE-absolue incomplète 

= pour les ET comprenant la mention de l'année, plus éventuellement le mois oui le jour

ex : en 1989, Juin 1989


** DATE-relative

= pour les ET ne comprenant pas de mention de l'année, i.e. pour lesquelles l'ancrage est non directement calculable à partir de l'expression.

Quelques marquage pour dates relative à l'énonciation (temp_ref=ST) et celles relatives à une référence textuelle (temp_ref=TT).

** DATEMOD

= pour les ET de type DATE dont la ref temporelle est modifiée/spécifiée

ex : Il y a 3 jours à peine.  15h environ. etc.

**DATEPOT = DATE potentielle	

Lorsque qu'on a un nombre à 4 chiffres et pas assez de contexte pour dire s'il s'agit d'une date ou pas 


** DUREE

Il y a un trait "heure" ou "date" attaché à la dépendance DUREE, pour spécifier s'il s'agit d'une durée en heure ou en élément supérieur aux heures.
Ce n'est pas très utile, cela peut-être enlevé.

** DUREEMOD

ex :il est parti un peu plus de 3 jours.

** SET

Ce type n'est pas très détaillé, cad pas de précision si agrégat de durée ou de date.

ex: tous les jeudis, chaque matin


** INTERVAL-TEMP

Type prévu pour InfoMagic.
Transformable en DATE.

ex : 1992-93, 

Avant cela couvrait également les expressions telles que "il part du 3 au 5 janvier, de jeudi à vendredi, entre mars et juin ", mais à présent ces dernières ont le type DATE.	

** HEURE

Type prévu pour InfoMagic et ESTER.
idem, transformable en DATE.

** HEUREMOD

ex : Il est trois heures à peine.

**HEUREPOT

Lorsque pas assez de contexte pour dire s'il s'agit d'une heure (date) ou d'une durée.



---------------------
C/ Comment c'est fait
---------------------

a) l'organisation des fichiers

Il existe en tout 6 fichiers sur le temps :

- localgram_dates.xip = traitement des dates "ultra classiques" de type "12 janvier 2005", "2005"
- localgram_heure.xip = traitement des heures --> annotation d'heures et de durées (en heures)
- localgral_temp.xip  = traitement des ET au sens large selon les types DATE, SET, DUREE, etc.
- marqueur_temp.xip   = intervient après le chunker classique ; fait remonter les traits sur les chunks
- chunker_temp.xip    = intervient après le chunker classique ; pour gérer les ET complexes, pas grand chose pour le moment.



b) les traits utilisés

Sont dans le fichier traits.xip, à la fin (section expressions temporelles)


Il y a d'anciens traits définis par Aude, il faut voir les documents dans parSem/xip/dev/GRAMMARS/FRENCH/DOC/Entites_nommees.


Description des traits selon l'ordre dans lequel ils apparaissent dans le fichier traits.xip :

-- fstmod = pour certains éléments sortant des FST qui sont découpés (via règles de fragmentation) puis reconstruits
-- todelete = pour la procédure qui efface les doublons de dépendances (des deps identiques à différents niveaux de l'arbre)

--pointage = pour les éléments lexicaux permettant de pointer un moment ; ces éléments font que l'ET est ancrable
ex : "dans trois jours", 'dans' pointe qlqch.
rq : trait pas encore très utilisé, peut être utile dans de futurs devs ?

-- postdate = pour les éléments lexicaux pouvant se rencontrer après une date

-- traits pour les heures : 

- heure = permet de marquer l'ET lorsque reconnue comme une heure et permet les éléments lexicaux correspondant à des divisions du temps propres aux heures (seconde, minute, etc.)
- heurepot = chiffre de 0 à 24 (attention, c'est aussi le nom de la dépendance pour les "heures potentielles")
- heureambig = heure ambigue entre heure(date) et durée
- numHeure,numMInute,numSeconde = pour marquer les différents éléments des heures pour la normalisation
- heuremod = poure marquer une ET reconnue comme "modifiée" (Il est environ trois heures) 
- heure_extens = pour "heure extension" ; tout ce qui peut être présent à la suite d'une expression d'heure (pile, local, d'affilée)


-- traits pour les dates: 

- datepot = dates potentielles : 4 chiffres seuls sans contexte
- shortyear =  dates à deux chiffres
- datemod = pour marquer les ET reconnues comme date modifiées (début 2008 ,fin 2008, dans plus de 3 ans ,etc)
- anneemod =  pour marquer les elements lexicaux modifiant le mot "année", ex : "année civile, universitaire, scolaire"
- nondate = pour marquer ce qui ne peut pas être une date = un nombre à 5 chiffres, un nom ne pouvant suivre une date ("la creation de 1600 emplois")
- datecoord =  pour les dates coordonnées (from Infomagic)
- date_event = date événementielle (Fêtes des mères, Journée mondiale de la femme), permet de construire la dépendance "EVENEMENT-TEMP"


-- traits pour les jours et les mois  

- numJour:{+,01,02,03,04,05,06,07,08,09,10-31} = pour la normalisation, pour dire le numéro du jour dans le mois. ex : "le 16 janvier 2008", "16" porte le trait numJour et c'est son lemme (16) qui est récupéré pour insérer dans la date normalisée (16-01-2008)
- numjourpot = pour marquer les éléments lexicaux pouvant potentiellement être de numéros de jour (1 à 31)
- jour:{+,1-7} = pour marquer le noms de jour suivant leur position dans la semaine (lundi=1, mardi=2,etc.)
- numMois:{+} = pour marquer les chiffres étant des numéros de mois (12/05/09 --> 05 pour le trait numMois)
- mois:{+,01,02,03,04,05,06,07,08,09,10-12} = pour marquer le noms de mois suivant leur position dans l'année (janvier=01, etc.)

-- Moments divers:

-daytime:{+,mo,mi,af,ev,ni,dt} =  moment du jour, avec pour valeur soit le matin (MO), le milieu du jour (MI, poir Mid-day), l'après-midi (AF), le soir (EV), la nuit (NI) ou  "temps du jour" (daytime = DT). Inspiré de TIDES, page 17, section 3.3.6.
-saison:{+,sp,su,fa,wi} = pour marquer les saison + leur valeur ; à utiliser pour la normalisation
-annee:{+}, = pour marquer les éléments lexicaux reconnus comme année dans une ET (ex : le 16 juin 2008, "2008" porte le trait année
-fete:{+} = pour fête des mères, fête du travailn fête de la musique, etc.
-trimestre:{Q1,Q2,Q3,Q4} =  pour marquer les éléments lexicaux reconnus comme trimestre, avec leur valeur


-- Modifieurs:

- timemod:{+}, = pour marquer les éléments lexicaux modifiant des ET "dernier jour, "lundi matin" etc.; trait non utilisé dans les règles
- orig:{+} = pour marquer les preps marquant une origine temporelle (borne gauche) : depuis, à partir de,etc. Trait utilisés pour marquer les chunks et les dependances également.
- tfin:{+} = idem mais lorsqu'il s'agit d'un poit final ('jusqu'à, jusque')

- t_mod:{+,less_than,more_than,equal_or_less,equal_or_more,start,mid,end,approx} = trait utilisé pour marquer les éléments lexicaux (générallement adv, prep, et parfois adj) pouvant modifier une ET. Le trait prend un valeur en fonction de la sémantique du mot. ex : "début" porte le trait "t_mod:start", "fin" porte le trait "t_mod:end" . Inspiré de Tides, section 3.4 p 20.


-- Pour les génériques
generic:{+} = pour marquer les Et reconnues comme génériques (l'hiver est rude)
genericsignal:{+} = pour marquer les éléments lexicaux introduisant des ET génériques (prime de Noël)
nogeneric:{+} = pour marquer ce qui n'est pas générique


-- Durees et autres:
- interval:{+} = pour marquer les ET et les PP correspondant, reconnus comme des intervals
- in-interval:{+} = pour marquer dates dans intervalles, si on veut les faire sortir ou non.
- set:{+} = pour marquer les éléments reconnus comme agrégats
- setsignal:{+} = pour marquer les éléments lexicaux signalant un set (tous les, chaque)
- setmod:{+} = idem mais version modifiée (environ tous les quinze jours)
- duree:{+,debut,fin} = pour marquer les ET reconnues comme durées (valeur du trait rarement utilisée)
- dureemod:{+} = idem mais version modifiée 
- numDuree:{+} = pour marquer l'élément permettant de quantifier la durée ("3 jours" : 3 porte le trait numDuree) ; pour la normalisation
- timeduree:{+}, = trait rarement utilisé, à voir si supprimable. !!!!!!! MAUD = A SUPPRIMER LA SEMAINE PROCHAINE
- tempexpr:{+}, = pour marquer TOUT ce qui est ET
- duree_extens:{+} = extension de durée ex: '3 heures de suite', 'de suite' porte le trait duree_extens

-- Valeurs: NB choses inspirées de Tides
- val_other:{we,e,c,l},
- val_ref:{present,future,past},
- val_duree:{S,MIN,H,D,WE,W,M,Y},  = second, minute, hour, day, week-end,week, month, year
- temp_ref:{ST,TT}, //SpeechTime = when the expression is relative to the time of speech - TopicTime = relative to the narrative time.


//----Pour marquer la simultanéité :

simult:{+},
datedebut:{+},
datefin:{+},


-- Traits de deps:

absolue:{+},
relative:{+},
complete:{+},
incomplete:{+},

--Pour les verbes:
vtemp_dur:{+} = pour marquer les verbes dont le sens implique que l'ET les modifiant soit nécessairement de type DUREE


--Traits de chunks !!!!!! MAUD = A ENELEVER CAR NON UTILISES
c_relat:{+},
c_absol:{+},
c_dur:{+},
c_approx:{+},


--Noeuds "deduits":  !!!!MAUD : peut-être supprimer tout ça

pour gérer les expressions de type "les 3 et 5 juin" ---> restituer "3 juin". 
nounfict:{+},
datededuced:{+}, //pour dates recomposées à partir d'une ellipse
intervaldeduced:{+},
depdeduced:{+},



 --for ESTER output  !!!! MAUD : à enlever
ester:{+},
notester:{+},
a_modifier:{+},
specialester:{+},

 --for INFOMAGIC output !!!! MAUD : à enlever
infomagic:{+},

//-----div  !!!! MAUD : à enlever ?
toto:{+},
tutu:{+},
toremove:{+},
inNbre:{+},
tosegment:{+},



presentatif:{+} =  pour marquer les éléments suivants : il y a, voici, voilà, cela fait












-------------------------
D/ CE QU'IL RESTE A FAIRE
-------------------------








