# Utility to build ONE package from CVS.  
# 
# arg[1] - the package name as it appears in cvs, e.g. CEGbase
# arg[2] - the environment name, e.g. user
# arg[3] - the library name, e.g. SITE_LIB
#
# Example on how to use (need to set a local R installation see R.Release.Notes/R-3.1.2/README.R):
# C:/Software/R-3.1.2/bin/i386/Rscript H:/user/R/RMG/Utilities/Environment/build.package.R FTR --Renv user312
#


.test <- function()
{
  require(CEGbase)
  name <- "PM" # "Tsdb" "CRR" 
  
  .set_version(.get_cvs_location(name), version=NULL) # increment version

  source("H:/user/R/RMG/Utilities/Environment/lib.packages.ceg.R")
  .build_and_install_source(name, env="user215", lib="SITE_LIB", DIR="C:/temp/")

 
  #============================================================
  # to install the SecDb package to the local R installation
  # needs to be i386 only for now
  # make the tar.gz file first with
  # cd c:/temp
  # C:/Software/R-3.1.2/bin/i386/Rcmd build --force "H:/user/R/RMG/Utilities/Interfaces/SecDb3/SecDb"
  # C:/Software/R-3.1.2/bin/i386/Rcmd check SecDb_2.0.4.tar.gz  # need to fix it!
  
  install.packages("c:/temp/SecDb_2.0.4.tar.gz", repos=NULL, type="source",
                   clean=TRUE, INSTALL_opts=c("--no-multiarch"))
   
  #============================================================
  # to install/update a cran package for the local R installation
  # for further testing
  install.packages("akima", "C:/Software/prod_312/cran-library")
  

}

################################################################
################################################################
#
library(argparser)

source("H:/user/R/RMG/Utilities/Interfaces/CEGbase/R/RLogger.R")
source("H:/user/R/RMG/Utilities/Environment/lib.packages.ceg.R")

p <- arg.parser("build_pkg")
p <- add.argument(p, "name", help="name of the package")
p <- add.argument(p, "--Renv",
                  help="the R environment name for .ENV_CEG",
                  default="user215")
p <- add.argument(p, "--fromDir",
                  help="local Directory location for the package",
                  default="")



rLog("Starting the package build...")
rLog("Be patient as this may take a while depending on package size.")

argv <- parse.args(p)
print(argv)

version <- NULL          # increment the package version
cvs.location <- .get_cvs_location(argv$name)
.set_version(cvs.location, version) 


if (argv$fromDir == "") {
  fromDir <- .default_fromDir(argv$Renv)
} else {
  fromDir <- argv$fromDir
}

# sometimes I m
lib <- if (grepl("site-library", fromDir)){
  "SITE_LIB"
} else if (grepl("cran-library", fromDir)) {
  "CRAN_LIB"
}

.build_and_install_source(argv$name, env=argv$Renv, lib=lib)

rLog("Done!\n\n")

       





## if (length(cArgs)>(ind+1)){
##   version <- cArgs[ind+2]  # pass package version third
## } else {
##   version <- NULL
## }
