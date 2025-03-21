echo on
del build\Test.class
javac -cp ../../dist/jxip.jar  -d build Test.java
java -Xmx128m -cp ../../dist/jxip.jar;build testGuesser.Test ../../../GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm input

