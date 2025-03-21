echo off

rem update JAVA_HOME to your environment
set XIP_HOME=%~dp0\..\..\..

if exist Test.class del Test.class
javac -classpath %XIP_HOME%\java\dist\jxip.jar Test.java 
java -mx128m  -classpath %XIP_HOME%\java\dist\jxip.jar;. Test %XIP_HOME%\grammar\english\norm\gram_norm_entit.grm input
if exist Test.class del Test.class
if exist errors.err del errors.err

