
# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	creation of lexical FSTs for XIP
# LANGUAGE:	POLISH
# AUTHOR:	Anne Schiller
# DATE:		12-Jul-2010
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------

# FSTs copied from Xelda-lingdata (format UTF-8)

# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e

GAWK	= gawk
CAT	= cat

AUX = ../aux
DATA = ../data

default:	tok lex norm lkup hmm

tok:		$(DATA)/tok.fst

lex:		$(DATA)/lex-morph.fst \
		$(DATA)/lex-noacc.fst \
		$(DATA)/lex-guess.fst \
		$(DATA)/lex-open.fst

norm:		$(DATA)/norm-std.fst \
		$(DATA)/norm-allcap.fst

lkup:		$(DATA)/lookup1.txt \
		$(DATA)/lookup1_abs.txt

hmm:		$(DATA)/hmm.txt

# ------------------------------------------------------------------------------
# tokenizer
$(DATA)/tok.fst:	tok.fst
	@$(ECHO) "\n------------- copying $@ ------------------\n"
	cp $< $@

# ------------------------------------------------------------------------------
# lexicons, guessers
# - lex-morph
# - lex-noacc
# - lex-guesser
# - lex-open

$(DATA)/lex-guess.fst:	guesser.xfst 	
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	$(FST) -l $< -e "define LEX" \
		-e "regex [ %+guess <- [..] || _ ? .#. ] .o. LEX ;" \
		-e "save stack $@" -stop

$(DATA)/lex-open.fst:	openclass.xfst 	
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	$(FST) -l $< -e "save stack $@" -stop

$(DATA)/lex-%.fst : 	lex-%.fst \
			map4xip.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	$(FST) -s $< -e "define LEX" \
		-e "regex [ %+$* <- [..] || _ ? .#. ] .o. LEX ;" \
		-l map4xip.xfst \
		-e "save stack $@" -stop


# ------------------------------------------------------------------------------
# other FSTs (normalizers, mappings, ...)
$(DATA)/%.fst:	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $< \
		-e "save stack $@" -stop

$(DATA)/lookup%.txt:		lookup%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $< $@


# lookup script with absolute path
FSTDIR = $(PWD:/src=/data)
$(DATA)/lookup%_abs.txt:	lookup%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@

# ------------------------------------------------------------------------------
# HMM table
$(DATA)/hmm.txt:	hmm.txt \
			map_hmm.sed
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	sed -f map_hmm.sed $< > $@


# ------------------------------------------------------------------------------
# other intermediate files

$(AUX)/%.fst:	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $< \
		-e "save stack $@" -stop

