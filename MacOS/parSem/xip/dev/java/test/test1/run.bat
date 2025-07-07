echo on

del Test.class

set XIP_HOME=%~dp0\..\..\..

set GRM=%XIP_HOME%\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen_entit.grm

set JAR=..\..\dist\jxip.jar

javac -classpath %JAR% Test.java 
java  -classpath %JAR%;. Test %GRM% "Peter is happy" 
del Test.class

