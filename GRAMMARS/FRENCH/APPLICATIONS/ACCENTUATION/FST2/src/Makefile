# -------------------------------------------------------------------------
# Xerox Research Center Europe - Grenoble
#
# Copyright (C)  Xerox Corporation
#
# -------------------------------------------------------------------------
# TYPE:		makefile 
# CONTENT:	creation of lexical FSTs
# LANGUAGE:	FRENCH
# AUTHOR:	Anne Schiller
# DATE:		15-Mar-2007
# -------------------------------------------------------------------------
# USAGE: 	gmake
# -------------------------------------------------------------------------

# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e

GAWK	= gawk
CAT	= cat

# directories
AUX 	= ../aux
DATA	= ../data

default:	$(DATA)/lex+mor.fst \
		$(DATA)/lex+syn.fst \
		$(DATA)/spec.fst \
		$(DATA)/guesser.fst \
		$(DATA)/openclass.fst \
		$(DATA)/norm1.fst \
		$(DATA)/norm2.fst \
		$(DATA)/norm3.fst


# source files

ODLEX =  	od_header.lexc \
		$(AUX)/roots1.lexc \
		roots2.lexc

# ==========================================================================
# modified root lexicon
# - removed function-words ( --> lex-func.lexc)

$(AUX)/roots1.lexc:	od_roots.lexc \
		cleanup-root.awk
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(GAWK) -f cleanup-root.awk od_roots.lexc > $@

# ==========================================================================
# combine source lexc file

$(AUX)/od_lex.all:	$(ODLEX)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(CAT) $(ODLEX) > $@


$(AUX)/lex-%.all:	flex-header.lexc \
			lex-%.lexc
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(CAT) $^  > $@


# ==========================================================================
# main lexicon

$(AUX)/od_lex+mor.fst:	$(AUX)/od_lex.all \
			od_rules.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc < $<"  \
		-l od_rules.xfst \
		-e "save stack $@" -stop

$(AUX)/od_lex+syn.fst:	$(AUX)/od_lex+mor.fst \
			Subcat.fst \
			AddSubcat.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-s Subcat.fst \
		-l AddSubcat.xfst \
		-e "save stack $@" -stop

# ==========================================================================
# entries to remove from
# ==========================================================================
# additional lexicons :
# - lex-comp.fst: 	compounds
# - lex-func.fst:	function words


$(AUX)/lex-%.fst:	$(AUX)/lex-%.all  \
			def-chars.xfst \
			flex-rules.xfst 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -e "read lexc < $<"  \
		-l def-chars.xfst \
		-l flex-rules.xfst \
		-e "save stack $@" -stop



# ==========================================================================

ADDLEX =	lex-comp lex-more lex-func lex-adv numbers units punct \
	 	lex-noms lex-prenoms lex-celeb lex-org \
		lex-france lex-pays lex-fleuves lex-lieux 

ADDFILES =	$(ADDLEX:%=$(AUX)/%.fst)

$(AUX)/new_lex.fst:	$(ADDFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(ADDFILES:%= -s %) \
		-e "union net" \
		-e "save stack $@" -stop

$(DATA)/lex%.fst:	$(AUX)/od_lex%.fst \
			$(AUX)/new_lex.fst \
			combine_lex.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(AUX)/od_lex$*.fst \
		-s $(AUX)/new_lex.fst \
		-l combine_lex.xfst \
		-e "save stack $@" -stop



# ==========================================================================
# Openclass guesser
$(DATA)/openclass.fst:	def-chars.xfst \
			openclass.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l def-chars.xfst \
		-l openclass.xfst \
		-e "save stack $@" -stop


# ==========================================================================
# normalizers
NORMID = 	space cap hyphen xml 
NORMFILES =	$(NORMID:%=$(AUX)/norm-%.fst)

$(DATA)/norm1.fst:	$(NORMFILES) 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(NORMFILES:%= -s %) \
		-e "union net" \
		-e "save stack $@" -stop

NORMID2 = 	space cap hyphen xml accent
NORMFILES2 =	$(NORMID2:%=$(AUX)/norm-%.fst)

$(DATA)/norm2.fst:	$(NORMFILES2) 
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) $(NORMFILES2:%= -s %) \
		-e "union net" \
		-e "save stack $@" -stop


# ==========================================================================
# OTHER
$(DATA)/%.fst:		%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $< \
		-e "save stack $@" -stop



$(AUX)/norm-%.fst:	def-chars.xfst \
			norm-%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l def-chars.xfst \
		-l norm-$*.xfst \
		-e "save stack $@" -stop


$(AUX)/%.fst:		%.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -l $< \
		-e "save stack $@" -stop


# ==========================================================================
# BASIC tests

test-%:		$(AUX)/lex-%.fst \
		test-lex.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $< \
		-l test-lex.xfst -stop
