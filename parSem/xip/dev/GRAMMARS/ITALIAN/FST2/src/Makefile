# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	FSTs for usage with XIP grammar
# LANGUAGE:	Italian
# AUTHOR:	Anne Schiller
# DATE:		16-Jan-2006
# UPDATED:	20-feb-2006	(removed links; added source files)
# UPDATED:	14-May-2008	(adapted to NTM; removed HMM)
# UPDATED:	15-Jul-2008	(using new basic analyser)
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------
# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e

# version of morphology (leave empty for current version)
ID=
# for old analyser: 
# ID=1


# ---------------------------------------------------------------------------
# files copied from XELDAHOME 
#	= /home/schiller/Xelda/xelda-lingdata/lingdata
#
# (1) morphological analyser morph-base.fst
#	= $XELDAHOME/MorphoAnalysis/FSTMorphoTag/Italian/data/lexicon.fst
#
# ---------------------------------------------------------------------------

AUX = ../aux$(ID)
DATA = ../data$(ID)

NTM  = 		$(DATA)/ntm.txt \
		$(DATA)/ntm_abs.txt \
		$(DATA)/lex4ntm.fst \
		$(DATA)/guess4ntm.fst \
		$(DATA)/open4ntm.fst \
		$(DATA)/norm1.fst \
		$(DATA)/norm2.fst \
		$(DATA)/norm3.fst 

# ==========================================================================
# TARGETS
default:	$(NTM)

clean:	
		rm -f $(NTM)
		rm -f $(AUX)/*.*

# ==========================================================================
# modified morphological analyser
# ==========================================================================
# ID=1
$(AUX)/morph1.fst:	morph-base1.fst \
			morph-cleanup1.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s morph-base1.fst \
		-l morph-cleanup1.xfst \
		-e "save stack $@" -stop

# ID=""
$(AUX)/morph.fst:	italian.lexc \
			lex-rules.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc italian.lexc" \
		-l lex-rules.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# additional lexicons
# - lex-func	= function words (prepositins, pronouns, conjunctions, ...)
# - lex-more	= additional common entries
# - lex-countries
# - lex regions
# - lex-names
# ==========================================================================
ADDLEX = adv func more numbers \
	countries cities lakes mountains regions places names lastnames foreignames \
	rivers newspapers sport professions organizations

ADDFILES = $(ADDLEX:%=$(AUX)/lex-%.fst)

$(AUX)/lex-all.fst:	$(ADDFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(ADDFILES:%=-s %) \
		-e "union net" \
		-e "save stack $@" -stop

# partial lexicon
$(AUX)/lex-%.fst:	lex-%.lexc \
			flex-rules.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc lex-$*.lexc" \
		-l flex-rules.xfst \
		-e "save stack $@" -stop

# TEMPORARY !!!

OLDLEX = places names cities countries org 

OLDFILES = $(OLDLEX:%=$(AUX)/old-%.fst)

$(AUX)/old-all.fst:	$(OLDFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(OLDFILES:%=-s %) \
		-e "union net" \
		-e "save stack $@" -stop

$(AUX)/old-%.fst:	old-%.lexc \
			flex-rules.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc old-$*.lexc" \
		-l flex-rules.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# special lexicons
# - spec-punct	= punctuations and symbols
# - spec-units	= measure units, currencies
# - spec-num	= numerals
# - spec-url	= URLs, e-mail, etc.
# ==========================================================================
SPECLEX = punct num date curr units
# more: url
SPECFILES = $(SPECLEX:%=$(AUX)/spec-%.fst)

$(AUX)/spec-all.fst:	$(SPECFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(SPECFILES:%=-s %) \
		-e "union net" \
		-e "save stack $@" -stop

# partial lexicon
$(AUX)/spec-%.fst:	spec-%.xfst \
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $^ \
		-e "save stack $@" -stop


# ==========================================================================
# Combine morphological lexicons

$(DATA)/lex4ntm.fst:	$(AUX)/morph$(ID).fst \
			$(AUX)/lex-all.fst \
			$(AUX)/spec-all.fst \
			map4xip.xfst \
			map-accents.xfst \
			map-apos.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/morph$(ID).fst \
		-s $(AUX)/lex-all.fst \
		-s $(AUX)/spec-all.fst \
		-e "union net" \
		-l map4xip.xfst \
		-l map-accents.xfst \
		-l map-apos.xfst \
		-e "save stack $@" -stop



# ==========================================================================
# guesser transducer
# ==========================================================================
$(DATA)/guess4ntm.fst:	guesser.xfst \
			map4xip.xfst\
			map-accents.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l guesser.xfst \
		-l map4xip.xfst \
		-l map-accents.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# open class transducer
# ==========================================================================
$(DATA)/open4ntm.fst:	open4ntm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l open4ntm.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# NTM normalizers
# ==========================================================================
$(DATA)/norm1.fst \
$(DATA)/norm2.fst \
$(DATA)/norm3.fst:		norm-ntm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $^ \
		-e "read regex NORM1;" \
		-e "save stack $(DATA)/norm1.fst" \
		-e "clear stack" \
		-e "read regex NORM2;" \
		-e "save stack $(DATA)/norm2.fst" \
		-e "clear stack" \
		-e "read regex NORMGUESS;" \
		-e "save stack $(DATA)/norm3.fst" \
		-stop


# ==========================================================================
# other FSTs
# ==========================================================================
$(DATA)/%.fst:		%.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $*.xfst \
		-e "save stack $@" -stop


$(AUX)/%.fst:		%.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $*.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# NTM script
$(DATA)/ntm.txt:	ntm.txt
	@echo "------------- updating $@ ------------------"
	@cp $< $@


# NTM script with absolute path
FSTDIR = $(PWD:/src=)
$(DATA)/ntm_abs.txt:	ntm.txt
	@echo "------------- updating $@ ------------------"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@






