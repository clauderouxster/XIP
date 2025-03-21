echo off

set XIP_HOME=%~dp0\..\..\..

set GRM=%XIP_HOME%\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen_entit.grm

if exist Test.class del Test.class
javac -classpath %XIP_HOME%\java\dist\jxip.jar Test.java 
java  -classpath %XIP_HOME%\java\dist\jxip.jar;. Test %GRM% input
if exist Test.class del Test.class

