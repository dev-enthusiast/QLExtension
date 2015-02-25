#########################################################################
# Things to note about migration to R 3.1.2!
#
#
# To prepare a local copy of R as follows: 
#   - Copy the production version of R to C:/Software/R-3.1.2
#   - Create the folder C:/Software/prod_312/cran-library
#   - Create the folder C:/Software/prod_312/site-library
#   - Copy the contents of S:\All\Risk\Software\R\prod\cran-library\R-3.1.2 to 
#     C:/Software/prod_312/cran-library
#   - Copy the contents of S:\All\Risk\Software\R\prod\site-library\R-3.1.2 to 
#     C:/Software/prod_312/site-library
#   - edit the C:/Software/R-3.1.2/etc/Rprofile.site to point to the correct libs 
#        user312 = list(
#           R_SCRIPT_ROOT = "H:/user/",
#           R_SECDB_DLL = "I:/ja/prod/dllw32/",
#           SITE_LIB = "C:/Software/prod_312/site-library",
#           CRAN_LIB = "C:/Software/prod_312/cran-library"),
#
#
#
#



# check out unit_tests.R for things that broke
sessionInfo()
options()
.libPaths()


destdir <- "S:/All/Risk/Software/R/prod/cran-library/R-3.1.2"

install.packages("doParallel", destdir)
install.packages("snow", destdir)


packages <- c(
  'argparser',            
  'bitops', 'Cairo', 'chron', 'colorspace', 'corrgram', 
  'data.table', 'DBI', 'DEoptim', 'df2json', 
  'filehash',
  'GenSA', 'ggplot2', 'googleVis', 'gsubfn', 
  'hash', 'hashFunction', 'httr',
  'inline', 'iterators', 'itertools', 'JavaGD',
  'labeling', 'lpSolve', 
  'meboot',  'mvtnorm', 
  'outliers', 'odfWeave',
  'PerformanceAnalytics', 'plyr', 'proto', 'quantmod', 
  'RColorBrewer', 'Rcpp', 'RCurl', 'reshape', "reshape2", 'rJava',
  'rjson', 'RJSONIO', 'rmongodb', 'RODBC', 'RQuantLib', 'Rserve', 'RSQLite',
  'RUnit', 'sqldf',
  'TTR', 'tseries', 
  'xlsxjars', 'XML',
  'xtable', 'xts', 'yaml', 'zoo')

install.packages(packages, destdir)  


# for xlsx download the tar.gz file and then you have to move it after install
install.packages("xlsx_0.5.7.tar.gz", type="source")






##########################################################################
# to build and install SecDb
#
1) go to C:/temp and do
S:/All/Risk/Software/R/R-3.1.2/bin/i386/Rcmd build --force "H:/user/R/RMG/Utilities/Interfaces/SecDb3/SecDb/"
#S:/All/Risk/Software/R/R-3.1.2/bin/i386/Rcmd check SecDb_2.0.4.tar.gz

2) Open S:/All/Risk/Software/R/R-3.1.2/bin/i386/Rgui and run
install.packages("c:/temp/SecDb_2.0.4.tar.gz", repos=NULL, type="source",
                 clean=TRUE, INSTALL_opts=c("--no-multiarch"))
##########################################################################


# the issue with the new PACKAGE argument for .Call well explained ...
http://r.789695.n4.nabble.com/question-re-error-message-package-error-quot-functionName-quot-not-resolved-from-current-namespace-td4663892.html

library(CEGbase)
library(SecDb)

(dlls <- getLoadedDLLs())

# cannot find the dll??
getDLLRegisteredRoutines( "RSECDBInterface" )


# seems that it's in here
getNativeSymbolInfo("EvaluateSlang") # does it have a package info?

# a direct call works (because the symbol resolution tries harder)
.Call("EvaluateSlang", "1+1")

# the wrapper from the package doesn't work                                        
secdb.evaluateSlang("1+1")

# the recommended way fails (probably because the dll is not part of the package)
.Call("EvaluateSlang", "1+1", PACKAGE="RSecDbInterface")





















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



