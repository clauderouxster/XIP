# ============================================================================
# CONTENT:	Makefile for lexical FSTs for
#
# LANGUAGE:	ENGLISH
#
# AUTHOR:	A. Schiller
#
# CREATED:	18-Jan-2006
#
# ============================================================================

# ----------------------------------------------------------------------------
# DIRECTORIES:

# intermediate files
AUX = ../aux

# target files
DATA = ../data

# source files for tokenizer
TOK = ./TOKENIZER

# source files for HMM training (and testing)
HMM = ./HMM

# source files for NTM specific additions
NTM = ./NTM


# ----------------------------------------------------------------------------
# TOOLS:
FST = 		fst -q -e "set quit-on-fail on"
LOOKUP =	lookup -flags cn
DISAMB =	disamb -abi1+

# for compilation on Linux:
HMMTRAIN = /opt/Xelda/buildXelda/build_1129/xelda/export/not-instrumented/ix86-linux2.4-gcc3.2/bin/XeldaHmmTrain.sh
ECHO = echo -e

# for compilation on Solaris:
# HMMTRAIN = /opt/Xelda/buildXelda/build_1129/xelda/export/not-instrumented/sparc-solaris8-forte6u2/bin/XeldaHmmTrain.sh
# ECHO = echo

# ===================================================================
# TARGET FILES:


DATAFILES =	$(DATA)/tok.fst \
		$(DATA)/lexicon+feat.fst \
		$(DATA)/guesser.fst \
		$(DATA)/openclass.fst \
		$(DATA)/norm-std.fst \
		$(DATA)/norm-all.fst \
		$(DATA)/lookup.txt \
		$(DATA)/lookup_abs.txt \
		$(DATA)/hmm.txt

NTMFILES =	$(DATA)/ntm-lexicon+feat.fst \
		$(DATA)/ntm-special.fst \
		$(DATA)/ntm-guesser.fst \
		$(DATA)/ntm-norm.fst \
		$(DATA)/ntm-open.fst \
		$(DATA)/ntm1.txt \
		$(DATA)/ntm1_abs.txt


# intermediate files for tagger constructions

# special lexicons (ambiguity classes only on lexical side
LEX4HMM =	$(AUX)/amb-lexicon.fst \
		$(AUX)/amb-guesser.fst \
		$(AUX)/amb-openclass.fst \
		$(AUX)/norm4tagger.fst \
		$(AUX)/lookup4tagger.txt \
		$(AUX)/lookup4test.txt

# training corpus:
TRAINCRP =	$(HMM)/corpus_train.txt \
		$(HMM)/corpus_dummy.txt

HMMFILES =	$(LEX4HMM) \
		$(AUX)/classes.txt \
		$(AUX)/tags.txt \
		$(AUX)/input_train.txt

# test HMM
TESTFILES =	$(AUX)/corpus_test.eval \
		$(AUX)/corpus_test.stat \
		$(AUX)/corpus_train.eval \
		$(AUX)/corpus_train.stat 

# ============================================================================
# DEFAULT TARGET

riskonly:	$(DATA)/lex4risk.fst 

default:	data ntm


clean:		cleanaux
		rm -f $(DATA)/%.fst
		rm -f $(DATA)/%.txt


cleanaux:
		rm -f $(AUX)/*.fst
		rm -f $(AUX)/*.txt


data:		$(DATAFILES) 

tagger:		$(LEX4HMM)

test:		$(TESTFILES)

ntm:		$(NTMFILES)


morph:		$(AUX)/morph-base.fst \
		$(AUX)/morph-spec.fst \
		$(AUX)/lex-business.fst \
		$(AUX)/lex-names.fst \
		$(AUX)/lex-places.fst \
		$(AUX)/lex-events.fst \
		$(AUX)/lex-docs.fst \
		$(AUX)/lex-more.fst \
		$(AUX)/morph.fst


### ==========================================================================
### DATA FILES
### ==========================================================================


# ===================================================================
# lookup with relative path name
# ===================================================================
$(DATA)/lookup.txt: lookup.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@cp $< $@

# ===================================================================
# lookup script with absolute path name (for usage with 'lookup')
# ===================================================================

FSTDATA = $(PWD:%/src=%/data)



$(DATA)/lookup_abs.txt: lookup.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@gawk '{sub(/\$$FST/,"$(FSTDATA)");print}' < $< > $@


# ===================================================================
# ntm script with relative path name
# ===================================================================
$(DATA)/ntm%.txt: ntm%.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@cp $< $@

# ===================================================================
# ntm script with absolute path name
# ===================================================================

OLDNTM = $(PWD:%/FST/src=%/NTMFILES)

$(DATA)/ntm_%_abs.txt: ntm_%.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@gawk '{sub(/\$$FST/,"$(FSTDATA)");sub(/\$$NTMFILE/,"$(OLDNTM)");print}' < $< > $@


$(DATA)/ntm%_abs.txt: ntm%.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@gawk '{sub(/\$$FST/,"$(FSTDATA)");print}' < $< > $@


# ===================================================================
# tokenizer FST
# - includes abbreviations
# - includes multi-words
# ===================================================================
$(DATA)/tok.fst:	$(TOK)/tok-basic.xfst \
			$(TOK)/tok-abbr.xfst \
			$(TOK)/tok-mwe.xfst \
			$(TOK)/tok-stapler.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l $(TOK)/tok-abbr.xfst \
		-l $(TOK)/tok-basic.xfst \
		-l $(TOK)/tok-mwe.xfst \
		-l $(TOK)/tok-stapler.xfst \
		-e "optimize net" \
		-e "save stack $@" -stop

# ===================================================================
# basic morphological analyzer
# ===================================================================
$(AUX)/morph-all.txt:	morph-roots.txt \
			morph-suffixes.txt
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@cat $+ > $@ 

$(AUX)/morph-all.fst:	$(AUX)/morph-all.txt
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "read lexc $(AUX)/morph-all.txt" \
		-e "save stack $@" -stop


$(AUX)/morph-base.fst:	$(AUX)/morph-all.fst \
			morph-rules.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/morph-all.fst \
		-l morph-rules.xfst \
		-e "save stack $@" -stop

# ===================================================================
# proper name lexicons
# ===================================================================
$(AUX)/lex-%.fst:	lex-%.lexc \
			lex-rules.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -utf8 -e "read lexc lex-$*.lexc" \
		-l lex-rules.xfst \
		-e "save stack $@" -stop


LEXSET = business places names events docs more
LEXSET1 = $(LEXSET:%=$(AUX)/lex-%.fst)

$(AUX)/prop-lex.fst: 	$(LEXSET1)
	@$(FST) $(LEXSET1:%= -s %) \
		-e "union net"\
		-e "save stack $@" -stop



MORDEF =	def-currencies.xfst \
		def-numbers.xfst \
		dig-units.xfst \
		dig-dates.xfst \
		dig-currencies.xfst \
		dig-numbers.xfst 


$(AUX)/morph-spec.fst:	$(MORDEF) 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -utf8 $(MORDEF:%= -l %) \
		-e "union net" \
		-e "save stack $@" -stop



# ===================================================================
# adapted morphological analyzer
# - clean up lexicon entries
# - add punctuations
# - add some SGML-tags
# ===================================================================
LEXMODFILES = lex_tagger.xfst

$(AUX)/morph.fst:	$(AUX)/morph-base.fst \
			morph-cleanup.xfst \
			morph-cleanup2.xfst \
			$(AUX)/morph-spec.fst \
			$(AUX)/prop-lex.fst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -utf8 -s $(AUX)/morph-base.fst \
		-l morph-cleanup.xfst \
		-s $(AUX)/morph-spec.fst \
		-s $(AUX)/prop-lex.fst \
		-e "union net" \
		-l morph-cleanup2.xfst \
		-e "save stack $@" -stop


# ===================================================================
# lexicon FST 
# - including HMM tags
# - including source marker "+lexicon"
# ===================================================================

$(DATA)/lexicon.fst:	$(AUX)/morph.fst \
			$(HMM)/add-hmmtags.xfst \
			$(HMM)/add-int-tags.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "define MARK %+lexicon ;" \
		-s $(AUX)/morph.fst \
		-l $(HMM)/add-hmmtags.xfst \
		-l $(HMM)/add-int-tags.xfst \
		-e "union net" \
		-e "save stack $@" -stop


# ===================================================================
# lexicon FST 
# - plus syntactic/semantic markers
# ===================================================================
ID = feat
FEATURES  = trans syn1 ent1

FEATFILES = $(FEATURES:%=feat-%.lexc) 
ADDFEAT = $(FEATFILES:%=-e "read lexc %" -l add-features.xfst )

$(DATA)/lexicon+$(ID).fst:	$(DATA)/lexicon.fst \
				add-features.xfst \
				$(FEATFILES)
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	@$(FST) -s $(DATA)/lexicon.fst \
		$(ADDFEAT) \
		-e "save stack $@" -stop


# ===================================================================
# lexicon FST  for NTM 
# - including contexts
# ===================================================================
$(DATA)/ntm-lexicon%.fst:	$(DATA)/lexicon%.fst \
				ntm-contexts.xfst
	@$(ECHO) "\n------------- updating $@ ------------------\n"
	$(FST) -s $(DATA)/lexicon$*.fst \
		-l ntm-contexts.xfst \
		-e "save stack $@" -stop


# ===================================================================
# guesser FST 
# - including HMM tags
# - including source marker "+guessed"
# ===================================================================

$(DATA)/guesser.fst:	guesser.xfst \
			$(HMM)/add-hmmtags.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "define MARK %+guesser ;" \
		-l guesser.xfst \
		-l $(HMM)/add-hmmtags.xfst \
	 	-e "optimize net" \
		-e "save stack $@" -stop


# ===================================================================
# other NTM specific FSTs
# ===================================================================
$(DATA)/ntm-%.fst:	ntm-%.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l ntm-$*.xfst \
		-e "save stack $@" -stop


# ===================================================================
# openclass guesser FST 
# - HMM tags only
# - including source marker "+open"
# ===================================================================

$(DATA)/openclass.fst:	openclass.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -e "define MARK %+open ;" \
		-l openclass.xfst \
		-e "save stack $@" -stop


# ===================================================================
# other FSTs 
# * norm-std.fst
# * norm-all.fst
# ===================================================================
$(DATA)/%.fst:	%.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l $*.xfst \
		-e "save stack $@" -stop


$(AUX)/%.fst:	%.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l $*.xfst \
		-e "save stack $@" -stop


# ===================================================================
# HMM table
# ===================================================================
$(DATA)/hmm.txt:	$(AUX)/input_train.txt \
			$(AUX)/classes.txt \
		 	$(AUX)/tags.txt
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(HMMTRAIN) -overwrite \
	 	  -verbose \
		  -output-data $@ \
		  -ambiguityclasses $(AUX)/classes.txt \
		  -taglist $(AUX)/tags.txt \
		  $(AUX)/input_train.txt



# ===================================================================
# FILES FOR HMM TRAINING ONLY:
# ===================================================================

# ===================================================================
# lookup script for tagger training
# - applies ambiguity FSTS instead of full analysis
# ===================================================================
FSTAUX = $(PWD:%/src=%/aux)
$(AUX)/lookup4tagger.txt: $(HMM)/lookup4tagger.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@gawk '{sub(/\$$FST/,"$(FSTAUX)");print}' < $< > $@

FST0 = $(PWD:%/src=%)

$(AUX)/lookup4test.txt: $(HMM)/lookup4test.txt
	@$(ECHO) "------------- updating $@ ------------------"
	@gawk '{sub(/\$$FST/,"$(FST0)");print}' < $< > $@


# ===================================================================
# normalizer for multi-column input
# ===================================================================
$(AUX)/norm4tagger.fst:	$(HMM)/norm4tagger.xfst
	@$(ECHO) "------------- updating $@ ------------------"
	@$(FST)	-l $< -e "save $@" -stop

# ===================================================================
# FST with ambiguity classes on lexical side
# ===================================================================
$(AUX)/amb-lexicon.fst:	$(DATA)/lexicon.fst \
			$(HMM)/amb-net.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(DATA)/lexicon.fst \
		-l $(HMM)/amb-net.xfst \
		-e "save stack $@" -stop

$(AUX)/amb-guesser.fst:	$(DATA)/guesser.fst \
			$(HMM)/amb-net.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(DATA)/guesser.fst \
		-l $(HMM)/amb-net.xfst \
		-e "save stack $@" -stop

$(AUX)/amb-openclass.fst:	openclass.xfst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -l openclass.xfst \
		-e "clear stack" \
		-e "read regex [ CLASS .x. ANY ];" \
		-e "save stack $@" -stop

# ===================================================================
# text file with ambiguity classes
# ===================================================================
$(AUX)/classes.txt \
$(AUX)/tags.txt:	$(AUX)/amb-lexicon.fst \
			$(AUX)/amb-guesser.fst \
			$(AUX)/amb-openclass.fst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/amb-lexicon.fst -e "upper-side" \
		-s $(AUX)/amb-guesser.fst -e "upper-side" \
		-s $(AUX)/amb-openclass.fst -e "upper-side" \
		-e "union net" \
		-e "set print-space ON" \
		-e "sort" \
		-e "print words | sort > $(AUX)/classes.txt" \
		-e "sigma net" \
		-e "sort" \
		-e "print words | sort > $(AUX)/tags.txt" -stop


# ===================================================================
# preprocessed corpus for HMM training
# ===================================================================
$(AUX)/input_train.txt:	$(TRAINCRP) \
			$(LEX4HMM) \
			$(HMM)/reformat_in.awk \
			$(HMM)/reformat_out.awk 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@cat $(TRAINCRP) |\
	 gawk -f $(HMM)/reformat_in.awk  |\
	 $(LOOKUP) -f $(AUX)/lookup4tagger.txt |\
	 gawk -f $(HMM)/reformat_out.awk > $@




# ===================================================================
# run tagger on test corpus
# ===================================================================
$(AUX)/corpus_%.tagged:	$(HMM)/corpus_%.txt \
			$(DATAFILES) \
			$(AUX)/lookup4test.txt \
			$(HMM)/reformat_in.awk \
			$(DATA)/hmm.txt
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@cat $(HMM)/corpus_$*.txt  |\
	 gawk -f $(HMM)/reformat_in.awk  |\
	 $(LOOKUP) -f $(AUX)/lookup4test.txt |\
	 $(DISAMB) $(DATA)/hmm.txt > $@


$(AUX)/corpus_%.eval:	$(AUX)/corpus_%.tagged \
			$(HMM)/hmm-eval.awk
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@cat $(AUX)/corpus_$*.tagged  |\
	 gawk -f $(HMM)/hmm-eval.awk > $@

$(AUX)/corpus_%.stat:	$(AUX)/corpus_%.eval \
			$(HMM)/hmm-stat.awk
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@cat $(AUX)/corpus_$*.eval  |\
	 gawk -f $(HMM)/hmm-stat.awk > $@

# ===================================================================
# (temporary) partial lexicons
# ===================================================================
$(DATA)/lex-%.fst:	$(AUX)/lex-%.fst \
			morph-cleanup2.xfst \
			$(HMM)/add-hmmtags.xfst 
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(AUX)/lex-$*.fst \
		-l morph-cleanup2.xfst \
		-e 'define MARK %+lexicon "+lex_$*";' \
		-l $(HMM)/add-hmmtags.xfst \
		-e "save stack $@" -stop


# ===================================================================
# (temporary) unioned lexicons for applications
# ===================================================================
OLDLEX = $(OLDNTM)/SOURCEFST/FORSEM/nvlex+context+add+sem+alter.fst

$(DATA)/lex4risk.fst:   $(OLDLEX) \
			$(DATA)/lex-locthales.fst \
			$(DATA)/lex-add_Risk.fst
	@$(ECHO) "--------------- updating $@ ----------------\n"
	@$(FST) -s $(OLDLEX) \
		-s $(DATA)/lex-locthales.fst \
		-s $(DATA)/lex-add_Risk.fst \
		-e union \
		-e "save $@" -stop


