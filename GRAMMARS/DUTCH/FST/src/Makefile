# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	creation of lexical FSTs for usage with XIP
# LANGUAGE:	Dutch
# AUTHOR:	Anne Schiller
# DATE:		06-Mar-2007
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

LEX0 =	$(DATA)/lexicon.fst \
	$(DATA)/guesser.fst \
	$(DATA)/openclass.fst 
LEX1 = 	$(LEX0:%.fst=%+hmm.fst)
LEX2 = 	$(LEX0:%.fst=%+utf8.fst)
LEX3 = 	$(LEX0:%.fst=%+hmm+utf8.fst)


TARGET = 	$(LEX0) $(LEX1) $(LEX3) \
		$(DATA)/norm.fst \
		$(DATA)/norm2.fst \
		$(DATA)/hmm4xip.txt


PARTS =		$(AUX)/pronouns.fst


# ==========================================================================
# TARGETS
default:	$(TARGET)

parts:		$(PARTS)

clean:	
		rm -f $(TARGET)
		rm -f $(AUX)/*.*



ID=pronouns
test:		$(AUX)/$(ID).old \
		$(AUX)/$(ID).new

lex1:		$(TARGET1)

# ==========================================================================
# lexicon 
# ==========================================================================

$(AUX)/lex1.fst:	lexicon.fst \
			removals.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s lexicon.fst -e "unopt" \
		-l removals.xfst \
		-e "save stack $@" -stop

$(DATA)/lexicon.fst:	$(AUX)/lex1.fst \
			additions.xfst \
			pronouns.xfst \
			punct.xfst \
			$(AUX)/lex-countries.fst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/lex1.fst \
		-l additions.xfst \
		-l pronouns.xfst \
		-l punct.xfst \
		-s $(AUX)/lex-countries.fst \
		-e "union net" \
		-e "save stack $@" -stop


# ==========================================================================
# guesser 
# ==========================================================================
$(DATA)/guesser.fst:	guesser.xfst \
			map4xip.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l guesser.xfst \
		-l map4xip.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# including HMM tags
# ==========================================================================
$(DATA)/%+hmm.fst:	$(DATA)/%.fst \
			map4hmm.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-s $(DATA)/$*.fst \
		-l map4hmm.xfst \
		-e "compose net" \
		-e "save stack $@" -stop

# ==========================================================================
# HACK for UTF-8 symbols
# ==========================================================================
$(DATA)/%+utf8.fst:	$(DATA)/%.fst \
			map4utf.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-s $(DATA)/$*.fst \
		-l map4utf.xfst \
		-e "compose net" \
		-e "save stack $@" -stop


# ==========================================================================
# OTHER 
# -  normalizer
# ==========================================================================
$(DATA)/norm2.fst:	$(DATA)/norm.fst \
			$(DATA)/norm-accents.fst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-s $(DATA)/norm.fst \
		-s $(DATA)/norm-accents.fst \
		-e "union net" \
	     	-e "save stack $@" -stop

$(DATA)/%.fst: 	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-l $< \
	     	-e "save stack $@" -stop



$(DATA)/hmm4xip.txt: 	hmm.txt \
			add_hmm.awk
	@$(ECHO) "------------- updating $@ ------------------"
	@$(GAWK) -f add_hmm.awk < $< > $@



# ==========================================================================
# auxiliary files
$(AUX)/lex-%.all:	flex-header.lexc \
			lex-%.lexc
	@$(ECHO) "------------- updating $@ ------------------"
	@$(CAT) $^ > $@

$(AUX)/lex-%.fst:	$(AUX)/lex-%.all \
			flex-rules.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-e "read lexc $<" \
		-l flex-rules.xfst \
		-e "save stack $@" -stop


$(AUX)/%.fst: 	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)	-l $< \
	     	-e "save stack $@" -stop



# ==========================================================================
# BASIC tests

test-%:		$(AUX)/lex-%.fst \
		test-lex.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-l test-lex.xfst -stop


show-%:		$(AUX)/lex-%.fst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
		@$(FST) -s $< \
			-e "echo >>> random upper words" \
			-e "random-low" \
			-e "echo >>> random lower words" \
			-e "random-upp"

