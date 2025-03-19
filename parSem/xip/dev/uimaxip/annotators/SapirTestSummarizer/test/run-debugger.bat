cd C:\EclipseWorkspace\SapirTestSummarizer
setlocal
call "%UIMA_HOME%\bin\setUimaClassPath"
set GRM_PATHNAME="H:\parSem\xip\dev\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN"
set GRAMMAR_FILE="H:\parSem\xip\dev\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen.grm"
if "%JAVA_HOME%"=="" (set UIMA_JAVA_CALL=java) else (set UIMA_JAVA_CALL=%JAVA_HOME%\bin\java)
"%UIMA_JAVA_CALL%" -cp "%UIMA_CLASSPATH%" "-Dcom.xerox.xrce.uimaxip.core.grammar.grmfile=%GRAMMAR_FILE%" "-Duima.home=%UIMA_HOME%" "-Duima.datapath=%UIMA_DATAPATH%" -DVNS_HOST=%VNS_HOST% -DVNS_PORT=%VNS_PORT% "-Djava.util.logging.config.file=%UIMA_LOGGER_CONFIG_FILE%" %UIMA_JVM_OPTS% org.apache.uima.tools.cvd.CVD
C:
"%UIMA_JAVA_CALL%" -cp "%UIMA_CLASSPATH%" %UIMA_JVM_OPTS% "-Djava.util.logging.config.file=%UIMA_LOGGER_CONFIG_FILE%" "-Duima.home=%UIMA_HOME%" "-Duima.tools.cvd.manpath=%UIMA_HOME%/docs/html" "-Duima.datapath=%UIMA_DATAPATH%" -DVNS_HOST=%VNS_HOST% -DVNS_PORT=%VNS_PORT% org.apache.uima.tools.cvd.CVD %*
