# Run this first
#

.R_ENV_CEG <- ""    # not exported


################################################################
# .RProfile should map to prod
map_area <- function(area="prod", verbose=TRUE)
{
  area <- tolower(area)

  #assign("R_ENV_CEG", area, pos="package:CEGbase")   
  assignInNamespace(".R_ENV_CEG", area, pos="package:CEGbase")   
  
  .Library.site <- .ENV_CEG[[area]]$SITE_LIB
  
  .libPaths(c(.Library.site, .ENV_CEG[[area]]$CRAN_LIB))

  if (verbose) {
    cat("Your current library path:\n")
    cat(paste(.libPaths(), collapse="\n"))

    cat("\nYour R_SCRIPT_ROOT is:", .ENV_CEG[[area]]$R_SCRIPT_ROOT, "\n\n")
  }
}

mapuser <- function(verbose=TRUE) {
  map_area(area="user", verbose=verbose)
}

mapdev <- function(verbose=TRUE) {
  map_area(area="dev", verbose=verbose)
}

mapprod <- function(verbose=TRUE) {
  map_area(area="prod", verbose=verbose)
}

#############################################################
# This is how you control the root of the R scripts
#
getroot <- function() {

  # when you call this function for the first time
  # read the area from env variable ARON_WRAP_AREA
  if (.R_ENV_CEG == "") {
    AWA <- tolower(Sys.getenv("ARON_WRAP_AREA"))

    area <- if (AWA == "") {
        "prod"    
      } else {
        x <- strsplit(AWA, "\\\\")[[1]]
        if (x[2] == "user") x[2] else x[3]  # can be one of "dev", "pre", "prod"
      }
    assignInNamespace(".R_ENV_CEG", area, pos="package:CEGbase")  
  }
  
  .ENV_CEG[[.R_ENV_CEG]]$R_SCRIPT_ROOT
}


