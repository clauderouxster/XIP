echo on
del build\Test.class
javac -cp ../../dist/jxip.jar  -d build Test.java
java -Xmx512m -cp ../../dist/jxip.jar;build Test ../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm inputs/input-en1 any

