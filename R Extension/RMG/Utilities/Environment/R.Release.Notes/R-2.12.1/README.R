


#########################################################################
# Things to note about migration to R 2.12.1!
#
#

"
1) Need to copy the 
   R/RMG/Utilities/Environment/R/Release.Notes/R-2.12.1/Rprofile.site 
   file to R/R-2.12.1/etc/

2) Copy the R.css file to S:\All\Risk\Software\R\R-2.12.1\doc\html


  Noted that


"





# check out unit_tests.R for things that broke
sessionInfo()
options()
.libPaths()


destdir <- paste("S:/All/Structured Risk/NEPOOL/Software/",
  "R/packages/sources/R-2.12.1", sep="")


packages <- c(
  'bitops', 'Cairo', 'chron', 'colorspace', 'data.table', 'DBI',
  'Defaults', 'filehash', 'googlebigquery', 'googleVis', 'gsubfn',
  'hash', 'inline', 'iterators', 'itertools', 'JavaGD', 'labeling',
  'outliers', 'plyr', 'proto', 'quantmod', 'RAutoGenRunTime',
  'RColorBrewer', 'Rcpp', 'RCurl', 'reshape', 'Rffi', 'rJava',
  'rjson', 'RJSONIO', 'RODBC', 'RQuantLib', 'Rserve', 'RSQLite',
  'RSQLite.extfuns', 'RUnit', 'sqldf', 'TTR', 'xlsxjars', 'XML',
  'xtable', 'xts', 'yaml', 'zoo')


#packages <- dir("S:/All/Risk/Software/R/prod/cran-library/R-2.12.1/")
#paste("'", paste(sort(packages), collapse="', '"), "'", sep="")

download.packages(packages, destdir, type="source")

# you need to check package dependencies ... when adding new packages






















################################################################
# Installing 3rd party packages
#
lib <- "S:/All/Risk/Software/R/prod/cran-library/R-2.12.1"
install.packages("outliers", lib=lib)
install.packages("Cairo", lib=lib)
install.packages("reshape", lib=lib)
install.packages("labeling", lib=lib)
install.packages(c("xlsxjars", "xlsx"), lib=lib)
install.packages("JavaGD", lib=lib)
install.packages("googleVis", lib=lib)
install.packages("googlebigquery", lib=lib)
install.packages("data.table", lib=lib)
install.packages("inline", lib=lib)
install.packages("Rffi", lib=lib, repos="http://www.omegahat.org/R")

# cannot move it 
Rcmd INSTALL -l C:/Temp  "S:/All/Structured Risk/NEPOOL/Software/R/packages/sources/R-2.12.1/XMLSchema_0.7-0.tar.gz"

Rcmd INSTALL -l C:/Temp  "C:/Temp/Downloads/SSOAP_0.9-0.tar.gz"

install.packages(c(
  "colorspace", "filehash", "odfWeave", "quantmod",
  "RColorBrewer", "rJava", "RQuantLib", "Rserve", 
  "RODBC", "zoo", "xtable", "hash", "proto", "rjson", 
  "RUnit", "sqldf", "TTR", "Rcpp", "XML", "xts", "yaml",
  "zoo"), lib=lib)


################################################################
# the in house packages; will get installed in the site-library
# at "S:/All/Risk/Software/R/prod/site-library/R-2.12.1"
cmd <- paste("S:/All/Risk/Software/R/R-2.12.1/bin/i386/Rscript",
  "H:/user/R/RMG/Utilities/Environment/build.package.R")
system(paste(cmd, "CEGterm"))
system(paste(cmd, "DivideAndConquer"))
system(paste(cmd, "FTR"))
system(paste(cmd, "PM"))




## test_dllpath <- function()
## {
##   print(Sys.which("RSecDbInterface.dll"))

##   Sys.setenv(path=paste("Y:/bld/dev/dllw32", Sys.getenv("path"), sep=";"))
##   print(Sys.which("RSecDbInterface.dll"))
## }



