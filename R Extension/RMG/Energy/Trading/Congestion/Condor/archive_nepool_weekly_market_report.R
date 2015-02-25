# Several steps:
# 1) Every week dowload the Nepool weekly market report
# 2) Save the pdf file in the archive
# 3) Read the summary page with read.pdf, and parse it
# 4) Archive the sumary in a RData file
# 5) Email the results
#
# Written by AAD on 07-Jan-2010

# .downloadReport(dirArchive)
# .grabLastSummaryPage()
# .getPublishedReportsLink()
# .processNewFiles(dirArchive, newFiles)
#
# To download all the reports from year 2008 run:
#   .downloadReport(dirArchive, year="2008") 
# To recreate the archive run:
#   .processNewFiles(dirArchive, list.files(dirArchive, full.names=TRUE), redo=TRUE)
#

#################################################################
# Download the missing weekly reports into the archive.   
# Report usually gets published on Fri with a Mon name  
#
.downloadReport <- function(asOfDate=Sys.Date(),
   dirArchive="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Reports/WeeklyMarketReport/Raw/" )
{
  while ( (weekdays(asOfDate) != "Friday") ) {
    asOfDate <- asOfDate - 1
  }
  rLog("Start with Fri", format(asOfDate))

  links <- .possible_links(asOfDate)    
  newFiles <- basename(links)

  downloadedFiles <- NULL
  for (f in seq_along(newFiles)) {
    rLog("Trying ", links[f])
    destfile <- paste(dirArchive, newFiles[f], sep="")
    if ( file.exists(destfile) )
      next
 
    # iso will return a text file with nonsense even if the file does not exist   
    try(download.file(links[f], destfile, quiet=FALSE, mode="wb", cacheOK=FALSE))
    
    if ( .isPDF( destfile ) ){
       rLog("Download OK")
       downloadedFiles <- c(downloadedFiles, destfile)
    } else {
      rLog("  --> ", newFiles[f], "is not a PDF file.  Removing it. ")
      unlink( destfile )
    }
  }

  
  return(downloadedFiles)
}



#################################################################
# Send this output in an email ...
#
.grabLastSummaryPage <- function(filename)
{
  out <- read.pdf(filename, first=2, last=3)
  out <- out[-which(out=="")]  # you have some empty lines in the new report
  
  res <- NULL
  token <- "Notable Events"
  try(res <- .extractIndentedToken(out, token))
  
  return(res)  
}



#################################################################
# Generate all the possible url links given a Friday.
# 
# @param fridayDate an R Date with the Friday the report may come out
# @return a vector of urls 
#
.possible_links <- function(fridayDate)
{
  base_url <- "http://www.iso-ne.com/static-assets/documents/"

  links <- NULL
  
  # check for every day up to two weeks
  for (i in seq(0, -13, by=-1)) {
    day <- fridayDate - i
    link <- paste(base_url, format(day, "%Y/%m"), "/we_",
                  format(day, "%Y_%m_%d_weekly.pdf"), sep="")
    links <- c(links, link)
    # or a revision
    link <- paste(base_url, format(day, "%Y/%m"), "/we_",
                  format(day, "%Y_%m_%d_weekly_rev1.pdf"), sep="")
    
    links <- c(links, link)
  }


  links
}



#################################################################
# find the previous Sunday
#
## .previous_sunday <- function(asOfDate)
## {
##   prevSun <- asOfDate
##   while( weekdays(prevSun) != "Sunday" ) {
##     prevSun <- prevSun - 1
##   }

##   prevSun
## }


#################################################################
# Add the summary from the new found files to the archive
# To recreate the archive, run it with all the available files.
#
#
.processNewFiles <- function(dirArchive, newFiles, redo=FALSE)
{
  fileName <- paste(dirArchive, "weekly_market_summary.csv", sep="")
  fileName <- gsub("/Raw", "", fileName)   # point to the right file
  if (redo){
    AA <- NULL
  } else {
    rLog("Load csv archive.")
    AA <- read.csv(fileName)
  }
  
  res <- NULL
  for (f in seq_along(newFiles)){
    res[[f]] <- .parse.PDF(newFiles[f], page=c(2,5))
  }
  res <- do.call(rbind, res)
  res$startDate <- as.character(res$startDate)
  res$endDate   <- as.character(res$endDate)
  
  AA <- rbind(AA, res)
  AA <- AA[order(AA$startDate, AA$event),]

  write.csv(AA, file=fileName, row.names=FALSE)
  rLog("Updated the archive.")

  res
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase); require(CEGterm); require(reshape)
source("H:/user/R/RMG/Utilities/read.pdf.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/nepool_weekly_market_report.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

# save the file in this directory
dirArchive <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
                    "Reports/WeeklyMarketReport/Raw/", sep="")


asOfDate <- Sys.Date()
rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")


rLog("Download missing reports to archive")
newFiles <- .downloadReport( asOfDate = asOfDate)
#newFiles <- rev(list.files(dirArchive, full.names=TRUE))[1]

if (!is.null(newFiles))
{
  res <- .processNewFiles(dirArchive, newFiles[length(newFiles)])

  out <- .grabLastSummaryPage(newFiles[length(newFiles)])
  out <- c(out,
    "\n\n DA Summary:\n", text_wrap(res[1, "comment"]),
    paste("\n\n\nPlease contact Adrian Dragulescu for questions regarding",
       "this procmon job."),
      paste("Procmon job:",
       "RMG/R/Reports/Energy/Trading/Congestion/archive_nepool_weekly_market_report"))
  
  rLog("Email last update")
  to <- paste(c("necash@constellation.com",
                "ang.li@constellation.com"), sep="", collapse=",")
  #to <-  "adrian.dragulescu@constellation.com"
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to, 
    paste("Nepool weekly market report for", as.character(asOfDate)), out)
  
}
rLog(paste("Done at ", Sys.time()))


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}







########################################################################################
#

## require(yaml)       # could use yaml but probably not!
## filename <- files[1]
## x <- out[26:47]
## yaml.load(paste(x, sep="", collapse="\n"))


#save(res, file="C:/Temp/nepool_weekly_report.RData")
#load("C:/Temp/nepool_weekly_report.RData")  
  
