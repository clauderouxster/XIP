# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	evaluation of XIP grammar
# LANGUAGE:	Dutch
# AUTHOR:	Anne Schiller
# DATE:		05-Jun-2007
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------
# tools for Linux
ECHO 	= echo -e
XIP 	= xip -f -tr -inpututf8 

GAWK	= gawk
SDIFF	= sdiff
CAT	= cat
SORT 	= sort


# directories
AUX = ../aux
RES = ../res

# grammar to apply:
GRMDIR = ../../BASIC

GRAM = $(GRMDIR)/dutch_hmm.grm

GRMFILES = $(GRMDIR)/categories.xip \
	$(GRMDIR)/chunker.xip \
	$(GRMDIR)/controls.xip \
	$(GRMDIR)/dependencies.xip \
	$(GRMDIR)/disamb_hmm.xip \
	$(GRMDIR)/disamb.xip \
	$(GRMDIR)/dutch_hmm.xip \
	$(GRMDIR)/dutch.xip \
	$(GRMDIR)/feat_hmm.xip \
	$(GRMDIR)/features.xip \
	$(GRMDIR)/functions.xip \
	$(GRMDIR)/lexicon.xip \
	$(GRMDIR)/trans_hmm.xip \
	$(GRMDIR)/translations.xip 


FSTDIR	= ../../FST

FSTFILES = 	$(FSTDIR)/data/guesser.fst \
	$(FSTDIR)/data/guesser+hmm.fst \
	$(FSTDIR)/data/guesser+hmm+utf8.fst \
	$(FSTDIR)/data/hmm4xip.txt \
	$(FSTDIR)/data/lexicon.fst \
	$(FSTDIR)/data/lexicon+hmm.fst \
	$(FSTDIR)/data/lexicon+hmm+utf8.fst \
	$(FSTDIR)/data/norm.fst \
	$(FSTDIR)/data/openclass.fst \
	$(FSTDIR)/data/openclass+hmm.fst \
	$(FSTDIR)/data/openclass+hmm+utf8.fst 

# default test file ID
ID = volkskrant-buitenland


# ==========================================================================
# TARGETS

default : 	$(RES)/$(ID).eval

diff :		$(RES)/$(ID).hand.tok \
		$(RES)/$(ID).xip.tok
	dif $^ &

# only run once to initialize the corpus for manual correction

init:		$(AUX)/$(ID).xip
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $^ $(ID).hand


# ==========================================================================
# run parser
# ==========================================================================

$(AUX)/$(ID).xip:	$(ID).txt \
			$(GRAM) \
			$(GRMFILES) \
			$(FSTFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(XIP) -grm $(GRAM) -nm -text $(ID).txt > $@

# ==========================================================================
# extract chunks
# ==========================================================================
$(AUX)/$(ID).hand.chunks:	$(ID).hand \
				prep_eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f prep_eval.awk $< | $(SORT) > $@

$(AUX)/$(ID).xip.chunks:	$(AUX)/$(ID).xip \
				prep_eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f prep_eval.awk $< | $(SORT) > $@


# ==========================================================================
# compute statistics
# ==========================================================================
$(RES)/$(ID).eval:	$(AUX)/$(ID).hand.chunks \
			$(AUX)/$(ID).xip.chunks \
			eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(SDIFF) $(AUX)/$(ID).hand.chunks $(AUX)/$(ID).xip.chunks |\
	 $(GAWK) -f eval.awk > $@

# ==========================================================================
# "tokenize" output
# ==========================================================================
$(RES)/$(ID).hand.tok:	$(ID).hand \
			tok_eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f tok_eval.awk $<  > $@

$(RES)/$(ID).xip.tok:	$(AUX)/$(ID).xip \
			tok_eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f tok_eval.awk $< > $@

