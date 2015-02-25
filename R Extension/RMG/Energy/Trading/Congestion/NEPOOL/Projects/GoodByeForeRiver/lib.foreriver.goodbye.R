#  Deal with FR reports 
#
#
#


#################################################################
#
#
.get_reports_FR <- function(month=as.Date("2014-09-01"))
{
  reportName <- ".*"
  fnames <- .get_report_csvfilenames(org="EXGN", month=month, reportName=reportName)
  fnames <- sort(fnames)

  res <- NULL
  for (fname in fnames){
    rLog("Working on ", fname)
    if ( file.info(fname)$isdir )
      next
    aux <- readLines(fname)
    if (any(grepl("FORE RIVER", aux))) {
      res <- c(res, fname)
    }
  }

  reports <- unique(.get_reportname(res))

  reports    
}

#######################################################################
# Read a MIS report and keep all the headers and
# all the lines that have FR in them
# 
# Save to a file
#
.process_report_FR <- function( filename, OUTDIR )
{
  rLog("Reading ", filename)
  RR <- readLines(filename)

  # headers, comments, or termination
  indHCT <- grep("^(\"H\"|\"C\"|\"T\")", RR)
  indFR  <- grep("FORE RIVER", RR)
  indED  <- grep("UN.EDGAR", RR)
  indId  <- grep(",4032(7|8),", RR)

  if ( length(c(indFR, indED, indId)) == 0 )
    return(NULL)

  out <- RR[unique(sort(c(indHCT, indFR, indED, indId)))]

  newFilename <- paste(OUTDIR, gsub("\\.CSV$", "_FR\\.CSV", basename(filename)), sep="/")

  writeLines(out, con=newFilename) 
}



#################################################################
#################################################################
# 
.test <- function()
{
  require(CEGbase)
  require(reshape)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Projects/GoodByeForeRiver/lib.foreriver.goodbye.R")

  month <- as.Date("2014-10-01")
  transferDate <- as.Date("2014-10-25")
  
  fnames <- list.files(paste("S:/Data/NEPOOL_EXGN/RawData/OldZips/Rpt_",
                             format(month, "%Y%m"), sep=""),
                             full.name=TRUE, pattern="\\.CSV$")
  reportDay <- .extract_report_day(fnames)





  # reports I thought should be sent
  #reports <- .get_reports_FR(month=as.Date("2014-09-01"))
  reports <- c("SD_DAUNITOP", "SD_FCMNSCDTL", "SD_FCMRESSTLDTL", "SD_REGDTLASM",
               "SD_RSVASTDTL", "SD_RTLOAD", "SD_RTUNITASM", "SD_RTUNITOP", "SD_VARCCPMT",
               "SR_FCMAVAILSUM") 

  
  #reportName <- "SD_RTLOAD"


  month <- as.Date("2014-10-01")  
  for (reportName in reports) {
    rLog("Working on report", reportName)
    fnames <- .get_report_csvfilenames(org="EXGN", month=month, reportName=reportName)

    lapply(fnames, function(filename){
      .process_report_FR( filename, month )
    })
  }

  
}

