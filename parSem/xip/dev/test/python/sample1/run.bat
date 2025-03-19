@echo on
set PYTHONPATH=..\..\..\lib\windows\sharedLibPy

copy ..\..\..\lib\windows\sharedLibPy\xips.dll xips.pyd

python  sample.py 

