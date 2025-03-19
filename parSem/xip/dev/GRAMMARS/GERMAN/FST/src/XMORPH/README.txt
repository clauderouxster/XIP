---------------------------------------------------------------------------
SOURCE FILES for German morphological analyzer
---------------------------------------------------------------------------
[] db-german.txt	= lexicon source (textual database)

All words (nouns, proper names, acronyms, adjectives, verbs, ...)
are stored (in alphabetical order) with their base form and a set of
lexc continuation classes.
Furthermore it contains additional information and comments which may
serve for future or classifications.

Some special word classes (numerals, irregular function words) are 
kept in separate files (see below).

==> more details in _DOC/README.db-german.txt

---------------------------------------------------------------------------
[] db2lexc.gawk		= gawk script for lexicon conversion

gawk script to textual database into lexc format

---------------------------------------------------------------------------
[] remove-derivs.gawk	= gawk script to removal derivations of lexicon

The script removes (most) derivational continuation classes from the
source lexicon in order to compile a purely "inflectional" analyzer.
Instead of the derivational analysis, an auxiliary lexicon file
(produced by 'get-derivs.xfst', see below) with lexicalised derivations
will be added.

eg. Anwendbarkeiten =>
   basic analyzer:	 anwenden+Verb}bar+Adj+Pos}keit+Noun+Fem+Pl+...
   infl. analyzer:	 Anwendbarkeit+Noun+Fem+Pl+NomAccGenDat

---------------------------------------------------------------------------
[] header.lexc		= lexc definitions and inital lexicons

This file contains all multi-character symbols (with a basic description
of the tags which are used in the analyzer), and the Root lexicons

---------------------------------------------------------------------------
[] flex-noun.lexc	= lexc lexicons for noun and proper name inflection

---------------------------------------------------------------------------
[] flex-verb.lexc	= lexc lexicons for verb inflection

This file includes sub-lexicons for all irregular verbs
---------------------------------------------------------------------------
[] flex-adj.lexc	= lexc lexicons for adjective and adverb inflection

The file includes sub-lexicons for irregular adjectives


---------------------------------------------------------------------------
[] numerals.lexc	= lexc lexicons for numerals

The file covers written numerals (e.g. "siebenundsiebzig") with their
derivations, and digital derivatives ("13teilig", "90-er")

---------------------------------------------------------------------------
[] cont-deriv.lexc	= lexc lexicons for derivations

---------------------------------------------------------------------------
[] lex-prondet.xfst		= sub-lexicon of pronouns and determiners
[] lex-conj.xfst		= sub-lexicon of conjunctions
[] lex-prep.xfst		= sub-lexicon of prepositions

---------------------------------------------------------------------------
[] rules-german.xfst	= (x)fst file with phonologiacl/orthographic rules

This file defines all steps from the initial lexc-compiled lexicon FST
to the final morphological analyzer FST.
The rules cover 
    - verb control		(e.g. no imperative with prefix: *an=komm!)
    - phonological rules	(e.g. arm +$er -> ärmer)
    - spelling rules		(e.g. Fluss -> Fluss^New, Fluß^Old
    - optional e-Elision	(e.g. Auge -> Aug')

---------------------------------------------------------------------------
[] special.xfst		= (x)fst file for special tokens

The file contains regular expressions to define punctuations (:, ?, !),
special symbols (#, $), digital expressions (1+2*12,3), single letters
(A, B, C), initials (A., J.S.), physical units (km/h, sec), etc.

---------------------------------------------------------------------------
[] get-derivs.xfst	= (x)fst file for derivation extraction

The script extracts derived word forms from basic analyzer and produces
lexc entries for further compilation of a "inflectional" analyzer which
has derived word forms as lemmas.

NOTE:	This script does not extract ALL derivations for lexicalisation
	as this would blow up the lexicon considerably. 
	e.g. generalized derivations (e.g. negation and/or nominalization
	     of participles) are exluded and will still be analyzed
	     as derivations
	Ungesagtheit -> un^{sagen+Verb+PPast^}heit+Noun+Fem+Sg+NomAccDatGen

==> More details about derivation in _DOC/README.derivs.txt


---------------------------------------------------------------------------
[] subst-prefix.xfst	= (x)fst script for general compound prefixation

The basic analyzer FST contains a symbol "*PREF*" as potential left-hand
part of a hyphenated compound. The script "subst-prefix" replaces this
symbol by any sequence of characters to enable the analysis of compounds
with unknown (i.e. not lexicalised) parts before a hyphen.
The "unkwnown" part is marked by surrounding symbols "[*" and "*]".
e.g.: XYZ-Zeitung :: [*XYZ*]^-Zeitung+Noun+Fem+Sg+NomAccDatGen
