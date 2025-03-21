# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	evaluation of XIP grammar
# LANGUAGE:	ITALIAN
# AUTHOR:	Anne Schiller
# DATE:		28-JuL-2007
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------
# tools for Linux
ECHO 	= echo -e
XIP 	= xip -f -tr -inpututf8 -sentence -sentencenum

GAWK	= gawk
GREP	= grep
SDIFF	= sdiff
CAT	= cat
MORE	= less
SORT 	= sort
JDIFF 	= /opt/ParSem/tools/jmeld/jdiff

# get current time
DATE 	= 'date +%Y-%m-%d_%k%M'
DATESH:sh = date

# Directories
AUX = ../aux
RES = ../res

# default test file ID
ID = eval1

# named entities
DEP = NE

# grammar to apply:
XIPDIR 	= ../..

GRAM = $(XIPDIR)/GRMFILES/italian_ntm.grm
GRMFILES = 	$(XIPDIR)/GRMFILES/italian.xip \
		$(XIPDIR)/FEATURES/features.xip \
		$(XIPDIR)/FEATURES/categories.xip \
		$(XIPDIR)/FEATURES/translations.xip \
		$(XIPDIR)/FEATURES/controls.xip \
		$(XIPDIR)/DEPENDENCIES/functions.xip \
		$(XIPDIR)/LEXICONS/lexicon_NE_keywords.xip \
		$(XIPDIR)/LEXICONS/lexicon.xip  \
		$(XIPDIR)/FEATURES/script.xip \
		$(XIPDIR)/DISAMB/disamb0.xip \
		$(XIPDIR)/LEXICONS/terms.xip \
		$(XIPDIR)/DISAMB/disamb.xip \
		$(XIPDIR)/CHUNKING/chunker.xip \
		$(XIPDIR)/DEPENDENCIES/dependency.xip 

LEXDIR = $(XIPDIR)/FST2/data
LEXFILES = 	$(LEXDIR)/guess4ntm.fst \
		$(LEXDIR)/lex4ntm.fst \
		$(LEXDIR)/norm1.fst \
		$(LEXDIR)/norm2.fst \
		$(LEXDIR)/norm3.fst \
		$(LEXDIR)/ntm.txt \
		$(LEXDIR)/open4ntm.fst 


# ==========================================================================
# TARGETS

default : 	$(RES)/$(ID)_$(DEP).stat


diff:		$(AUX)/$(ID)_$(DEP).hand \
		$(RES)/$(ID)_$(DEP).xip
	@$(JDIFF) $^ &


stat:		$(RES)/$(ID)_$(DEP).stat
	@$(MORE) $(RES)/$(ID)_$(DEP).stat


NOW = $(shell date +%Y-%m-%d_%k%M)

# save statitics with date and time stamp
save:		$(RES)/$(ID)_$(DEP).xip \
		$(RES)/$(ID)_$(DEP).stat
	@cp $(RES)/$(ID)_$(DEP).xip $(RES)/$(ID)_$(DEP).xip.$(NOW) 
	@cp $(RES)/$(ID)_$(DEP).stat $(RES)/$(ID)_$(DEP).stat.$(NOW) 

# display the current time 
now:	
	@echo $(NOW)

# only run once to initialize the corpus for manual correction !

init:		$(RES)/$(ID)_$(DEP).xip
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $^ $(ID)_$(DEP).hand1



# ==========================================================================
# run parser
# ==========================================================================

$(AUX)/$(ID).xip:	$(ID).txt \
			$(GRAM) \
			$(GRMFILES) \
			$(LEXFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(XIP) -grm $(GRAM) -text $(ID).txt > $@


# ==========================================================================
# extract dependencies and sentence
# ==========================================================================
$(RES)/$(ID)_$(DEP).xip:	$(AUX)/$(ID).xip \
				def_eval_$(DEP).awk \
				filter_eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f def_eval_$(DEP).awk -f filter_eval.awk $< | $(SORT) > $@


# ==========================================================================
# re-sort manual annotated file
# ==========================================================================
$(AUX)/$(ID)_$(DEP).hand:	$(ID)_$(DEP).hand
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(SORT) $< > $@

# ==========================================================================
# compute statistics
# ==========================================================================
$(AUX)/$(ID)_$(DEP).xip0: $(RES)/$(ID)_$(DEP).xip
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GREP) ":dep" $^ > $@


$(AUX)/$(ID)_$(DEP).hand0: $(ID)_$(DEP).hand
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GREP) ":dep" $^ | $(SORT) > $@


$(RES)/$(ID)_$(DEP).stat:	$(AUX)/$(ID)_$(DEP).hand0 \
				$(AUX)/$(ID)_$(DEP).xip0 \
				eval.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(SDIFF) $(AUX)/$(ID)_$(DEP).hand0 $(AUX)/$(ID)_$(DEP).xip0 |\
	 $(GAWK) -f eval.awk > $@


