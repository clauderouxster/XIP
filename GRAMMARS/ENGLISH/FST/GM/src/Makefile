# ============================================================================
# CONTENT:	Makefile for lexical FSTs for
#
# LANGUAGE:	ENGLISH (temporary files for LITIGATION)
#
# AUTHOR:	A. Schiller
#
# CREATED:	20-Apr-2006
#
# ============================================================================

# ----------------------------------------------------------------------------
# DIRECTORIES:

# intermediate files
AUX = ../aux

# target files
DATA = ../data


# ----------------------------------------------------------------------------
# TOOLS:
FST = 		fst -q -e "set quit-on-fail on"
LOOKUP =	lookup -flags cn
DISAMB =	disamb -abi1+

# for compilation on Linux:
ECHO = echo -e

# for compilation on Solaris:
# ECHO = echo

# ===================================================================
# TARGET FILES:


DATAFILES =	$(DATA)/ntm1.txt \
		$(DATA)/ntm1_abs.txt \
		$(DATA)/ntm2.txt \
		$(DATA)/ntm2_abs.txt \
		$(DATA)/lex-GM.fst \
		$(DATA)/nvlex+GM.fst

# ============================================================================
# DEFAULT TARGET

default:	$(DATAFILES) 


clean:		cleanaux
		rm -f $(DATAFILES)


cleanaux:
		rm -f $(AUX)/*.fst
		rm -f $(AUX)/*.txt




### ==========================================================================
### DATA FILES
### ==========================================================================

# basic analyser
NTMDIR = $(HOME)/parSem/xip/dev/GRAMMARS/ENGLISH/NTMFILES
NVLEX = $(NTMDIR)/SOURCEFST/FORENTITIES/nvlex+context+add.fst


# ===================================================================
# ntm script with relative path name
# ===================================================================
$(DATA)/ntm%.txt: ntm%.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@cp $< $@

# ===================================================================
# ntm script with absolute path name
# ===================================================================

FSTDATA = $(PWD:%/src=%/data)

$(DATA)/ntm%_abs.txt: ntm%.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@gawk '{sub(/\$$FST/,"$(FSTDATA)");sub(/\$$NTMFILE/,"$(NTMDIR)");print}' < $< > $@


# ===================================================================
# special lexicons
# ===================================================================
$(AUX)/lex-%.fst:	lex-%.lexc \
			rules-new.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -utf8 -e "read lexc lex-$*.lexc" \
		-l rules-new.xfst \
		-e "save stack $@" -stop

$(DATA)/lex-%.fst:	$(AUX)/lex-%.fst \
			add_hmm.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -utf8 -e "define MARK 0 ;" \
		-s $(AUX)/lex-$*.fst \
		-l add_hmm.xfst \
		-e "save stack $@" -stop


# ===================================================================
# union of basic lexicon and special lexicon
# ===================================================================
$(DATA)/nvlex+%.fst:	$(NVLEX) \
			$(DATA)/lex-%.fst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(NVLEX) \
		-s $(DATA)/lex-$*.fst \
		-e "union net" \
		-e "save stack $@" -stop

