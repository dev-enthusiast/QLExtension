rem Check directory paths for accuracy
rem
set PATH=C:\Program Files\IndigoPerl\perl\bin;C:\Rtools\bin;C:\Program Files\R\R-2.5.0\bin;C:\mingw\bin;C:\Program Files\MiKTeX 2.6\miktex\bin;C:\Program Files\HTML Help Workshop\
set MINGW_ROOT=C:\mingw
set RHOME=C:\Program Files\R\R-2.5.0
Rcmd build --no-vignettes --force --binary --use-zip %1
