
# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	creation of lexical FSTs
# LANGUAGE:	SWEDISH
# AUTHOR:	Anne Schiller
# DATE:		08-Jun2010
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

all:	lex cpd spec guess norm ntm

lex:	$(DATA)/lexicon.fst

cpd:	$(DATA)/compounds.fst

spec:	$(DATA)/specials.fst

guess:	$(DATA)/guesser.fst

ntm:	$(DATA)/ntm1.txt

norm:	$(DATA)/norm.fst


func:	$(AUX)/lex-func.fst

# ------------------------------------------------------------------------------
# continuation classes
$(AUX)/cont.lexc:	inflection_table.txt \
			rules2cont.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f rules2cont.awk $< > $@

# ------------------------------------------------------------------------------
# mapping rules
$(AUX)/map.awk:		inflection_table.txt \
			rules2awk.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f rules2awk.awk $< > $@

# ------------------------------------------------------------------------------
$(AUX)/root.lexc:	inflection_lex.txt \
			$(AUX)/map.awk \
			lex2lexc.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f $(AUX)/map.awk  -f lex2lexc.awk $< > $@


# ------------------------------------------------------------------------------
# lexc source file
$(AUX)/lexicon.lexc:	$(AUX)/root.lexc \
			$(AUX)/cont.lexc
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cat $^ >  $@

# ------------------------------------------------------------------------------
# lexicon fst
# step 1: compiling lexc files
# step 2: adding "phonological" rules (mainly cutting affixes)
# step 3: map SUC tags onto Xerox-style tagset
# step 4: add function word lexicon

$(AUX)/lexicon1.fst:	$(AUX)/lexicon.lexc
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc $<" \
		-e "save stack $@" -stop

$(AUX)/lexicon2.fst:	$(AUX)/lexicon1.fst \
			lex-rules.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-l lex-rules.xfst \
		-e "save stack $@" -stop

$(AUX)/lexicon3.fst:	$(AUX)/lexicon2.fst \
			map-tags.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-l map-tags.xfst \
		-e "save stack $@" -stop

$(DATA)/lexicon.fst:	$(AUX)/lexicon3.fst \
			$(AUX)/lex-func.fst \
			$(AUX)/lex-abbr.fst \
			$(AUX)/numbers.fst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-s $(AUX)/lex-func.fst \
		-s $(AUX)/numbers.fst \
		-s $(AUX)/lex-abbr.fst \
		-e "union net" \
		-e "save stack $@" -stop

# ------------------------------------------------------------------------------
# compound lexicon

$(DATA)/compounds.fst:	$(AUX)/lexicon3.fst \
			compounds.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-l compounds.xfst \
		-e "save stack $@" -stop

# ------------------------------------------------------------------------------
# special lexicon (punctuations, digits, etc.)
SPEC = digits punct spec
SPECFILES = $(SPEC:%=$(AUX)/%.fst)

$(DATA)/specials.fst:	$(SPECFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	$(FST) $(SPECFILES:%= -s %) \
		-e "union net" \
		-e "save stack $@" -stop



# ------------------------------------------------------------------------------
# NTM scripts
$(DATA)/ntm%.txt:	ntm%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $< $@



# ------------------------------------------------------------------------------
# other data FSTs (guesser, normalizer, ...)

$(DATA)/%.fst:	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $< \
		-e "save stack $@" -stop

# ------------------------------------------------------------------------------
# other intermediate files

$(AUX)/lex-%.fst: 	lex-%.lexc
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	$(FST) -e "read lexc $<" \
		-e "save stack $@" -stop

$(AUX)/%.fst:	%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $< \
		-e "save stack $@" -stop

