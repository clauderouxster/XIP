@echo on
del res
hostname >res
call runCmd.bat 1>>res
call run.bat 1>>res
cd versionc++
call run.bat 1>>res
