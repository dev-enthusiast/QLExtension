rem Check directory paths for accuracy
rem
rem PATH=C:\Rtools\perl\bin;C:\Rtools\bin;S:\All\Risk\Software\R\R-2.8.1\bin;C:\Rtools\mingw\bin;S:\All\Risk\Software\MiKTeX 2.7\miktex\bin;C:\Program Files\HTML Help Workshop\
set MINGW_ROOT=C:\Rtools\mingw
set RHOME=S:\All\Risk\Software\R\R-2.8.1
Rcmd build --no-vignettes --force --binary --use-zip %1
