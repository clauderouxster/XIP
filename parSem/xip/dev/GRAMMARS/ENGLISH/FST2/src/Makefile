
# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	creation of lexical FSTs
# LANGUAGE:	ENGLISH
# AUTHOR:	Anne Schiller
# DATE:		22-May-2007
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------

# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e

GAWK	= gawk
CAT	= cat

AUX = ../aux
DATA = ../data


lex:	$(DATA)/lexicon.fst \
	$(DATA)/lex+hmm.fst \
	$(DATA)/lexicon+feat.fst \
	$(DATA)/lex+hmm+feat.fst \
	$(DATA)/norm1.fst \
	$(DATA)/ntm0.txt \
	$(DATA)/ntm1.txt

# ------------------------------------------------------------------------------
# HMM lexicon with additional faetures
FEATURES  = trans syn1 ent1

FEATFILES = $(FEATURES:%=feat-%.lexc) 
ADDFEAT = $(FEATFILES:%=-e "read lexc %" -l add-features.xfst )

$(DATA)/lex%+feat.fst:	    	$(DATA)/lex%.fst \
				add-features.xfst \
				$(FEATFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/lex$*.fst \
		$(ADDFEAT) \
		-e "save stack $@" -stop


# ------------------------------------------------------------------------------
# unified lexicon
$(DATA)/lexicon.fst:	$(DATA)/lex1.fst \
			$(DATA)/lex2.fst \
			$(DATA)/spec.fst \
			combine_lex.xfst \
			compute_nadj.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/lex1.fst \
		-s $(DATA)/lex2.fst \
		-s $(DATA)/spec.fst \
		-l combine_lex.xfst \
		-l compute_nadj.xfst \
		-e "save stack $@" -stop

$(DATA)/lex+hmm.fst:	$(DATA)/lexicon.fst \
			add-hmmtags.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/lexicon.fst \
		-l add-hmmtags.xfst \
		-e "save stack $@" -stop


test-hmm:	$(DATA)/lexicon.fst \
		$(DATA)/lex+hmm.fst \
		check-mapping.xfst
	@$(ECHO) "\n------------- CHECK $@ ------------------\n"
	@$(FST) -s $(DATA)/lexicon.fst \
		-s $(DATA)/lex+hmm.fst \
		-l check-mapping.xfst

# ------------------------------------------------------------------------------
# Main lexicon

$(DATA)/lex1.fst:	lexicon.lexc
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc $^" \
		-e "save stack $@" -stop


# ------------------------------------------------------------------------------
# Special lexicon

SPECDEF =	def-currencies.xfst \
		def-numbers.xfst \
		dig-units.xfst \
		dig-dates.xfst \
		dig-currencies.xfst \
		dig-numbers.xfst \
		def-punct.xfst


$(DATA)/spec.fst:  $(SPECDEF)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) $(SPECDEF:%= -l %) \
		-e "union net" \
		-e "save stack $@" -stop


# ------------------------------------------------------------------------------
# other lexicons
LEXDEF =	func more misc names places business


$(DATA)/lex2.fst:	$(LEXDEF:%=$(AUX)/lex-%.fst)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) $(LEXDEF:%= -s $(AUX)/lex-%.fst) \
		-e "union net" \
		-e "save stack $@" -stop

$(AUX)/lex-%.fst:	lex-%.lexc \
			lex-rules.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "read lexc lex-$*.lexc" \
		-l lex-rules.xfst \
		-e "save stack $@" -stop

# ------------------------------------------------------------------------------
# Normalizer

NORMID =	space cap hyphen xml
NORMFILES =	$(NORMID:%=$(AUX)/norm-%.fst)

$(DATA)/norm1.fst:	$(NORMFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(NORMFILES:%= -s %) \
		-e "union net" \
		-e "save stack $@" -stop


$(AUX)/norm-%.fst:	def-chars.xfst \
			norm-%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l def-chars.xfst \
		-l norm-$*.xfst \
		-e "save stack $@" -stop


# ------------------------------------------------------------------------------
# NTM script
$(DATA)/ntm%.txt:	ntm%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $<  $@


