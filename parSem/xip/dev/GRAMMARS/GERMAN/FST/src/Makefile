# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	mappings for XIP added to lookup output
# AUTHOR:	Anne Schiller
# DATE:		27-Jul-2005
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------
# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e

GAWK	= gawk
CAT	= cat

# directories
AUX = ../aux
DATA = ../data

# morphology sources
SRC	= ./XMORPH

TARGET = 	$(DATA)/tok.fst \
		$(DATA)/lookup_v2.txt \
		$(DATA)/lookup_v2_abs.txt \
		$(DATA)/lexicon.fst \
		$(DATA)/guesser.fst \
		$(DATA)/html.fst \
		$(DATA)/open4xip.fst \
		$(DATA)/mark2-lex.fst \
		$(DATA)/mark2-guess.fst \
		$(DATA)/mark2-open.fst \
		$(DATA)/norm-std.fst \
		$(DATA)/norm-allcap-std.fst \
		$(DATA)/norm-umlaut.fst \
		$(DATA)/map_morph.fst \
		$(DATA)/map_feat.fst \
		$(DATA)/filter_morph.fst \
		$(DATA)/rank_compounds.fst \
		$(DATA)/rank_morph.fst 


# ==========================================================================
# TARGETS
default:	$(TARGET)

clean:	
		rm -f $(TARGET)
		rm -f $(AUX)/*.*



# ==========================================================================
# tokenizer transducer 
# ==========================================================================
$(DATA)/tok.fst:		tok-abbr.xfst \
				tok-basic.xfst \
				tok-mwe.xfst \
				tok-stapler.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)  -l tok-abbr.xfst \
		 -l tok-basic.xfst \
		 -l tok-mwe.xfst \
		 -l tok-stapler.xfst \
		 -e "optimize net" \
		 -e "save stack $@" -stop


$(DATA)/tok1.fst:		tok-abbr.xfst \
				tok-basic.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)  -l tok-abbr.xfst \
		 -l tok-basic.xfst \
		 -e "optimize net" \
		 -e "save stack $@" -stop


# ==========================================================================
# lookup scripts
$(DATA)/lookup%.txt:		lookup%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $< $@


# lookup script with absolute path
FSTDIR = $(PWD:/src=/data)
$(DATA)/lookup%_abs.txt:	lookup%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@




$(DATA)/map_morph.fst:	$(AUX)/multi-chars.fst \
			map_morph.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/multi-chars.fst \
		-l map_morph.xfst \
		-e "save stack $@" -stop

# source markers (added at end of output)
$(DATA)/mark2-%.fst: 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-e "read regex %+%#$* <- [..] || _ .#. ;" \
	     	-e "save stack $@" -stop


# other FSTs

$(DATA)/%.fst: 	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-l $< \
	     	-e "save stack $@" -stop





## -------------------------------------------------------------------------
## intermediate files
## -------------------------------------------------------------------------

# extract multi-character symbols

$(AUX)/multi-chars.fst:	$(DATA)/lexicon.fst \
			$(DATA)/guesser.fst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/lexicon.fst -e "unopt net"\
		-e "single char sigma net" \
		-s $(DATA)/guesser.fst \
		-e "single char sigma net" \
		-e "union net" \
		-e "save stack $@" -stop



# ===================================================================
# main lexicon
# - german-A.fst:	basic analyzer (with derivation analysis)
# - german-B.fst:	analyzer with lexicalised derivations
# ===================================================================
$(DATA)/lexicon.fst:	$(AUX)/german-B.fst \
			$(SRC)/cleanup-eps.xfst \
			$(SRC)/subst-prefix.xfst \
			$(AUX)/special.fst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/german-B.fst \
		 -l $(SRC)/cleanup-eps.xfst \
		 -l $(SRC)/subst-prefix.xfst \
		 -s $(AUX)/special.fst \
		 -e "union net" \
		 -e "optimize net" \
		 -e "save $@" -stop



# ============================================================================
# Conversion of db-german.txt into lexc format (for basic analyzer)
# ============================================================================
.PRECIOUS:	$(AUX)/german.lexc
$(AUX)/german.lexc:	$(SRC)/db-german.txt \
			$(SRC)/db2lexc.gawk
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(ECHO) "LEXICON VerbStems" > $@
	@$(CAT) $(SRC)/db-german.txt |\
	$(GAWK) -F "[	]+" '{if ($$3~/V[^S]/) print}' |\
	$(GAWK) -f $(SRC)/db2lexc.gawk >> $@
	@$(ECHO) "LEXICON VerbPrefix1" >> $@
	@$(CAT) $(SRC)/db-german.txt |\
	$(GAWK) -F "[	]+" '{if ($$3~/VSep[01A]/) print}' |\
	$(GAWK) -f $(SRC)/db2lexc.gawk >> $@
	@$(ECHO) "LEXICON VerbPrefix2" >> $@
	@$(CAT) $(SRC)/db-german.txt |\
	$(GAWK) -F "[	]+" '{if ($$3~/VSep2/) print}' |\
	$(GAWK) -f $(SRC)/db2lexc.gawk >> $@
	@$(ECHO) "LEXICON SingleStems" >> $@
	@$(CAT) $(SRC)/db-german.txt |\
	$(GAWK) -F "[	]+" '{if ($$3=="XX") print}' |\
	$(GAWK) -f $(SRC)/db2lexc.gawk >> $@
	@$(ECHO) "LEXICON OtherStems" >> $@
	@$(CAT) $(SRC)/db-german.txt |\
	$(GAWK) -F "[	]+" '{if ($$3!~/^[VX]/) print}' |\
	$(GAWK) -f $(SRC)/db2lexc.gawk >> $@


# ============================================================================
# lexicalisation of derived word forms
# ============================================================================
.PRECIOUS:	$(AUX)/derivs.lexc

$(AUX)/derivs.lexc:	$(AUX)/derivs-verb.lexc \
			$(AUX)/derivs-other.lexc
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(ECHO) "LEXICON VerbStemsD\n" > $@
	@$(CAT) $(AUX)/derivs-verb.lexc >> $@
	@$(ECHO) "LEXICON OtherStemsD\n" >> $@
	@$(CAT) $(AUX)/derivs-other.lexc >> $@

$(AUX)/derivs-%.lexc:	$(AUX)/german-A.fst2 \
			$(SRC)/cleanup-eps.xfst \
			$(SRC)/get-derivs-%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/german-A.fst2 \
		 -l $(SRC)/cleanup-eps.xfst \
		 -l $(SRC)/get-derivs-$*.xfst \
		 -e "print words > $@" -stop

# ============================================================================
# removal of derivational continuation classes
# ============================================================================
.PRECIOUS:	$(AUX)/noderivs.lexc
$(AUX)/noderivs.lexc:	$(AUX)/german.lexc \
			$(SRC)/remove-derivs.gawk
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(CAT) $(AUX)/german.lexc |\
	$(GAWK) -f $(SRC)/remove-derivs.gawk > $@


# ============================================================================
# Compilation of lexc files
# ============================================================================
LEXFILES = 	$(SRC)/header.lexc \
		$(SRC)/cont-deriv.lexc \
		$(SRC)/numerals.lexc \
		$(SRC)/flex-noun.lexc \
		$(SRC)/flex-verb.lexc \
		$(SRC)/flex-adj.lexc 

$(AUX)/german-A.lexc:	$(AUX)/german.lexc \
			$(LEXFILES)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(CAT) $(LEXFILES) $(AUX)/german.lexc > $@


$(AUX)/german-B.lexc:	$(AUX)/derivs.lexc \
			$(AUX)/noderivs.lexc \
			$(LEXFILES)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(CAT) $(LEXFILES) $(AUX)/noderivs.lexc $(AUX)/derivs.lexc > $@


$(AUX)/german-%.fst1:	$(AUX)/german-%.lexc 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "read lexc < $(AUX)/german-$*.lexc" \
		 -e "save $@" -stop


# apply rules
.PRECIOUS:	$(AUX)/german-A.fst2 \
		$(AUX)/german-B.fst2

$(AUX)/german-%.fst2:	$(AUX)/german-%.fst1 \
			$(SRC)/rules-german.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/german-$*.fst1 \
		 -l $(SRC)/rules-german.xfst \
		 -e "save $@" -stop

$(AUX)/german-%.fst:	$(AUX)/german-%.fst2 \
			$(AUX)/lex-parts.fst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/german-$*.fst2 \
		 -s $(AUX)/lex-parts.fst \
		 -e "union net" \
		 -e "save $@" -stop


# lexicon parts 

LEXDEF = prep conj prondet
LEXPARTS = $(LEXDEF:%=$(AUX)/lex-%.fst) 

$(AUX)/lex-parts.fst:	$(LEXPARTS)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) $(LEXPARTS:%= -s %) \
		 -e "union net" \
		 -e "save $@" -stop



# OTHER

$(AUX)/%.fst:	$(SRC)/%.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l $(SRC)/$*.xfst \
		 -e "save $@" -stop


