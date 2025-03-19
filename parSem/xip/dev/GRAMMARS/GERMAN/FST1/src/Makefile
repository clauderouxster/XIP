# tools for Linux
FST = fst -utf8 -q -e "set quit-on-fail on"
ECHO = echo -e

GAWK	= gawk
CAT	= cat
GREP	= grep


DATA = ../data
AUX = ../aux


# ==========================================================================
# TARGETS
default:	tok lex guess norm lkp

tok:		$(DATA)/tok.fst

lex:		$(DATA)/html.fst \
		$(DATA)/lexicon.fst \
		$(DATA)/lexicon_seg.fst \

guess:		$(DATA)/guesser.fst \
		$(DATA)/openclass.fst

norm:		$(DATA)/norm-std.fst \
		$(DATA)/norm-allcap-std.fst \
		$(DATA)/norm-umlaut.fst \
		$(DATA)/rank.fst 

lkp:		$(DATA)/lookup.txt \
		$(DATA)/lookup_abs.txt \
		$(DATA)/lookup-seg.txt \
		$(DATA)/lookup-seg_abs.txt

query:		$(DATA)/tok.fst \
		$(DATA)/html.fst \
		$(DATA)/lexicon.fst \
		$(DATA)/lexicon_seg.fst \
		$(DATA)/guesser.fst \
		$(DATA)/openclass.fst \
		$(DATA)/norm-std.fst \
		$(DATA)/norm-query.fst \
		$(DATA)/rank.fst \
		$(DATA)/lookup-query-seg.txt \
		$(DATA)/lookup-query.txt \
		$(DATA)/lookup-query-seg_abs.txt \
		$(DATA)/lookup-query_abs.txt

# ==========================================================================
# tokenizer transducer 
# ==========================================================================
$(DATA)/tok.fst:		tok-abbr.xfst \
				tok-basic.xfst \
				tok-mwe.xfst \
				tok-stapler.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST)  -l tok-abbr.xfst \
		 -l tok-basic.xfst \
		 -l tok-mwe.xfst \
		 -l tok-stapler.xfst \
		 -e "optimize net" \
		 -e "save stack $@" -stop


# ==========================================================================
# lookup scripts
# ==========================================================================
$(DATA)/lookup.txt:		lookup.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $< $@


# lookup script with absolute path
FSTDIR = $(PWD:/src=/data)
$(DATA)/lookup_abs.txt:	lookup.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@


$(DATA)/lookup%.txt:		lookup%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@cp $< $@


# lookup script with absolute path
FSTDIR = $(PWD:/src=/data)
$(DATA)/lookup%_abs.txt:	lookup%.txt
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	gawk '{sub(/\$$FST/,"$(FSTDIR)");print}' < $< > $@


# ============================================================================
# Conversion of db-german.txt into lexc format (for basic analyzer)
# ============================================================================
DBGER =	lex-german.txt \
	prop-german.txt 
CONV =	convert_db.awk

$(AUX)/german0.lexc:	$(DBGER) $(CONV)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(ECHO) "LEXICON VerbStems" > $@
	@$(CAT) $(DBGER) |\
	$(GAWK) -F "[	]+" '{if ($$3~/V[^S]/) print}' |\
	$(GAWK) -f $(CONV) >> $@
	@$(ECHO) "LEXICON VerbPrefix1" >> $@
	@$(CAT) $(DBGER) |\
	$(GAWK) -F "[	]+" '{if ($$3~/VSep[01A]/) print}' |\
	$(GAWK) -f $(CONV) >> $@
	@$(ECHO) "LEXICON VerbPrefix2" >> $@
	@$(CAT) $(DBGER) |\
	$(GAWK) -F "[	]+" '{if ($$3~/VSep2/) print}' |\
	$(GAWK) -f $(CONV) >> $@
	@$(ECHO) "LEXICON SingleStems" >> $@
	@$(CAT) $(DBGER) |\
	$(GAWK) -F "[	]+" '{if ($$3=="XX") print}' |\
	$(GAWK) -f $(CONV) >> $@
	@$(ECHO) "LEXICON OtherStems" >> $@
	@$(CAT) $(DBGER) |\
	$(GAWK) -F "[	]+" '{if ($$3!~/^[VX]/) print}' |\
	$(GAWK) -f $(CONV) >> $@


# ============================================================================
# produce derived forms
# ============================================================================
$(AUX)/german_basic.lexc: $(AUX)/german0.lexc
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(GREP) -v "	}" $(AUX)/german0.lexc > $@

$(AUX)/german_deriv.lexc: $(AUX)/german0.lexc \
			prod-derivs.awk
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(GREP) "	}" $(AUX)/german0.lexc |\
	$(GAWK) -f prod-derivs.awk > $@


# ============================================================================
# Compilation of lexc files
# ============================================================================
LEXFILES1 = 	header.lexc \
		numerals.lexc \
		cont-deriv.lexc \
		flex-noun.lexc \
		flex-verb.lexc \
		flex-adj.lexc \
		$(AUX)/german0.lexc 

$(AUX)/germanA.lexc:	$(LEXFILES1)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(CAT) $(LEXFILES1) > $@


LEXFILES2 = 	header.lexc \
		numerals.lexc \
		flex-noun.lexc \
		flex-verb.lexc \
		flex-adj.lexc \
		$(AUX)/german_basic.lexc \
		$(AUX)/german_deriv.lexc 

$(AUX)/german.lexc:	$(LEXFILES2)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(CAT) $(LEXFILES2) > $@


$(AUX)/german.fst:	$(AUX)/german.lexc \
			rules-german.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "read lexc < $(AUX)/german.lexc" \
		-l rules-german.xfst \
		-e "save $@" -stop





# ============================================================================
# function words (and other partial lexicons)
# ============================================================================
$(AUX)/lex-%.fst:	lex-%.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l $^ \
		-e "save $@" -stop


# ============================================================================
# proper names (not part pf compounding!)
# ============================================================================

$(AUX)/prop-%.fst:	prop-%.lexc \
			rules-prop.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "read lexc prop-$*.lexc" \
		-l rules-prop.xfst \
		-e "save $@" -stop



# ============================================================================
# COMBINE LEXICONS
# ============================================================================
LEXALL = german prop-names lex-conj lex-prondet lex-prep lex-special

$(AUX)/lexicon.fst:	$(LEXALL:%=$(AUX)/%.fst)
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) $(LEXALL:%= -s $(AUX)/%.fst) \
		 -e "union net" \
		 -e "save $@" -stop

$(DATA)/lexicon.fst: 	$(AUX)/lexicon.fst \
		 	map4xip.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/lexicon.fst \
	        -l map4xip.xfst \
		-e "save $@" -stop

# ============================================================================
# LEXICON WITH SEGMENT MARKERS ONLY
# ============================================================================
$(AUX)/lexicon_seg.fst:	$(AUX)/lexicon.fst \
			compute-map.xfst \
			def-map.awk \
			def-subst.awk
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/lexicon.fst \
		-l compute-map.xfst \
		-e "save $@" -stop


$(DATA)/lexicon_seg.fst: 	$(AUX)/lexicon_seg.fst \
		 	map4xip.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/lexicon_seg.fst \
		-e "set minimal off" \
	        -l map4xip.xfst \
		-e "save $@" -stop


# ============================================================================
# Other simple FSTs
# ============================================================================
$(DATA)/%.fst:	%.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l $^ \
		-e "save $@" -stop

