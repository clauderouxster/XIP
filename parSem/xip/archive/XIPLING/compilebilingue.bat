cd C:\eclipse\XIPLING
c:\python27\python frenchfst.py
fst.exe -utf8 -f french.src
copy lingvatafrench.fst C:\eclipse\XIPLING\grammaire
c:\python27\python englishfst.py
fst.exe -utf8 -f english.src
copy lingvataenglish.fst C:\eclipse\XIPLING\grammaire
