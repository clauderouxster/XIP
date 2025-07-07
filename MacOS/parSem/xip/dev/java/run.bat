@echo on
set GRAM=..\GRAMMARS

set GRM=%GRAM%\ENGLISH\GRMFILES\GRAM_GEN\gram_gen_entit.grm
REM set GRM=%GRAM%\ENGLISH\APPLICATIONS\COREF\coref.grm
REM set GRM=%GRAM%\FRENCH\BASIC\french.grm

REM set CLASS=com.xerox.jxip.JXip
REM set CLASS=com.xerox.jxip.JXip2
REM set CLASS=com.xerox.jxip.RuleLoadedManager
set CLASS=com.xerox.jxip.XipResultManager

set SENT="Peter is happy. He is a nice boy."
REM set SENT="Je suis content."

set JAR=dist\jxip.jar
REM set JAR=dist\jxipPy.jar

java -cp %JAR% %CLASS% %GRM% %SENT%
