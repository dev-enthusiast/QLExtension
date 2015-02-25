# Download operational announcements from NYISO
# http://www.nyiso.com/public/market_data/reports/operational_announcements.jsp
#
# Run every morning at 7:00AM
#
# Written by Adrian Dragulescu on 1-Sep-2009


############################################################################
# 
#
doDownload <- function(reportDate, outDir)
{
  reportDate <- as.Date(reportDate)
  rootURL <- "http://mis.nyiso.com/public/csv/OperMessages/"
  fullURL <- paste(rootURL, format(reportDate, "%Y%m%d"),
    "OperMessages.csv", sep="")
  con <- url(fullURL)
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error"){
    rLog("Cannot open url.  Exiting.")
    return(1)
  }
  res <- NULL
  res <- try( read.csv(con, sep=",", row.names=NULL) )
  if (class(res)[1]=="error"){
    rLog("Cannot close url.  Exiting.")   
    return(list(FALSE, NULL))
  }
  close(con)

  if (ncol(res)==3) {
    # something is wrong again with the report, it should have only 2 columns
    res[,2] <- paste(res[,2], res[,3])
    res <- res[,-3]
  }
  colnames(res) <- c("Insert.Time", "Message")
  
  fileout <- paste(outDir, "opAnnouncements_",
    format(reportDate, "%Y-%m"), ".csv", sep="")
  if (file.exists(fileout)){ 
    TT  <- read.csv(fileout)
    res <- rbind(TT, res)
  }
  write.csv(res, file=fileout, row.names=FALSE)
  
  rLog("Wrote", as.character(reportDate), "announcements to", fileout)

  return( list(TRUE, res) )  
}


filter_results <- function( x )
{
  # keep only the rows that have the word generator
  y <- x[grepl("Generator", x[,2]), ]
  
  y[,2]
}


############################################################################
# MAIN 

options(width=200)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
returnCode <- 0    # succeed = 0

outDir <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/",
  "NYPP/OpAnnouncements/", sep="")

asOfDate   <- Sys.Date()
reportDate <- asOfDate - 1

rLog("Download NYISO operational announcemnts for",
     as.character(reportDate))
rLog("Start at ", as.character(Sys.time()))

# usually only one day ... 
# reportDates <- reportDate:reportDate  
# for (d in as.character(reportDates))
res <- doDownload(reportDate, outDir)

#doArchive()

if (!res[[1]]) {
  returnCode <- 99
} else {
  rLog("Email results")
  to <- paste(c("adrian.dragulescu@constellation.com",
                "rujun.hu@constellation.com"
                ), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"

  out <- filter_results(res[[2]])
  
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to,
    paste("NYISO operational announcements for", reportDate),
    c(capture.output(out),
      "\n\nprocmon process: RMG/R/Reports/Energy/Trading/Congestion/download_NYISO_OpAnnouncements"))
}


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


