------------------------------------------------------------------------

CONTENT:	Description of db-german.txt
AUTHOR:		Anne Schiller, XRCE
DATE:		30-Jul-2003

------------------------------------------------------------------------

The lexical database "db-german.txt" is a simple text format where each
line consists of 4 columns separated by "tabs".

Column 1 (optionally) contains a spelling marker
Column 2 contains the lexical entrye which generally consists of 
	 a base form and one ore several tags and may contain special
	 characters which serve for morphophonological operations
Column 3 contains one or several lexical classes which correspond to
	 "continutaion classes" (or LEXICONs) in lexc
Column 4 (optionally) contains comments, examples, spelling mappings,
	 derivational info, ... which may be usefule, but are NOT used
	 for now in the morphological analyzer.

The details are described below.
------------------------------------------------------------------------

Note: examples below refer to 
   (L) == corresponding entry in lexc source file 
   (A) == analyzer example <SurfaceWord> :: <LexicalString>

========================================================================
COLUMN 1 : spelling markers
------------------------------------------------------------------------

[new]  = new spelling	
[old]  = old spelling
[err]  = erroneous, but frequent spelling

These markers will be converted into "flag diacritics" in the FST
and will show up as "^New", "^Old", "^Err" in the output.

NOTE:	 spelling changes from ß to ss (e.g. Fluß^Old, Fluss^New)
	 will be covered by phonological rules, only the new
	 spelling "ss" is entered into the lexicon.
	 Words which do NOT change to ß (according the old rules 
	 at the end of a word or before a consonant) must be
	 marked as special (see below)

========================================================================
COLUMN 2 : lexical entries
------------------------------------------------------------------------

Lexical entries a generally character strings representing base forms,
but they may contain some "control characters" which are necessary
to ensure the coorect application of rules.

    --------------------------------------------------------------------
(1) VERB CONTROL
    Verbs require a special set of control symbols in order to guarantee
    the right combinations of prefixes, infixes and suffixes:

[PZG]  == [P][Z][G]
[PZ]   == [P][Z]
[ZG]   == [Z][G]

[P]	marker for separable verb prefix
	It is converted into a flag diacritic and a symbol "^="
	in the output. This marker controls V1 forms and imperative
	which must NOT have the prefix attached.
	For these (unprefixed) forms, a second entry will be
	generated in the lexc source file without the prefix, but still 
	marked "^=" to indicate the missing prefix.
[Z]	place holder for possible "zu" infix for infinitives
[G]	place holder for possible "ge" prefix for past participles 
	e.g. ein[PZG]kaufen+Verb
	 (A) einzukaufen :: ein^=kaufen+Verb+Izu
	 (A) eingekauft :: ein^=kaufen+Verb+PPart
	 (A) einkaufst :: ein^=kaufen+Verb+Verb+Indc+2P+Sg+Pres+V2
	 (A) kaufst :: ^=kaufen+Verb+Verb+Indc+2P+Sg+Pres+V1
	 (A) kauf :: ^=kaufen+Verb+Imp+2P+Sg
	 (A) einkauf ## FAIL

    --------------------------------------------------------------------
(2) COMPOUND CONTROL
    Compounding is mainly controlled through continuation classes and
    flag diacritics associated to them. By default any noun, adjective,
    verb stem, numeral and explicit compound prefixes can occur on the left
    hand of a compound, and only nouns and adjectives are allowed to be
    compound heads. 
    

[-]	compound boundary at the beginning of a word
	- mainly used for first names (to restrict compoistion)
	- indicates that a (productive) compound must have a hyphen
	
[^]	compound boundary at the beginning of a word
	- mainly used for acronyms
	Like [-] but additionally, the initial character remaines 
	unchanged for noun or adjective head
	e.g. [^]US+Prop+Country+Masc
	 (A) US-freundlich ::US+Prop+Country+Masc+Pl^-freundlich+Adj+Pos+Pred
	BUT: Kind+Noun+Neut
	 (A) kinder-freundlich :: Kind+Noun+Neut+Pl^-freundlich+Adj+Pos+Pred

[=]	hyphen within a word
	- for compound proper names which change case in derivations
	e.g. Rheinland[=]Pfalz+Prop+State+Neut
	 (A) rheinland-pfälzisch :: Rheinland-Pfalz+Prop+State+Neut^}isch+Adj+Pos+Pred

[&]	mandatory compound boundary at the beginning of a word
	- for "derivational"compound head words
	Some (mostly derivational) head words require an initial part.
	e.g. [&]äugig+Adj
	(A) blauäugig :: blau+Adj+Pos^#äugig+Adj+Pos+Pred
	(A) äugig :: FAIL

        NOTE: The "correct" decomposition would probably be 
		[ Word1 # Word2 ] + Affix
	      but this would add a lot to the complexity of the
	      (finite-state!) analyser. 
	      The current system adopts the decomposition
	      	      Word1 # [ Word2 + Affix ]
	      where [Cat2 + Affix ] must be lexicalized.

[*]	inhibit compounding
	- mainly for multi-words ("Hab und Gut") or expressions ("Ade")
	  which should not be part of a compound.
	e.g. [*]Ade+Noun+neut
	(A) Ade- ## FAIL
	(A) Scharade :: Scharade+Noun+Fem+Sg+NomAccDatGen
	NOT: Scharade :: Schar+Noun+Fem+Sg^#Ade+Noun+Neut...

    --------------------------------------------------------------------
(3) OPTIONAL CHARACTERS
    Some lemmas vary
^e	"e" that may be omitted on surface 
	- the "e" remains in the lemma
	e.g. múd^e
	 (A) müde :: müde+Adj+Pos
	 (A) müd :: müde+Adj+Pos

^E	 "e" that may be omitted on surface 
	 - the "e" is omitted in the lemma
	 e.g. Asiat^E
	  (A) Asiat :: Asiat+Noun...
	  (A) Asiate :: Asiat+Noun...

^a	 "a" that may be omitted on surface 
	- the "s" remains in the lemma
	e.g. d^arauf+Adv
	 (A) darauf :: darauf+Adv
	 (A) drauf :: darauf+Adv

    --------------------------------------------------------------------
(4) OTHER SYMBOLS:
\	optional negation, i.e. a prefix "un" may be added
	(The symbol is converted as flag diacritic)
	e.g. \unbekannt+Adj 
	 (A) bekannt :: bekannt+Adj+Pos+Pred
	 (A) unbekannt :: un^{bekannt+Adj+Pred

!	"cut" lexical string here for lexc entry
	Irregular words (especially verbs) are treated in special
	sublexicons. To facilitate the readability and the maintenance of
	the source lexicon, the full base forms are in the textual database
	but only the (prefix) part of the word which is not handled in the
	continuation class is used in the lexc source file.
	e.g. an[PZG]!kommen
	 (L) an[P][Z][G]    V.kommen ;
	 (A) ankommen :: an^=kommen+Verb+Inf
	 (A) ankam an^=kommen+Verb+Indc+1P3P+Sg+Past+V2

^|	segment boundary (may be used to make output more readable
	or in order to trigger or block phonological rule which apply
	(This symbol is part of the analysis output.)
	e.g. Miss^|erfolg 
	 (A) Mißerfolg :: Miß^|erfolg+Noun...^Old 
	 (A) Misserfolg :: Miss^|erfolg+Noun...^New
	     - without the ^| no conversion ss->ß would take place
	e.g. Aus^|strahlung ==>
	 (A) Aus^|strahlung :: Aus^|strahlung+Noun ...
	     NOT: Außtrahlung^Old
	     - without the ^| a conversion ss -> ß would apply !

^*	special symbol to to trigger or block phonological rule
	(similar to ^|, but does not appear in the output)
	e.g. Cross^*+Noun
	 (A) Cross :: Cross+Noun+...
	e.g. schwer^*+Adj ==>
	 (A) schwerer :: schwer+Adj+Comp
	     NOT: schwrer :: schwer+Adj+Comp
	     - without the ^* a rule e -> 0 before r e would apply
	       cf. sicher+er -> sichrer

^_	white space
	e.g. ad^_hoc+Adv ==> 
	 (A) ad hoc :: ad^_hoc+Adv

========================================================================
COLUMN 3 : lexical classes
------------------------------------------------------------------------
One ore several lexical classes must be specified for every entry
(multiple classes are sparated by "|")

A lexical class corresponds any of the "major" continuation lexicons 
defined in flex-noun.lexc, flex-adj.lexc, flex-verb.lexc, ...

In addition some characters can be removed from or added to the base
form before the continuation class is applied:

^1	remove last character from base form
^2	remove 2 last characters from base form
^3	remove 3 last characters from base form
[a-z]	add character to the base form
	e.g. Corpus+Noun+Neut   N_Pl:^2ora 
	 (L) Corpus+Noun+Neut^2ora	   N_Pl ;
	 (A) Corpora :: Corpus+Noun+Neut+Pl+NomAccDatGen
	e.g. Hilfe+Noun+Fem Cp_s:^1
	 (L) Hilfe+Noun+Fem^1s	  Cp_s ;
	 (A) Hilfs- :: Hilfe+Noun+Fem+Sg^/s+Cpd

========================================================================
COLUMN 4 : comments
------------------------------------------------------------------------
Comments are optional; multiple comments are separated by "|"

(1) Spelling:
@O	corresponding old spelling (for "[new]" entries)
@N	corresponding new spelling (for "[old]" entries)
@S	corresponding correct spelling (for "[err]" entries)

(2) reference:
[D	attested by "Duden: Die deutsche Rechtschreibung"
[F	attested by "Duden: Das Fremdwörterbuch"
[G	found with Google

(4) Derivation "root"
«	possible decomposition or derivation of the baseform

(3) other:
"	example
%	domain	(%Chem., %Med., %Hist., ...)
cc=	country code (ISO 3166)
