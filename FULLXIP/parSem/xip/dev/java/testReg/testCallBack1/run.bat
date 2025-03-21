echo on
del build\Test.class
set jars=../libExt/commons-io-2.0.1.jar;../../dist/jxip.jar
mkdir build
javac -cp %jars%  -d build CallBack1.java
#java -Xmx512m -cp %jars%;build testCallBack1.CallBack1 ../GRAMMARS/ENGLISH/GRMFILES/GRAM_NORM/gram_norm_entit.grm en1 any
#java -Xmx512m -cp %jars%;build testCallBack1.CallBack1 ../GRAMMARS/PORTUGUESE/GRMFILES/portuguese.grm por1 utf8
#java -Xmx512m -cp %jars%;build testCallBack1.CallBack1 ../GRAMMARS/ITALIAN/CELI_090313/italian.grm ita1 utf8
java -Xmx512m -cp %jars%;build testCallBack1.CallBack1 ../GRAMMARS/GERMAN/BASIC/german.grm ger1 utf8

