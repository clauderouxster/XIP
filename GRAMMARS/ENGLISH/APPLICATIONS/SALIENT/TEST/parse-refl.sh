mv test-refl.parse test-refl-prev.parse

cat test-refl.txt | ../BIN_refl_json/txtparse | egrep "^SENT|^REFLSENT|^CONCEPT_REFL|^CONTEXT" > test-refl.parse

sdiff test-refl-prev.parse test-refl.parse > sdiff.refl

egrep "\||>|<" sdiff.refl

