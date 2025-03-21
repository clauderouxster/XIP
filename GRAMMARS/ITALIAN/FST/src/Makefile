# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	FSTs and HMM files for usage with XIP grammar
# LANGUAGE:	Italian
# AUTHOR:	Anne Schiller
# DATE:		16-Jan-2006
# UPDATED:	20-feb-2006	(removed links; added source files)
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
#	= $XELDAHOME/MorphoAnalysis/FSTMorphoTag/Italian/data/lexicon.fst
#
# (2) HMM table hmm.txt
#	= $XELDAHOME/Disambiguation/HMM/Italian/data/hmm-std-1-def
#
# ---------------------------------------------------------------------------

AUX = ../aux
DATA = ../data

NTM  = 		$(DATA)/ntm.txt \
		$(DATA)/ntm_abs.txt \
		$(DATA)/lex4ntm.fst \
		$(DATA)/guess4ntm.fst \
		$(DATA)/open4ntm.fst \
		$(DATA)/norm1.fst \
		$(DATA)/norm2.fst \
		$(DATA)/hmm4xip.txt

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
ntm:		$(NTM)

default:	$(TARGET)


clean:	
		rm -f $(TARGET)
		rm -f $(NTM)
		rm -f $(AUX)/*.*

# ==========================================================================
# tokenizer transducer 
# ==========================================================================
$(DATA)/tok.fst:	tok-abbr.xfst \
			tok-elid.xfst \
			tok-basic.xfst \
			tok-mwe.xfst \
			tok-stapler.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l tok-abbr.xfst \
		-l tok-elid.xfst \
		-l tok-basic.xfst \
		-l tok-mwe.xfst \
		-l tok-stapler.xfst \
		-e "optimize net" \
		-e "save stack $@" -stop

# ==========================================================================
# modified morphological analyser (without HMM tags)
# ==========================================================================
$(AUX)/lex-base.fst:	morph-base.fst \
			morph-cleanup.xfst \
			lex-delete.xfst \
			lex-modify.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s morph-base.fst \
		-l morph-cleanup.xfst \
		-l lex-delete.xfst \
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

$(DATA)/open4ntm.fst:		norm-ntm.xfst \
				open4ntm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l norm-ntm.xfst \
		-l open4ntm.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# lexical FSTs for NTM
# ==========================================================================
$(DATA)/%4ntm.fst:	$(DATA)/%+morph.fst \
			tok-abbr.xfst \
			cleanup4ntm.xfst \
			map4xip.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/$*+morph.fst \
		-l tok-abbr.xfst \
		-l cleanup4ntm.xfst -e "define LEX" \
		-l map4xip.xfst -e "define MAP" \
		-e "read regex MAP .o. LEX ;" \
		-e "save stack $@" -stop


# ==========================================================================
# NTM normalizers
# ==========================================================================
$(DATA)/norm1.fst:		norm-ntm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $^ \
		-e "read regex [ NORMWS | NORMINIT | NORMCAPS | NORMXML ];" \
		-e "save stack $@" -stop

$(DATA)/norm2.fst:		norm-ntm.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $^ \
		-e "read regex [ NORMWS | NORMINIT | NORMCAPS | NORMACC | NORMXML ];" \
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


# ==========================================================================
# NTM script
$(DATA)/ntm.txt:	ntm.txt
	@echo "------------- updating $@ ------------------"
	@cp $< $@


# lookup script with absolute path
FSTDIR = $(PWD:/src=)
$(DATA)/ntm_abs.txt:	ntm.txt
	@echo "------------- updating $@ ------------------"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@






