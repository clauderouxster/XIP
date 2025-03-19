//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//! Xerox Research Centre Europe - Grenoble Laboratory
//!
//! Copyright (c) 2003 Xerox Corporation.  All Rights Reserved.
//!
//! --- CONTENTS ---
//!   Project    : LINGVETA XIP grammar
//!   filename   : features.xip
//!   Date       : 10/02/2003
//!   Author     : Caroline Hagege
//!  
//!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Features:
 [features: 
  
  [
   cat:0,
   upper:+,allupper:+,
   noun:{+},
   verb:{+},  
   vp:+,
   np:+,
   adj:{+},
   prep:{+},
   adv:{+},
   det:{+},
   pron:{+},
   punct:{+},
   phrasal:+,
   

   poss:{+},   
   refl:{+},

   agreements:[
	antecedent:[
  	   antecedentcase: {nom, gen, acc,  dat },
	   antecedentgender: {fem, masc,neutral}
	 ],
	 semantics:[
	   loc:+,
	   temporal:+,
	   num:+,
	   proper:{+}],

	  case: {nom,dat,gen,acc},
	  gender: {neutral,fem,masc},
	  pers:{p1,p2,p3},
	  number:{sg,pl},
	  coord:+,neg:+
   ],
  antcase: [
     case:{},
     antecedentcase: {}
  ],
  verbagreement: [
   tense: {inf,fut,past,pres},
   coord:{},
   neg:{},
   rel:+
  ],
   def:+,conj:+,
   sent:+
  ]
]









