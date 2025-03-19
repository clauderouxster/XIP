# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	FSTs and HMM files for usage with XIP grammar
# LANGUAGE:	Spanish
# AUTHOR:	Anne Schiller
# DATE:		19-Dec-2005
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------


# tools for Linux
FST = fst -q -e "set quit-on-fail on"
ECHO = echo -e

# ---------------------------------------------------------------------------
# files copied from XELDAHOME 
#	= /home/schiller/Xelda/xelda-lingdata/lingdata
#
# (1) morphological analyser morph-base.fst
#	= $XELDAHOME/MorphoAnalysis/FSTPOSTag/Spanish/auxiliary/lex-0.fst
#
# (2) HMM table hmm.txt
#	= $XELDAHOME/Disambiguation/HMM/Spanish/data/hmm-std-1-def
#
# ---------------------------------------------------------------------------


AUX = ../aux
DATA = ../data


TARGET = 	$(DATA)/tok.fst \
		$(DATA)/lookup.txt \
		$(DATA)/lookup_abs.txt \
		$(DATA)/lex+morph.fst \
		$(DATA)/guess+morph.fst \
		$(DATA)/open4xip.fst \
		$(DATA)/norm-std.fst \
		$(DATA)/norm-allcap.fst \
		$(DATA)/norm-accents.fst \
		$(DATA)/map4xip.fst \
		$(DATA)/hmm4xip.txt

#		$(DATA)/lex-spec.fst \

# ==========================================================================
# TARGETS
default:	$(TARGET)

clean:	
		rm -f $(TARGET)
		rm -f $(AUX)/*.*

# ==========================================================================
# tokenizer transducer 
# ==========================================================================
$(DATA)/tok.fst:	tok-abbr.xfst \
			tok-basic.xfst \
			tok-mwe.xfst \
			tok-stapler.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l tok-abbr.xfst \
		 -l tok-basic.xfst \
		 -l tok-mwe.xfst \
		 -l tok-stapler.xfst \
		 -e "optimize net" \
		 -e "save stack $@" -stop

# ==========================================================================
# modified morphological analyser (without HMM tags)
# ==========================================================================
$(AUX)/lex-base.fst:	morph-base.fst \
			lex-modify.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s morph-base.fst \
		-l lex-modify.xfst \
		-e "save stack $@" -stop



# ==========================================================================
# lexicon transducer
# ==========================================================================
$(AUX)/lex+hmm.fst:	$(AUX)/lex-base.fst \
			lex-tagmap.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/lex-base.fst \
		-l lex-tagmap.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# morphological analyser with syntactic/sematic tags (and HMM tags)
# ==========================================================================
$(DATA)/lex+morph.fst:	$(AUX)/lex+hmm.fst \
			lex-features.lexc \
			add-features.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc lex-features.lexc" \
		-s $(AUX)/lex+hmm.fst \
		-l add-features.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# guesser transducer
# ==========================================================================
$(DATA)/guess+morph.fst:	guesser.xfst \
				lex-tagmap.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l guesser.xfst \
		-l lex-tagmap.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# open class transducer
# ==========================================================================
$(DATA)/open4xip.fst:		open4xip.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l open4xip.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# other FSTS
# - normalizers
# - mapping for xip
# ==========================================================================
$(DATA)/%.fst:		%.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $*.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# HMM table
# .. with additional prefix "hmm_" to HMM tags
# (in order to avoid conflicts with lower case feature names)

$(DATA)/hmm4xip.txt: 	hmm.txt \
			add_hmm.awk
	@echo "------------- updating $@ ------------------"
	@gawk -f add_hmm.awk < $< > $@


# ==========================================================================
# lookup script
$(DATA)/lookup.txt:	lookup.txt
	@echo "------------- updating $@ ------------------"
	@cp $< $@


# lookup script with absolute path
FSTDIR = $(PWD:/src=/data)
$(DATA)/lookup_abs.txt:	lookup.txt
	@echo "------------- updating $@ ------------------"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@






