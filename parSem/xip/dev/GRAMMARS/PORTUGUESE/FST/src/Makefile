# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	FSTs and HMM files for usage with XIP grammar
# LANGUAGE:	Portuguese
# ENCODING;	UTF-8
#
# AUTHOR:	Anne Schiller
# DATE:		12-Dec-2005
# UPDATE:	03-Aug-2006	(added source files in UTF-8)
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------

# tools for Linux
FST 	= fst -utf8 -q -e "set quit-on-fail on"
ECHO 	= echo -e
GAWK	= gawk


# subdirectories
AUX = ../aux
DATA = ../data

# target files
TOK =		$(DATA)/tok.fst

LEX =		$(DATA)/lookup.txt \
		$(DATA)/lookup_abs.txt \
		$(DATA)/lex-spec.fst \
		$(DATA)/lexicon.fst \
		$(DATA)/lexicon-noacc.fst \
		$(DATA)/guesser.fst \
		$(DATA)/openclass.fst \
		$(DATA)/norm-std.fst \
		$(DATA)/norm-allcap.fst \
		$(DATA)/mark-spec.fst \
		$(DATA)/mark-lex.fst \
		$(DATA)/mark-lex-noacc.fst \
		$(DATA)/mark-guess.fst \
		$(DATA)/mark-open.fst \
		$(DATA)/hmm-tagmap.fst \
		$(DATA)/map4xip.fst

HMM =		$(DATA)/hmm4xip.txt

TARGET =	$(TOK) $(LEX) $(HMM)

# ==========================================================================
# TARGETS
default:	$(TARGET)

tok:		$(TOK)

lex:		$(LEX)

hmm:		$(HMM)

clean:	
		rm -f $(TARGET)


# ==========================================================================
# copy tokenizer transducer 
$(DATA)/tok.fst:		tok-abbr.xfst \
				tok-basic.xfst \
				tok-mwe1.xfst \
				tok-mwe2.xfst \
				tok-sgml.xfst \
				tok-stapler.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)  -l tok-abbr.xfst \
		 -l tok-basic.xfst \
		 -l tok-mwe1.xfst \
		 -l tok-stapler.xfst \
		 -l tok-mwe2.xfst \
		 -l tok-stapler.xfst \
		 -l tok-sgml.xfst \
		 -l tok-stapler.xfst \
		 -e "save stack $@" -stop


# ==========================================================================
# HMM table (for POS disambiguation)
# NOTE:  the source file "hmm_xelda.txt" is copied from
# ~/Xelda/xelda-lingdata/lingdata/Disambiguation/HMM/Portuguese/data/hmm-std-1-def

# .. with additional prefix "hmm_" to HMM tags
# (in order to avoid conflicts with lower case feature names)

$(DATA)/hmm4xip.txt: 	hmm-xelda.txt \
			hmm-add-prefix.awk
	@$(ECHO) "------------- updating $@ ------------------"
	@$(GAWK) -f hmm-add-prefix.awk < $< > $@



# ==========================================================================
# lookup script
$(DATA)/lookup.txt:	lookup.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@cp $< $@



# lookup script with absolute path (for usage with run-time program "lookup")
FSTDIR = $(PWD:/src=/data)
$(DATA)/lookup_abs.txt:	lookup.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@$(GAWK) '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@


# ==========================================================================
# lexicon FST
# NOTE: the source file "lex-xelda.fst" is copied from
# ~/Xelda/xelda-lingdata/lingdata/MorphoAnalysis/FSTMorphoTags/Portuguese/data/lexicon.fst

$(DATA)/lexicon.fst:	lex-xelda.fst \
			lex-cleanup.xfst \
			lex-modify.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-s lex-xelda.fst \
		-l lex-cleanup.xfst \
		-l lex-modify.xfst \
		-e "save stack $@" -stop

# lexicon without accents
$(DATA)/lexicon-noacc.fst:	$(DATA)/lexicon.fst \
				norm-accents.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-s $(DATA)/lexicon.fst -e "def LEX" \
		-l norm-accents.xfst -e "def MAP" \
		-e "read regex LEX .o. MAP ;" \
		-e "save stack $@" -stop


# ==========================================================================
# special lexicon for punctuations, symbols, SGML-markers, ...
$(DATA)/lex-spec.xfst:	lex-punct.xfst \
			lex-sgml.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l lex-punct.fst \
		-l lex-sgml.xfst \
		-e "union net" \
		-e "save stack $@" -stop


# ==========================================================================
# lexicon markers (to identify where a word's analysis comes from)

$(DATA)/mark-%.fst:
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-e "define MARK %+%#$*;" \
		-e "read regex MARK <- [..] || _ .#.;"\
		-e "save stack $@" -stop

# ==========================================================================
# OTHER FSTs
# * hmm-tagmap.fst
# * guesser.fst
# * openclass.fst
# * norm-std.fst, norm-allcap.fst

$(DATA)/%.fst:	%.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l $^ \
		-e "save stack $@" -stop






