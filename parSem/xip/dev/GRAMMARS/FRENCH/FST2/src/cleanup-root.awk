# remove some entries from root lexicon
{
  if ($0~/\+Det/) {} 		# redefined in lex-func.lexc
  else if ($0~/\+Pron/) {} 	# redefined in lex-func.lexc
  else if ($0~/\+Conj/) {} 	# redefined in lex-func.lexc
  else if ($0~/\+Prep/) {} 	# redefined in lex-func.lexc
  else if ($1~/^que\+Adv/) {} 	# redefined in lex-func.lexc
  else if ($0~/\+Adj\+Card/) {} # redefined in numbers.xfst
  else if ($0~/\+Prop/) {} 	# redefined in lex-{prenom,lieux}.lexc
  else print
}
