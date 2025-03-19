# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	FSTs for usage with XIP grammar and NTM
# LANGUAGE:	PORTUGUESE
# AUTHOR:	Anne Schiller
# DATE:		07-Aug-2008
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------
# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e


# ---------------------------------------------------------------------------
# files copied from XELDAHOME 
#	= /home/schiller/Xelda/xelda-lingdata/lingdata
#
# (1) morphological analyser morph-base.fst
#	= $XELDAHOME/MorphoAnalysis/FSTMorphoTags/Portuguese/data/lexicon.fst
#
# (2) HMM table hmm.txt
#	= $XELDAHOME/Disambiguation/HMM/Portuguese/data/hmm-std-1-def
# ---------------------------------------------------------------------------

AUX = ../aux
DATA = ../data

NTM  = 		$(DATA)/ntm.txt \
		$(DATA)/ntm_abs.txt \
		$(DATA)/xml4ntm.fst \
		$(DATA)/lex4ntm.fst \
		$(DATA)/guess4ntm.fst \
		$(DATA)/open4ntm.fst \
		$(DATA)/norm1.fst \
		$(DATA)/norm2.fst \
		$(DATA)/norm3.fst 

NTMHMM  = 	$(DATA)/ntm+hmm.txt \
		$(DATA)/ntm+hmm_abs.txt \
		$(DATA)/xml4ntm+hmm.fst \
		$(DATA)/lex4ntm+hmm.fst \
		$(DATA)/guess4ntm+hmm.fst \
		$(DATA)/open4ntm+hmm.fst \
		$(DATA)/norm1.fst \
		$(DATA)/norm2.fst \
		$(DATA)/norm3.fst \
		$(DATA)/hmm4xip.txt

# ==========================================================================
# TARGETS
default:	ntm1 ntm2

# for ntm without HMM disambiguation (i.e.without HMM tags)
ntm1:		$(NTM)

# lexical FSTs with final HMM tags
ntm2:		$(NTMHMM)

clean:	
		rm -f $(NTM)
		rm -f $(NTMHMM)
		rm -f $(AUX)/*.*


# ==========================================================================
# HMM table
# .. with additional prefix "hmm_" to HMM tags
# (in order to avoid conflicts with lower case feature names)

$(DATA)/hmm4xip.txt: 	hmm.txt \
			add-hmm.awk
	@echo "------------- updating $@ ------------------"
	@gawk -f add-hmm.awk < $< > $@

# ==========================================================================
# modified morphological analyser
# ==========================================================================
$(AUX)/morph.fst:	morph-base.fst \
			morph-cleanup.xfst \
			add-features.lexc \
			add-features.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s morph-base.fst \
		-l morph-cleanup.xfst \
		-e "read lexc add-features.lexc" \
		-l add-features.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# additional lexicons, e.g.
# - lex-additions	= additional common entries
# - lex-func		= function words (prepositions, pronouns, conjunctions, ...)
# - lex-countries
# - lex cities
# - lex-loc
# ==========================================================================
ADDLEX =	additions func \
	 	countries cities loc demonyms \
	 	names org brands prop

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

$(DATA)/lex4ntm.fst:	$(AUX)/morph.fst \
			$(AUX)/lex-all.fst \
			$(AUX)/spec-all.fst \
			map4xip.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/morph.fst \
		-s $(AUX)/lex-all.fst \
		-s $(AUX)/spec-all.fst \
		-e "union net" \
		-e "def LEX" -e "read regex LEX %+%#lex:0 ;" \
		-l map4xip.xfst \
		-e "save stack $@" -stop



# ==========================================================================
# special XML tag lexicon
# ==========================================================================
$(DATA)/xml4ntm.fst:	spec-xml.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l spec-xml.xfst \
		-e "def LEX" -e "read regex LEX %+%#xml:0 ;" \
		-l map4xip.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# guesser transducer
# ==========================================================================
$(DATA)/guess4ntm.fst:	guesser.xfst \
			map4xip.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l guesser.xfst \
		-l map4xip.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# open class transducer
# ==========================================================================
$(DATA)/open4ntm.fst:	open4ntm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $^ \
		-e "save stack $@" -stop

$(DATA)/open4ntm+hmm.fst:	open4ntm+hmm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $^ \
		-e "save stack $@" -stop

# ==========================================================================
# lexical FSTs with HMM tags
# ==========================================================================
$(DATA)/%+hmm.fst:	$(DATA)/%.fst \
			map4hmm.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/$*.fst \
		-l map4hmm.xfst \
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
# NTM script (for usage without HMM)
$(DATA)/ntm.txt:	ntm.txt
	@echo "------------- updating $@ ------------------"
	@cp $< $@


# NTM script for usage with HMM
$(DATA)/ntm+hmm.txt:	ntm.txt
	@echo "------------- updating $@ ------------------"
	gawk '{sub(/4ntm/,"4ntm+hmm");print}' < $< > $@

# NTM script with absolute path
FSTDIR = $(PWD:/src=)
$(DATA)/ntm_abs.txt:	$(DATA)/ntm.txt
	@echo "------------- updating $@ ------------------"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@


# NTM script with absolute path
FSTDIR = $(PWD:/src=)
$(DATA)/ntm+hmm_abs.txt:	$(DATA)/ntm+hmm.txt
	@echo "------------- updating $@ ------------------"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@





