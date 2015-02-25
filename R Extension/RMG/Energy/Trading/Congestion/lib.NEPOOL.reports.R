# Library to parse official csv NEPOOL reports. 
# http://www.iso-ne.com/support/tech/rpt_descriptions/index.html
# from our CSV folders
#
# .archive_report
# .extract_accountNumber
# .extract_report_day
# .extract_report_version
# .filenames_by_settlement         collect filenames by settlement
# .get_report_csvfilenames
# .get_reportname                  from a filename
# .is_folder_zipped
# .load_archive
# .process_report                  one at a time
# .read_reports                    several at once
#
# get_ARRAWDSUM    - ARR awards summary report 
# get_FCMCLOSTLDTL - FCM capacity load obligation settlement details 
# get_MTHSTLICAP
# 
#

ACCOUNT_NUMBER <- structure(c("000050027", "000050017", "000050428"),
  names=c("CCG", "CNE", "EXGN"))


#######################################################################
# Archive report to an RData file.  Use convention over specification!
# always add the date, the timestamp, and the filename to the table.
# If the file already there it doesn't get archived again!
#
# @param filename a vector of filenames pointing to MIS reports
# @param tab which tab to pick
# @param AA pass the archive so you don't read it every time
# @param save if you want to save it
# @param minReportDay a Date.  All reports earlier than this date will be
#    chopped from the archive.
# @param company, if the filenames correspond to a given company, EXGEN or CNE   
# @param postfix, if you want to add a postfix to the filename
#
.archive_report <- function(filename, tab, AA=NULL, save=FALSE,
                            minReportDay=NULL, company="", postFix="")
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MIS/"
  reportName <- .get_reportname(filename[1])
  if (company != "")
    company <- paste(company, "_", sep="")
  archiveName <- paste(DIR, company, reportName, "_tab", tab, postFix, ".RData", sep="")

  if ( is.null(AA) )      
    AA <- .load_archive(reportName, tab, onlyLatest=FALSE)

  ind <- which(!(basename(filename) %in% unique(AA$filename)))
  if (length(ind)==0) {
    rLog("Archive up to date.")
    return(AA)               
  }

  fnames <- filename[ind]            # the files that need to get archived
  res <- .read_reports(fnames, tab)       

  if (is.null(res))
    rLog("Reports loaded are empty.")
  
  # may want to have a safe rbind here! in case report format changes
  AA <- rbind(AA, res)   

  if (!is.null(minReportDay)) {
    AA <- AA[AA$reportDay >= minReportDay,]
  }

  
  if (save && !is.null(res)) {
    save(AA, file=archiveName)
    rLog("Saved ", archiveName)
  }

  AA
}


#######################################################################
# Extract the accountNumber 
# fnames - a vector of filenames
#
.extract_accountNumber <- function(fnames)
{
  aux <- gsub(".*_([[:digit:]]+)_(.*)_(.*).CSV(.gz)?$", "\\1", basename(fnames))

  aux
}


#######################################################################
# Extract the day the report is for
# fnames - a vector of filenames
#
.extract_report_day <- function(fnames, asDates=TRUE)
{
  aux <- gsub(".*_(.*)_(.*).CSV(.gz)?$", "\\1", basename(fnames))
  if (asDates) {
    aux <- as.Date(substring(aux, 1, 8), format="%Y%m%d")
  }

  aux
}


#######################################################################
# Extract the report version (the datetime stamp)
# fnames - a vector of filenames
#
.extract_report_version <- function(fnames, asDates=FALSE)
{
  aux <- gsub(".*_([[:digit:]]+).CSV(.gz)?$", "\\1", basename(fnames))
  if (asDates) {
    aux <- as.Date(substring(aux, 1, 8), format="%Y%m%d")
  }

  aux
}


#######################################################################
# Split filenames by settlemetn (1st, 2nd, 3rd, etc.)
# @param fnames a vector of filenames
#
.filenames_by_settlement <- function( fnames, settlement=c("1st", "2nd",
                                                "3rd", "final") )
{
  reportName <- unique(.get_reportname( fnames ))
  if (length(reportName) != 1)
    stop("Only one report type is allowed!")

  reportDay <- .extract_report_day( fnames )

  aux <- lapply(split(fnames, reportDay), sort)
  maxSettlementPeriod <- max(sapply(aux, length))
  
  res <- if (settlement == "1st") {
    sapply(aux, "[[", 1)
  } else if (settlement == "2st") {
    sapply(aux, function(x) {
      switch(as.character(length(x)),
        "1" = x[1],
     #   "2" = x[2],
         x[2])    
    })
  } else if (settlement == "3st") {
    sapply(aux, function(x) {
      switch(as.character(length(x)),
        "1" = x[1],
        "2" = x[2],
        x[3])    
    })  
  } else if (settlement == "final") {
    sapply(aux, function(x) x[length(x)])
  } else {
    stop("Settlement ", settlement, " not supported!")
  }
  

  res
}


#######################################################################
# A filename factory.
# @param month, an R Date. NOT a vector.
# @param reportName the name of the report
# @param DIR a directory location, in case the files are not in the standard locations
# @return the path to requested reports.
# 
.get_report_csvfilenames <- function(org="CNE",
  month=as.Date("2010-06-01"), reportName="SD_FCMCLOSTLDTL",
  DIR=NULL)
{
  mth <- format(as.Date(month), "%Y%m")
  if (org == "CNE"){
    root <- "S:/Data/NEPOOL_CNE/RawData/OldZips/Rpt_"
  } else if (org == "CCG") {
    root <- "S:/Data/NEPOOL/RawData/OldZips/Rpt_"
  } else if (org == "EXGN") {
    root <- "S:/Data/NEPOOL_EXGN/RawData/OldZips/Rpt_"
  } else {
    if (is.null(DIR)) 
      stop("Argument org must be CCG, CNE or EXGN or you specify the DIR arg!")
  }
  
  pattern <- paste("^", reportName, sep="")
  if (is.null(DIR))
    DIR <- paste(root, mth, sep="")
  if ( .is_folder_zipped(DIR) ) {
    zfile <- list.files( DIR, pattern="zip$", full.names=TRUE)
    all.files <- unzip(zfile, list=TRUE)
    file <- paste(DIR, "/",
      all.files[grepl(pattern, all.files$Name), "Name"], sep="")
    if (file[1] == paste(DIR, "/", sep="")) file <- NULL  # it's not there!
  } else {   
    file <- sort(list.files( DIR, 
      pattern=paste("^", reportName, "_", sep=""), full.names=TRUE),
      decreasing=TRUE)
  }

  if (length(file)==0){
    rLog("No",  reportName, "report found in", DIR)
    return(NULL)
  } else {
    return(file)  # can return a vector of files if it's a daily rpt!
  }
}


#######################################################################
# Return the report name from a given filename
# 
.get_reportname <- function(filename)
{
  fname <-  basename(filename)
  # web reports have no account number info!
  ifelse(grepl("^WW_", fname),    
    gsub("(.*)_[[:digit:]]+_[[:digit:]]+.*", "\\1", fname),  
    gsub("(.*)_[[:digit:]]+_[[:digit:]]+_[[:digit:]]+.*", "\\1", fname)
  )
}


#######################################################################
# Check if the folder is already zipped
# 
.is_folder_zipped <- function(DIR)
{
  aux <- list.files(DIR)
  if (length(aux)==1 & grepl("zip$", basename(aux[1]))) TRUE else FALSE
}


#######################################################################
# load an archive from the share drive
# @param onlyLatest if you want to keep only the most recent version of
#    the report, set to TRUE
#
.load_archive <- function(reportName, tab, onlyLatest=FALSE)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MIS/"
  archiveName <- paste(DIR, reportName, "_tab", tab, ".RData", sep="")
  
  if (file.exists(archiveName)) {
    load(archiveName)      # load AA variable
    rLog("Loaded ", archiveName)
  } else {
    AA <- NULL
  }

  if (onlyLatest) {
    if ("accountNumber" %in% colnames(AA)) {
      AA <- ddply(AA, c("reportDay", "accountNumber"), function(x) {
        subset(x, reportTimestamp == max(x$reportTimestamp))
      })
    } else {           # web reports don't have accountNumber
      AA <- ddply(AA, c("reportDay"), function(x) {
        subset(x, reportTimestamp == max(x$reportTimestamp))})
    }
  }

  
  AA
}



#######################################################################
# Read one MIS report by splitting the report 
# into parts and reading each part into a data.frame.
# Return a list of data.frames, each data.frame for one report tab
#
.process_report <- function(filename, keepOnlyData=TRUE)
{
  on.exit(closeAllConnections())
  
  rLog("Reading ", filename)
  RR <- readLines(filename)
  
  indH <- grep("^\"H\"", RR)    # look for data headers 
  if (length(indH) %% 2 != 0)
    stop("Your report does not have even number of headers!")
  if ( length(indH)==0 ) {
    rLog("No headers!  Or data is not quoted! Exiting.")
    return(NULL)
  }
  
  # find number of report parts
  blocks <- length(indH)/2  
  res <- vector("list", length=length(blocks))
  
  indH <- indH[seq(1,length(indH),2)]   # pick the first line
  indH <- c(indH, length(RR))
  for (b in 1:blocks){
    ind <- indH[b]:(indH[b+1]-1)
    if (length(ind) > 1)         # remove the units, format, etc.
      ind <- ind[-2]       
    out <- read.csv( textConnection(RR[ind]) )
    if (keepOnlyData)
      out <- subset(out, H == "D")
    
    res[[b]] <- out
    #closeAllConnections()
  }

  # remove first column, it's always H, D, C!
  # res <- lapply(res, function(x){x <- x[-1,]})  
  
  return(res)
}


#######################################################################
# Read the filenames and return a data.frame
# @filenames a vector of filenames
# @tab the tab you want to read
# @return a data.frame
#
.read_reports <- function(fnames, tab)
{
  res <- vector("list", length=length(fnames))
  for (i in seq_len(length(res))) {
    aux <- .process_report(fnames[i])
    aa <- aux[[tab]]
    if (nrow(aa) == 0) next
    aa$reportDay <- .extract_report_day(fnames[i])
    aa$reportTimestamp <- .extract_report_version(fnames[i])
    if (!grepl("^WW_", basename(fnames[i])))    # web reports have no account number info!
      aa$accountNumber <- .extract_accountNumber(fnames[i])
    aa$filename <- basename(fnames[i])
    res[[i]] <- aa
  }
  res <- do.call(rbind.fill, res)  # need rbind.fill to protect the future!
  
  res
}


#######################################################################
# Report has 7 parts ....
#  - keep an index of parts to keep, e.g. c(1,3) to keep parts 1 and 3 
# 
get_FCMCLOSTLDTL <- function(filename, keep=NULL)
{
  res <- .process_report(filename)
  #lapply(res, head)
  #if (length(res) != 7) stop("Report format changed!")
  
  names(res) <- c("Pool", "CapacityZone", "Customer", "Resource",
    "CLO.Bilateral", "DailyPeakContribution", "MonthlyPeakContribution")
  
  if (!is.null(keep)) res <- res[[keep]]

  res
}


#######################################################################
# Report has 5 parts ....
#  - keep an index of parts to keep, e.g. c(1,3) to keep parts 1 and 3 
# 
# NOTE: This report has been discontinued since 2010-06-01
#   and replaced with sd_fcmclostdtl report
# 
get_MTHSTLICAP <- function(filename, keep=NULL)
{
  res <- .process_report(filename)
  #lapply(res, head)
  #if (length(res) != 4) stop("Report format changed!")
  
  names(res) <- c(
    "Monthly.UCAP.per.Generation.Asset.per.Participant",                   
    "Load.Response.Adjusted.Capacity.per.Participant",                   
    "Monthly.UCAP.Obligation.per.Load.Asset.per.Participant",
    "Daily.UCAP.Peak.Contribution.Value")
  
  if (!is.null(keep)) res <- res[[keep]]

  return(res)
}


#######################################################################
#######################################################################
#
.test <- function()
{
  require(CEGbase)
  require(plyr)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

  reportName <- "SD_FCMCLOSTLDTL"; tab <- 3
  reportName <- "SD_FCMCLOSTLDTL"; tab <- 2
  reportName <- "SD_FCMCLOSTLDTL"; tab <- 1
  reportName <- "SR_FCMNRCPSUM"; tab <- 2
  
  months <- seq(as.Date("2013-01-01"), nextMonth(), by="1 month")
  fnames <- unlist(sapply(months, function(m){
    c(.get_report_csvfilenames(org="CNE", month=m, reportName=reportName),
      .get_report_csvfilenames(org="CCG", month=m, reportName=reportName),
      .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName))
  }))

  
  reportName <- "SR_RSVCHARGE"; tab <- 6
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="CNE", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  reportName <- "SD_DACLEARED"; tab <- 1
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)


  reportName <- "SR_DANCPCSTLSUM"; tab <- 2
  months <- seq(as.Date("2013-01-01"), currentMonth(), by="1 month")
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="CNE", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  
  
  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  

  aux <- lapply(fnames, .process_report)
  res <- do.call(rbind, lapply(aux, "[[", 1))



  
  
  
}


## #######################################################################
## # Report has 7 parts ....
## #  - keep an index of parts to keep, e.g. c(1,3) to keep parts 1 and 3 
## # 
## get_ARRAWDSUM <- function(filename, keep=NULL)
## {
##   stop("NOT IMPLEMENTED YET!")
##   res <- .process_report(filename)
##   #lapply(res, head)
##   #if (length(res) != 7) stop("Report format changed!")
  
##   ## names(res) <- c("Pool", "CapacityZone", "Customer", "Resource",
##   ##   "CLO.Bilateral", "DailyPeakContribution", "MonthlyPeakContribution")
  
##   if (!is.null(keep)) res <- res[[keep]]

##   res
## }


