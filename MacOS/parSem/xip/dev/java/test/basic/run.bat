echo off

del Test.class

REM set GRAM=..\..\..\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen_entit.grm
set GRAM=..\..\..\GRAMMARS\ENGLISH\GRMFILES\GRAM_NORM\gram_norm_entit.grm
set JAR=..\..\dist\jxip.jar

javac -classpath %JAR% Test.java 
java  -classpath %JAR%;. Test %GRAM% "Peter is happy" 
del Test.class

