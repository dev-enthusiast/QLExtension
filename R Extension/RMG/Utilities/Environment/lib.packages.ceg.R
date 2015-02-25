# Utility to build CEG packages from CVS.
#
# NOTE: I've spent +4h trying to make Rcmd INSTALL -l work with sources
#   instead of binaries.  I kept on getting an error about not beeing able
#   to create a 00LOCK folder.  Don't know why!
#
#
#
# Procmon calls R/Energy/Trading/Congestion/Condor/install_packages_ceg.R
#
# .build_and_install_binary
# .build_and_install_source
# .get_csv_location
# .get_version
# .move_pkg_to_prod
# .set_version
#


##################################################################
# Only for our packages
# @param name the name of the package (CVS folder)
# @param env the name of the environment to use
# @param lib
.build_and_install_source <- function(name, env="user215", lib="SITE_LIB",
  DIR="C:/Temp/")
{
  rversion <- R.Version()
  cvsLocation <- .get_cvs_location(name)
  pkgVersion  <- .get_version(cvsLocation)

  setwd(DIR) # maybe we should have it somewhere fixed... C:/Temp

  rLog("Building package in folder", normalizePath(DIR))
  out <- system(paste("Rcmd build --force", cvsLocation), invisible=TRUE)

  gzfile <- paste(name, "_", pkgVersion, ".tar.gz", sep="")
  if (!file.exists(gzfile)) {
    stop("Package gz file not created!")
  } else {
    rLog("File", gzfile, "created.\n\n")
  }


  rLog("Installing the package in environment=", env, "library=", lib)
  install.packages(gzfile, .ENV_CEG[[env]][[lib]], type="source",
                   INSTALL_opts="--no-lock")


  # try to load the package after installation
  library(name, character.only=TRUE)

  out   # returns 0 for success
}

##################################################################
# Copy one folder to another without the .svn dirs
#  fromDir <- "C:/Users/adrian/R/findataweb/temp/xlsx/trunk"
#  toDir <- "C:/Temporary/Downloads/xlsx"
#  .deepCopy(fromDir, toDir)
#
# file.copy(recursive=TRUE does not work! on windows?!)
#
.deepCopy <- function(fromDir, toDir, overwrite=TRUE)
{
  if ( !file.exists(toDir) )
    dir.create(toDir)

  if (file.info(fromDir)$isdir){
    fromFiles <- list.files(fromDir, full.names=TRUE)

    for (f in fromFiles){
      if (file.info(f)$isdir){
        toDir2 <- paste(toDir, basename(f), sep="/")
        dir.create(toDir2)
        .deepCopy(f, toDir2, overwrite=overwrite)
      } else {
        file.copy(f, toDir, overwrite=overwrite)
      }
    }
  } else {
    file.copy(fromDir, toDir)
  }
}


##################################################################
# Return a default directory for the local R installation
#
.default_fromDir <- function(Renv) {
  if (Renv == "user215") {
    "C:/Software/prod_215/site-library/R-2.15.1/"
  } else if (Renv == "user312") {
    "C:/Software/prod_312/site-library/"
  } else {
    stop("Don't know the Renv variable!")
  }
}


##################################################################
# only for our packages
# Works fine if I install on C:/Temp/Downloads or local, not on shared drive
# As a work around, I build the package in a local directory and move it to
# the desired location after.
# @param gzfile the gzip file
#
.install_source <- function(gzfile, env="user", lib="SITE_LIB",
  DIR="C:/temp/")
{
  setwd(DIR) # maybe we should have it somewhere fixed... C:/Temp

  rLog("Installing the package in environment=", env, "library=", lib)
  repo <- DIR                                 # temporary repository
  install.packages(gzfile, repo, type="source")

  # delete the tar.gz, rdx, etc. files
  #filesToDelete <- setdiff(list.files(tdir), name)
  #unlink(filesToDelete)
  stop()
  # location of the repository
  trueRepo <- .ENV_CEG[[env]][[lib]]  # cannot do it on S:/All anymore!

  name <- gsub("(.*)_(.*)", "\\1", basename(gzfile))
  # delete the old installation
  unlink(paste(trueRepo, name, sep="/"), recursive=TRUE, force=TRUE)

  # copy the directory to the true repo!
  file.copy(repo, trueRepo, recursive=TRUE)
  suppressWarnings(.deepCopy(repo, trueRepo))
  rLog("Done!")

  # remove the package folder in the temp directory
  unlink(name, recursive=TRUE, force=TRUE)

  # try to load the package after installation
  require(name, character.only=TRUE)

  out   # returns 0 for success
}


##################################################################
#
.get_cvs_location <- function(name)
{
  # you can customize it here
  repos <- matrix(c(
    "FTR",        "H:/user/R/RMG/Utilities/Interfaces/FTR/"
     ), ncol=2, byrow=TRUE)
  location <- repos[which(repos[,1]==name),2]

  # OR try the default location ...
  if (length(location)==0){
    location <- paste("H:/user/R/RMG/Utilities/Interfaces/", name,
                      "/", sep="")
  }

  return(location)
}

##################################################################
# get version from the DESCRIPTION file
#
.get_version <- function(cvsLocation)
{
  file <- paste(cvsLocation, "DESCRIPTION", sep="")
  DD  <- readLines(file)
  ind  <- grep("Version: ", DD)
  version <- strsplit(DD[ind], " +")[[1]][2]

  return(version)
}


##################################################################
# Called from script move_pkg_to_prod
# @param name the package name, e.g. Tsdb
# @param fromDir
# @param toDir
#
.move_pkg_to_prod <- function(name, fromDir, toDir)
{
  fname <- paste(toDir, name, sep="/")
  if ( file.exists(fname) ) {
    toFname <- paste(fname, "_old_", sep="")
    if (file.exists( toFname ))  # remove the old folder
      unlink( toFname, recursive=TRUE, force=TRUE )
    file.rename(fname, toFname)
  }

  fromFname <- paste(fromDir, name, sep="")
  rLog("Copy from", fromFname, "to", fname)
  .deepCopy(fromFname, fname)
  #unlink( toFname, recursive=TRUE, force=TRUE ) # remove the old folder

  rLog("Done!")
}


##################################################################
# version best be a numeric value, but it's not neccessary
#
.set_version <- function(cvs.location, version=NULL)
{
  file <- paste(cvs.location, "DESCRIPTION", sep="")
  DD  <- readLines(file)
  ind  <- grep("Version: ", DD)
  aux <- strsplit(DD[ind], " +")[[1]]

  if (is.null(version)){   # increase by one
    vSplit    <- strsplit(aux[2], "\\.")[[1]]
    vSplit[3] <- as.character(as.numeric(vSplit[3])+1)
    version <- paste(vSplit, sep="", collapse=".")
  }
  DD[ind] <- paste(aux[1], version)

  ind <- grep("Date: ", DD)
  aux <- strsplit(DD[ind], " ")[[1]]
  DD[ind] <- paste(aux[1], Sys.Date())

  writeLines(DD, con=file)
  invisible()
}



##################################################################
##################################################################
.main <- function()
{
  require(CEGbase)

  source("H:/user/R/RMG/Utilities/Environment/lib.packages.ceg.R")

  # install new version in C:/temp and move it by hand!
  install.packages("C:/temp/downloads/xlsxjars_0.5.0.tar.gz",
                   "C:/temp/", type="source")
  install.packages("C:/temp/downloads/xlsx_0.5.2.tar.gz",
                   "C:/temp/", type="source")



  .install_source("C:/temp/downloads/xlsxjars_0.5.0.tar.gz", env="user215")
  .install_source("PM_0.1.182.tar.gz", env="user312")




  name <- "PM"

  .build_and_install_source(name, lib=.ENV_CEG[["user"]]$SITE_LIB)  # FAILS!

  .build_and_install_source(name, lib="C:/Temp/")  # WORKS!

  # for xlsx, you need to install locally and then move by hand!
  install.packages("C:/Temp/PM_0.1.182.tar.gz", "C:/Temp/",
                   type="source")


  .build_and_install_binary(name, lib=ENV_CEG[["user"]]$SITE_LIB)

}





  ## cmd <- paste(Rcmd, "INSTALL -l", shQuote("C:/Temp/Downloads"), shQuote(zipfile))
  ## system(cmd)


  ## rLog("Removing the old installation ...")
  ## dir <- list.files(ENV_CEG[[area]]$SITE_LIB, paste("^", name, sep=""), full.names=T)
  ## unlink(dir)
  ## rLog("Done!\n\n")

  ## rLog("Moving the created zip file ...")
  ## file.copy(zipfile, paste("S:/All/Risk/Software/R/archive/",zipfile,sep=""))
  ## unlink(zipfile)
  ## rLog("Done!\n\n")

  ## Rcmd <<- '"S:/All/Risk/Software/R/R-2.12.1/bin/i386/Rcmd"'
  ## command <- paste(Rcmd, "INSTALL", cvsLocation)







  ## cArgs <- commandArgs()
  ## ind <- max(grep("^-", cArgs))
  ## print(cArgs)
  ## if (length(cArgs)>ind){
  ##   name <- cArgs[ind+1]  # pass package name first
  ## } else {
  ##   stop("Need to provide a package name!")
  ## }

  ## if (length(cArgs)>(ind+1)){
  ##   version <- cArgs[ind+2]  # pass package version second
  ## } else {
  ##   version <- NULL
  ## }



  ## dyn.load("RSecDbInterface.dll")

  ## library.dynam("RSecDbInterface.dll")

  ## secdbDllPath <- system("which RSecDbInterface.dll")


## ##################################################################
## # only for our packages
## #
.build_and_install_binary <- function(name, lib=ENV_CEG[["user"]]$SITE_LIB)
{
    cvsLocation <- .get_cvs_location(name)
    pkgVersion  <- .get_version(cvsLocation)

    setwd(tempdir()) # maybe we should have it somewhere fixed...
                                        #setwd("C:/Temp")

    rLog("Build package")
                                        #Rcmd <- '"S:/All/Risk/Software/R/R-2.12.1/bin/i386/Rcmd"'
    out <- system(paste("Rcmd build --force --binary", cvsLocation), invisible=TRUE)
    rLog("Done!\n\n")

    rLog("Installing the package ...")
    zipfile <- paste(name, "_", pkgVersion, ".zip", sep="")
    if (!file.exists(zipfile))
        stop("Package zip file not created.")

    install.packages(paste(name, "_", pkgVersion, ".zip", sep=""),
                     lib=lib, contriburl=NULL)

    out   # returns 0 for success
}

