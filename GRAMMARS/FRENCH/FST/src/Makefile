# ============================================================================
# CONTENT:	Makefile for lexical FSTs for French XIP Grammar
#
# LANGUAGE:	FRENCH
#
# AUTHOR:	A. Schiller
#
# CREATED:	31-Jan-2006
#		- reimplemented previous Makefile by J.P. Chanod
#		
# ============================================================================

# for compilation on Linux:
ECHO = echo -e

# for compilation on Solaris:
# ECHO = echo



# underlying morphological analyser
# BASELEX = /opt/Mltt/morph/french/INXIGHT5.0/data/FrenchSmall 
BASELEX = 	FrenchSmall

# transducer for adding subcat
SUBCATFST =	Aleth.subcat.fst


# Directory for intermediate data
AUX =	../aux

# directory for target files
DATA = 	../data

# TOOLS:
FST = 		/opt/ParSem/bin/fst-8.4.3/bin/linux/fst -q -e "set quit-on-fail on"

# ==============================================================================
# TARGET FILES
# ==============================================================================

DATAFILES =	$(DATA)/normxml.fst \
		$(DATA)/norm0.fst \
		$(DATA)/norm1.fst \
		$(DATA)/norm2.fst \
		$(DATA)/norm3.fst \
		$(DATA)/terrors.fst \
		$(DATA)/guesser.fst \
		$(DATA)/finalguesser.fst \
		$(DATA)/Basictok.fst \
		$(DATA)/Prefix2.fst \
		$(DATA)/num+.fst \
		$(DATA)/Email_URLs.fst

#		$(DATA)/sgml.fst

default:	$(DATAFILES) \
		$(DATA)/num-trans.fst \
		$(DATA)/Basictok+num.fst \
		$(DATA)/Basictok4Aladin.fst \
		$(DATA)/Basictok4Aladin+term.fst 


blog:		$(DATAFILES) \
		$(DATA)/Basictok4Blog.fst


clean:
		rm -f $(DATAFILES)
		rm -f $(AUX)/*.fst



# -------------------------------------------------------------------------------
# normalizers
# - norm1.fst
# - norm2.fst
# - norm3.fst
# - normxml.fst
# -------------------------------------------------------------------------------
$(DATA)/norm%.fst:	norm%.xfst \
			def-norm.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l def-norm.xfst \
		-l norm$*.xfst \
		-e "save $@" -stop	

# ==============================================================================
# basic tokens lexicon
# ==============================================================================
$(DATA)/Basictok.fst:	def-chars.utf8.xfst \
			$(AUX)/lex-Prefix.fst \
			$(AUX)/FrBasic.fst \
			Basictok.xfst \
			remove-lex.xfst \
			flexions.xfst \
			prenoms.xfst \
			lieux.xfst \
			org.xfst \
			celeb.xfst\
			add-lex.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST) -utf8	-l def-chars.utf8.xfst \
		-s $(AUX)/lex-Prefix.fst \
		-s $(AUX)/FrBasic.fst \
		-l Basictok.xfst \
		-l remove-lex.xfst \
		-l flexions.xfst \
		-l prenoms.xfst \
		-l lieux.xfst \
		-l org.xfst \
		-l celeb.xfst \
		-l add-lex.xfst \
		-e "save $@" -stop


# ==============================================================================
# prefix lexicon
# ==============================================================================
$(DATA)/Prefix2.fst:	def-chars.xfst \
			Prefix2.xfst \
			$(AUX)/lex-Prefix.fst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l def-chars.xfst \
		-s $(AUX)/lex-Prefix.fst \
		-l Prefix2.xfst \
		-e "save $@" -stop


# ==============================================================================
# numerals lexicon
# ==============================================================================
$(DATA)/num+.fst:	def-chars.xfst \
			num+.xfst 
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l def-chars.xfst \
		-l num+.xfst \
		-e "save $@" -stop

# ==============================================================================
# Urls and Email lexicon
# ==============================================================================
$(DATA)/Email_URLs.fst:	def-chars.xfst \
			Email_URLs.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l def-chars.xfst \
		-l Email_URLs.xfst \
		-e "save $@" -stop

# ==============================================================================
# other files (direct compilation of FST scripts)
# - guesser.fst
# - finalguesser.fst
# ==============================================================================
$(DATA)/%.fst:		%.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l $*.xfst \
		-e "save $@" -stop

# ==============================================================================
# stand alone sgml file
# ==============================================================================

$(DATA)/sgml.fst:	def-chars.xfst \
			sgml.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l def-chars.xfst \
		-l sgml.xfst \
		-e "save $@" -stop

################################################################################
# INTERMEDIATE FILES

# -------------------------------------------------------------------------------
# basic FST without flag diacritics
# -------------------------------------------------------------------------------
$(AUX)/FrenchSmallNoFlags.fst:	$(BASELEX) \
				RemoveFlags.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-s $(BASELEX) \
		-l RemoveFlags.xfst \
		-e "save $@" -stop

# -------------------------------------------------------------------------------
# basic FST with added syntactic information
# -------------------------------------------------------------------------------
$(AUX)/FrenchSmall+Syn.fst:	$(AUX)/FrenchSmallNoFlags.fst \
				$(SUBCATFST) \
				AddSubcat.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-s $(AUX)/FrenchSmallNoFlags.fst \
		-s $(SUBCATFST) \
		-l AddSubcat.xfst \
		-e "save $@" -stop



# -------------------------------------------------------------------------------
# filtered basic FST (with added HMM)
# -------------------------------------------------------------------------------
$(AUX)/FrBasic.fst:	$(AUX)/FrenchSmall+Syn.fst \
			$(AUX)/FilterLex.fst \
			$(AUX)/Map.fst \
			BasicBuild.xfst 
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-s $(AUX)/FrenchSmall+Syn.fst \
		-s $(AUX)/FilterLex.fst \
		-s $(AUX)/Map.fst \
		-l BasicBuild.xfst \
		-e "save $@" -stop


# -------------------------------------------------------------------------------
# mapping FST which adds part-of-speech tags to morphological information
# -------------------------------------------------------------------------------
$(AUX)/Map.fst:		MorTag.xfst \
			PosTag.xfst \
			Mapping.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l PosTag.xfst \
		-l MorTag.xfst \
		-l Mapping.xfst \
		-e "save $@" -stop


# -------------------------------------------------------------------------------
# special sub-lexicons
# -------------------------------------------------------------------------------
$(AUX)/lex-%.fst:	def-chars.xfst \
			lex-%.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l def-chars.xfst \
		-l lex-$*.xfst \
		-e "save $@" -stop


# -------------------------------------------------------------------------------
# transalted numbers lexicon
# -------------------------------------------------------------------------------
$(DATA)/num-trans.fst: 		numbers-trans.xfst \
				num4old.xfst \
				$(DATA)/Basictok.fst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l numbers-trans.xfst \
		-s $(DATA)/Basictok.fst \
		-l num4old.xfst \
		-e "save $@" -stop

$(DATA)/Basictok+num.fst:	$(DATA)/Basictok.fst \
				$(DATA)/num-trans.fst \
				cleanup-adv.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST) -utf8	-s $(DATA)/Basictok.fst \
		-s $(DATA)/num-trans.fst \
		-e "union net" \
		-l cleanup-adv.xfst \
		-e "save $@" -stop

$(DATA)/Basictok4Aladin.fst:	$(DATA)/Basictok+num.fst \
				abrev_med.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST) -s $(DATA)/Basictok+num.fst \
		-l abrev_med.xfst \
		-e "union net" \
		-e "save $@" -stop

$(DATA)/Basictok4Aladin+term.fst:  $(DATA)/Basictok4Aladin.fst \
				   term_med.xfst \
				   add-lex-term_med.xfst \
				   flexions.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST) -utf8 -s $(DATA)/Basictok4Aladin.fst \
		-l term_med.xfst \
		-e "union net" \
		-l flexions.xfst \
		-l add-lex-term_med.xfst \
		-e "save $@" -stop


# ==============================================================================
# Lexcicon for blog analysis:
# - map all first names to lower case
# place name also
# ==============================================================================
$(DATA)/Basictok4Blog.fst:	$(DATA)/Basictok+num.fst \
				lowercase-names.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST) -utf8 -s $(DATA)/Basictok.fst \
		-l lowercase-names.xfst \
		-e "save $@" -stop

# ==============================================================================
# other auxiliary files
# ==============================================================================
$(AUX)/%.fst:		%.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l $*.xfst \
		-e "save $@" -stop


