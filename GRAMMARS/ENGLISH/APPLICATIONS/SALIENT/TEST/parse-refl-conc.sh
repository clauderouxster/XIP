mv test-refl-conc.parse test-refl-conc.parse-prev

cat test-refl-conc.txt | /home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SALIENT/BIN_refl_conc/txtparse | egrep "^SENT|^REFLCONC|> " > test-refl-conc.parse

sdiff test-refl-conc.parse-prev test-refl-conc.parse > sdiff.all

sdiff test-refl-conc.parse-prev test-refl-conc.parse | egrep "\|"

