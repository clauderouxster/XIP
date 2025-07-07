echo off

set JAR=..\..\dist\jxip.jar
del *.log
del TestLoadGram.class
javac -classpath %JAR% TestLoadGram.java
java  -classpath %JAR%;. TestLoadGram  
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
java  -classpath %JAR%;. TestLoadGram
del TestLoadGram.class

