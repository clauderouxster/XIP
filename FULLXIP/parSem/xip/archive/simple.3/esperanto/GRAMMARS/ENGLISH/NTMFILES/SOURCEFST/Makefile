
BASICS = ./BASICS
ENTITIES = ./FORENTITIES
SYNTAX = ./FORSYNTAX
SEMANTICS = ./FORSEM

# tools for Linux
FST = fst -q -e "set quit-on-fail on"
ECHO = echo -e


all:	$(ENTITIES)/SpecialStrings.fst \
	$(ENTITIES)/cities.fst \
	$(SYNTAX)/lex_nouv.fst \
	$(BASICS)/alternative.fst\
	$(BASICS)/nv-open-1.fst\
	$(BASICS)/nvlex+context.fst \
	$(ENTITIES)/nvlex+context+add.fst \
	$(SEMANTICS)/nvlex+context+add+sem+alter.fst \
	lexicon_sem.fst \
	lexicon_num.fst



## LEXICON with number conversion:

LEX=$(SEMANTICS)/nvlex+context+add+sem+alter.fst

lexicon_sem.fst: $(LEX) \
		mark_internal.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(LEX) \
		-l mark_internal.xfst \
		-e "save $@" -stop


lexicon_num.fst: $(LEX) \
		mark_internal.xfst \
		numbers.xfst \
		add_numbers.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(LEX) \
		-l mark_internal.xfst \
		-l numbers.xfst \
		-l add_numbers.xfst \
		-e "save $@" -stop



## TARGET 

# THINGFINDER STUFF
# URLS, EMAIL, PHONE NUMBERS :SpecialStrings.fst 

$(ENTITIES)/SpecialStrings.fst:	$(ENTITIES)/SpecialStrings.xfst $(ENTITIES)/Entity_BasicDef.xfst
	fst -e "source $(ENTITIES)/Entity_BasicDef.xfst" -l $(ENTITIES)/SpecialStrings.xfst -e "save $(ENTITIES)/SpecialStrings.fst" -stop

# For unknown city names : cities.fst

$(ENTITIES)/cities.fst: $(ENTITIES)/cities.xfst
	fst -l $(ENTITIES)/cities.xfst -e "save $(ENTITIES)/cities.fst" -stop

#fOR XIP LEXICONS:
# lex_nouv.fst:
$(SYNTAX)/lex_nouv.fst: $(BASICS)/lex-1.fst $(BASICS)/remove-words.xfst $(SYNTAX)/add-s_tags.xfst
	./script_lexnouv


# nvlex+context.fst
$(BASICS)/nvlex+context.fst: $(SYNTAX)/lex_nouv.fst $(BASICS)/nvlex+context.xfst
	fst -e "load $(SYNTAX)/lex_nouv.fst" -l  $(BASICS)/nvlex+context.xfst -e  "save $(BASICS)/nvlex+context.fst" -stop

# The new lexicon with some words addeds 
# nvlex+context+add.fst : entities
$(ENTITIES)/nvlex+context+add.fst: $(ENTITIES)/additions.xfst $(SYNTAX)/lex_nouv.fst $(BASICS)/nvlex+context.fst $(ENTITIES)/titles.xfst $(ENTITIES)/professions.xfst $(ENTITIES)/locations.xfst $(ENTITIES)/adjectivesCountry.xfst $(ENTITIES)/locationMod.xfst $(ENTITIES)/pers_name.xfst $(ENTITIES)/pers1.xfst $(ENTITIES)/pers2.xfst $(ENTITIES)/event_law.xfst $(ENTITIES)/organisation.xfst $(ENTITIES)/orgNames.xfst $(ENTITIES)/villes.xfst
	fst  -e "source $(ENTITIES)/adjectivesCountry.xfst" -e "source $(ENTITIES)/professions.xfst" -e "source  $(ENTITIES)/locations.xfst"    -e "source  $(ENTITIES)/locationMod.xfst" -e "source $(ENTITIES)/pers_name.xfst"  -e "source $(ENTITIES)/pers1.xfst"  -e "source $(ENTITIES)/pers2.xfst"  -e "source $(ENTITIES)/event_law.xfst" -e "source $(ENTITIES)/organisation.xfst"  -e "source $(ENTITIES)/orgNames.xfst"  -e "source $(ENTITIES)/villes.xfst"   -e "source $(ENTITIES)/titles.xfst" -e "load stack $(BASICS)/lex-1.fst" -e "define  basicDic" -e "load stack $(BASICS)/nvlex+context.fst" -l  $(ENTITIES)/additions.xfst  -e "save $(ENTITIES)/nvlex+context+add.fst" -stop

$(BASICS)/alternative.fst: $(BASICS)/alternative.xfst
	fst -l  $(BASICS)/alternative.xfst -e "save $(BASICS)/alternative.fst" -stop

$(BASICS)/nv-open-1.fst: $(BASICS)/nv-open-1.xfst
	fst -l  $(BASICS)/nv-open-1.xfst -e "save $(BASICS)/nv-open-1.fst" -stop

# Semantics
$(SEMANTICS)/nvlex+context+add+sem+alter.fst: $(ENTITIES)/nvlex+context+add.fst $(SEMANTICS)/add-semantics.xfst $(SEMANTICS)/add-verb-alternation.xfst
	fst -s  $(ENTITIES)/nvlex+context+add.fst -l $(SEMANTICS)/add-semantics.xfst -e "save $(SEMANTICS)/nvlex+context+add+sem.fst" -l $(SEMANTICS)/add-verb-alternation.xfst -e "save $(SEMANTICS)/nvlex+context+add+sem+alter.fst" -stop
