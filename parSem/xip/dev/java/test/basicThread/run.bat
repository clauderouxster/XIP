echo off
del Test.class
set JAR=..\..\dist\jxip.jar
javac -classpath %JAR% Test.java 
java  -classpath %JAR%;. Test 
del *.class

