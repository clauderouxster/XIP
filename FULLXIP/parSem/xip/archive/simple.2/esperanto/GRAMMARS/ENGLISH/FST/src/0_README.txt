

Makefile		general Makefile

def-currencies.xfst	currrency names, e.g. euro
dig-currencies.xfst	currency symbols and amounts, e.g. 150,00€
dig-dates.xfst		date and time expressions 
dig-numbers.xfst	digital numerals
dig-units.xfst		measure units

morph-roots.txt		morphological analyser : root lexicon
morph-numbers.txt	numerals, eg. twenty-two
morph-suffixes.txt	affic lexicons (continuation classes)
morph-rules.xfst	morpho-phonological rules
morph-cleanup.xfst	final cleanup rules (addition of tagger specific entries, removal of some tokens)

lex-more.lexc		words to add to the morphological analyser
lex-business.lexc	names of business entities, organisations, etc.
lex-docs.lexc		names of documents, laws, ...
lex-events.lexc		event names
lex-names.lexc		person names (first names, last names, ...)
lex-places.lexc		geographic names and demonyms
lex-GM.lexc		special lexicon for gemneral Motors ("litigation" application)
lex-rules.xfst		morphophonological rules to apply with lex-XXX.lexc files

feat-trans.lexc		additional features for transitive verbs
feat-syn1.lexc		additional features for syntactic analysis
feat-ent1.lexc		additional feature for named entities
feat-sem1.lexc		additional experiencer features
feat-sem2.lexc		additional verb alternation features

add-features.xfst	script to add extra features to basic lexicon

# ----------------------------------------------------------------------------------------
# for usage with tokenize and lookup
guesser.xfst		morphological guesser
openclass.xfst		"last chance" guesser 
norm-std.xfst		normalizer for word initial caps
norm-all.xfst		normalizer for overall caps
lookup.txt		template for lookup script

# ----------------------------------------------------------------------------------------
# for usage with ntm
ntm-contexts.xfst	script to add contexts to lexicon FST
ntm-special.xfst	special tokens (E-mail, phone numbers, ...)
ntm-guesser.xfst	guesser for NTM
ntm-open.xfst		"last chance" guesser for NTM
ntm-norm.xfst		normalizer for NTM
ntm1.txt		template for ntm script
			
# ----------------------------------------------------------------------------------------
# construction of tokenizer FST

TOKENIZER/		subdirectory for tokenizer files
  tok-basic.xfst	basic segmentation rules
  tok-abbr.xfst		definition of abbreviations
  tok-mwe.xfst		definition of multi-words
  tok-stapler.xfst	compilation tool (DON'T MODIFY!)


# ----------------------------------------------------------------------------------------
# construction of HMM tables
HMM/			subdirectory for HMM files
  add-hmmtags.xfst	mapping for disambiguation tags
  add-int-tags.xfst	addition of internal tags (to differentiate ambiguity classes)
  amb-net.xfst		tool to compute ambiguity classes
  corpus_train.txt	training corpus
  corpus_dummy.txt	additional training coprus (for internal tags)
  corpus_test.txt	test corpus
  lookup4tagger.txt	lookup script for tagger training
  lookup4test.txt	lookup script for tagger testing
  norm4tagger.xfst	special normalizer for tagger tests
  hmm-eval.awk		script for tagger evaluation
  hmm-stat.awk		script for tagger statistics
  reformat_in.awk	workaround for format restrictions of lookup
  reformat_out.awk	workaround for format restrictions of lookup

