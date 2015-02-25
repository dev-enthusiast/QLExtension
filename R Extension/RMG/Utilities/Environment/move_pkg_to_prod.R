# Workflow for package development:
#
# 
#
# 1) make package modifications
# 2) test changes, run test
# 3) install locally with Utilities/Environment/build.package.R
# 4) move to prod with Utilities/Environment/move_pkg_to_prod.R
#
# for example to move from your local area to the prod area
# S:/All/Risk/Software/R/R-3.1.2/bin/i386/Rscript H:/user/R/RMG/Utilities/Environment/move_pkg_to_prod.R FTR --Renv user312
#

library(argparser)

source("H:/user/R/RMG/Utilities/Interfaces/CEGbase/R/RLogger.R")
source("H:/user/R/RMG/Utilities/Environment/lib.packages.ceg.R")

p <- arg.parser("move_pkg")
p <- add.argument(p, "name", help="name of the package")
p <- add.argument(p, "--Renv",
                  help="the R environment name for .ENV_CEG",
                  default="user215")
p <- add.argument(p, "--fromDir",
                  help="local Directory location for the package",
                  default="")

argv <- parse.args(p);

if (!grepl("^S", commandArgs()[1]))
  stop("Use the production R version for moving packages to prod!")
#print(argv)

if (argv$fromDir == "") {
  fromDir <- .default_fromDir(argv$Renv)
} else {
  fromDir <- argv$fromDir
}

toDir <- if (grepl("site-library", fromDir)){
  .ENV_CEG[[argv$Renv]]$SITE_LIB
} else if (grepl("cran-library", fromDir)) {
  .ENV_CEG[[argv$Renv]]$CRAN_LIB
}

rLog("Moving package to ", toDir)

.move_pkg_to_prod(argv$name, fromDir, toDir)




################################################################
################################################################
.test <- function()
{
  name <- "CEGbase"
  fromDir <- "C:/Software/prod_312/site-library/R-3.1.2/"
  toDir <- "S:/All/Risk/Software/R/prod/site-library/R-3.1.2" 

  .move_pkg_to_prod(name, fromDir, toDir)
  

}
