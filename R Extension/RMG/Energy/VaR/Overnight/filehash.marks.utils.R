# Various utilities to maintain the price and vol filehashes. 
#
#
#
#
#
#
# First version on 4-Aug-2007 by Adrian Dragulescu

source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/backupLoad.R")

#=================================================================
# Insert one day marks for one curve.name into the hash.
# Called by overnightPriceJobFilehash.R
# - hPP is the filehash environment
# - hP  is the marks you want to insert now (hPrices file)
#
insert.one.curve <- function(cname, db, hPP, hP, options){
  dirfilehash <- "//nas-msw-07/rmgapp/Prices/Historical"
  rLog(paste("Pushing ", cname, "...", sep=""))
  aux   <- subset(hP, curve.name==cname)
  curve <- tolower(cname)
  if (curve %in% ls(hPP)){
     fileName <- paste(dirfilehash, curve, sep="/")
     if (backupLoad(fileName, filehash=hPP)){
       CC <- hPP[[curve]]   
     } else {
       next        # skip for now ...
     }
 
    ucc <- unique(CC[,1:2])  # just in case things get awry
    CC  <- CC[rownames(ucc), ]
    rownames(CC) <- 1:nrow(CC)
    rTrace(paste("Dim of hPP[[curve]] = ", dim(CC), sep=""))
    ind <- grep(options$asOfDate, names(CC))
    if (length(ind)>0){CC <- CC[, -ind]}  # overwrite it!
    CC <- merge(CC, aux, all=TRUE)
    ind.Dates <- grep("-", names(CC))
    CC[,ind.Dates] <- CC[, sort(names(CC)[ind.Dates])] # sort them
    names(CC)[ind.Dates] <- sort(names(CC)[ind.Dates]) # relabel them
  } else {                 # it's a new curve ... just reshuffle
    CC <- aux[, c("curve.name", "contract.dt", as.character(options$asOfDate))]
  }
  res <- try(dbInsert(db, curve, CC))
  return(res)
}

#=================================================================
# Try to repair one curve in the filehash.  Snapshot can be "hourly.1",
# "nightly.1", "weekly.1", etc.
#
filehash.repair.curve <- function(db, hPP, curve.name, dirfilehash,
                                  snapshot="nightly.1"){
  repaired <- TRUE
 # curve.name <- "commod pwx offpeak"
  file <- paste(dirfilehash, "/", curve.name, sep="") 
  res  <- try(hP.fh <- hPP[[curve.name]]) # sometimes it gets stuck!
  if (class(res)=="try-error"){
    rLog("Get the backup from one night ago:")
    file.bk  <- paste(dirfilehash, "/~snapshot/", snapshot,
                      "/", curve.name, sep="")
    finfo.bk <- file.info(file.bk) 
    file.copy(file.bk, file, overwrite=TRUE)
   
    res  <- try(hP.fh <- hPP[[curve.name]]) # sometimes it gets stuck!
    if (class(res)=="try-error"){
      rLog("Could not repair with last night's copy.  Do better!")
      repaired <- FALSE
    }
  }
  return(repaired)
}


#=================================================================
# Inset the mkt.files from "dev" into the hash.  Fastest way to
# increase the hashfile.  Just a utility to increase the hash.
# You can make mkt files using the make.(range).mkt.files.R
#
insert.mkt.files.in.filehash <- function(){
  require(filehash)
  source("H:/user/R/RMG/Utilities/RLogger.R")  
  filehashOption(defaultType = "RDS")
  dirfilehash <- "R:/Prices/Historical"     # note: no ending forward slash!
  dbCreate(dirfilehash)
  db  <- dbInit(dirfilehash)
  hPP <- db2env(db) 
  uCurves.fh <- ls(hPP)
  
  mkt.dir <- "S:/All/Risk/Data/VaR/prod/Prices/Market/" # <---------------- !!!
#  mkt.files <- list.files(mkt.dir, pattern="all\\.FCO.*\\.Prices.RData", full.names=TRUE)
  mkt.files <- list.files(mkt.dir, pattern="all\\..*\\.Prices.RData", full.names=TRUE)
  for (file in mkt.files){                # loop over the market files
    load(file)
    cat("Inserting ", file, "\n")
    uCurves <- sort(unique(hP.mkt$curve.name))
    for (ind.c in 1:length(uCurves)){
      aux <- subset(hP.mkt, curve.name==uCurves[ind.c])  # what to insert
      ind <- grep("value", names(aux))                   # still shows up!
      if (length(ind)>0){aux <- aux[,-ind]}              # so remove it
      
      cname <- tolower(uCurves[ind.c])    # curve name
      if (cname %in% uCurves.fh){         # load it from filehash
        fileName <- paste(dirfilehash, cname, sep="/")
        if (backupLoad(fileName, filehash=hPP)){
          hP.fh <- hPP[[cname]]
        } else {                          # it's a new curve
          ind <- which(names(hP.fh) %in% names(aux))   # find if there is overlap
          ind <- ind[-grep("[[:alpha:]]", names(aux))] # drop the id variables
          if (length(ind)>0){hP.fh <- hP.fh[,-ind]}    # overwrite the dates
          aux <- merge(aux, hP.fh, all=T)
          ind <- grep("-", names(aux))
          aux[, ind] <- aux[, ind[order(names(aux)[ind])]] # sort columns ascending
          names(aux)[ind] <- sort(names(aux)[ind])         # label the columns right
        }
      }
      res <- try(dbInsert(db, cname, aux))
      if (class(res)=="try-error"){
        rLog(paste("Could not insert curve:",curve," into filehash\n",sep=""))
      }
    }
  } 
  cat("Done!\n")
}

################################################################################
# Clean the filehash. Some FCO curves were duplicated in the mkt files and have
# NA prices in the hash, with duplicated curve.names in lower case.
#
clean.filehash.duplicates <- function(pattern=" fco ")
{
  require(filehash)
  filehashOption(defaultType = "RDS")
  dirfilehash <- "R:/Prices/Historical"     # note: no ending forward slash!
  db  <- dbInit(dirfilehash)
  
  source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")
  hPP.env <- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")

  all.curves <- ls(hPP.env)
  curves <- grep(pattern, all.curves)
  for (ind.c in curves){
     curve <- all.curves[ind.c]
     rLog("Working on", curve)
     aux <- hPP.env[[curve]]

     ind.rm <- which((aux$curve.name != toupper(aux$curve.name)))
     if (length(ind.rm)>0){
       aux <- aux[-ind.rm, ]
       rownames(aux) <- NULL
       res <- try(dbInsert(db, curve, aux))
       if (class(res)=="try-error"){
         rLog(paste("Could not insert curve:",curve," back into filehash\n",sep=""))
       }       
     }    
  }
  

}



################################################################################
################################################################################
################################################################################
#=================================================================
# Use the temmplate.UT_PRICES_FILEHASH_HIST.dag to create the file 
# UT_PRICES_FILEHASH_HIST.dag, ready for dag submission.
# Where hdays is a vector of Dates.  Not used any more.
#
## make.UT_PRICES_FILEHASH_HIST.dag <- function(hdays){

## #  hdays <- seq(as.Date("2005-01-01"), as.Date("2006-11-20"), by="day")
##   dir.hPrices <- "S:/All/Risk/Data/VaR/prod/Prices/"
##   files <- list.files(dir.hPrices, pattern="^hPrices")
##   bDays <- strsplit(files, "\\.")   # official business days
##   bDays <- as.Date(sapply(bDays, function(x){x[2]}))

##   hdays <- hdays[which(hdays %in% bDays)]
##   hdays <- sort(hdays)
##   str <- paste(hdays, sep="", collapse="\n")
##   cat("Following days will be added to the dag:\n", str, "\n", sep="")
    
##   no.days <- length(hdays)   # no of hdays to push into hash
##   dir  <- "H:/user/R/RMG/Energy/VaR/Overnight/Condor/"
##   file <- paste(dir, "template.UT_PRICES_FILEHASH_HIST.dag", sep="")

##   FF <- readLines(file)
##   #-------------------------------------- add the jobs ---------------
##   ind <- grep("JOB", FF)
##   temp <- FF[ind]
##   for (d in 1:length(hdays)){
##     ltemp <- gsub("yyyy-mm-dd", hdays[d], temp)   # one line
##     FF <- append(FF, ltemp, after=(ind+d-1))
##   }
##   FF <- FF[-ind]  # remove the the template job
##   #-------------------------------------- add the vars ---------------
##   ind  <- grep("VARS", FF)
##   temp <- FF[ind]
##   for (d in 1:length(hdays)){
##     ltemp <- gsub("yyyy-mm-dd", hdays[d], temp)   # one line
##     FF <- append(FF, ltemp, after=(ind+d-1))
##   }
##   FF <- FF[-ind]  # remove the the template job
##   #-------------------------------------- add the parents ------------
##   ind  <- grep("PARENT", FF)
##   temp <- FF[ind]
##   for (d in 1:(length(hdays)-1)){
##     ltemp <- gsub("-day1", hdays[d], temp)      # one line
##     ltemp <- gsub("-day2", hdays[d+1], ltemp)   # one line
##     FF <- append(FF, ltemp, after=(ind+d-1))
##   }
##   FF <- FF[-ind]  # remove the the template job
  
##   dirout  <- "S:/All/Risk/Software/R/prod/Energy/VaR/Overnight/Condor/"
##   fileout <- paste(dirout, "UT_PRICES_FILEHASH_HIST.dag", sep="")
##   writeLines(FF, fileout)
  
##   cat("Wrote UT_PRICES_FILEHASH_HIST.dag to prod.\n")
## }


