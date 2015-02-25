# Check each day before that MX has sent us the file before 4pm. 
#
# Written by Adrian Dragulescu on 10/06/2011




#################################################################
# A vector of strings, with an "OK" element if everything is OK.
#
.check <- function(config)
{
  asOfDate <- Sys.Date()
  files <- list.files(config$dir)

  msg <- NULL
  
  if (length(files) != 0) {
    submitted_days <- gsub("MX_DEMANDBID_(.*)_NEISO.csv", "\\1", files)
    submitted_days <- as.Date(submitted_days, "%Y%m%d")
    submitted_days <- submitted_days[which(submitted_days > asOfDate)]

    daysAhead <- sort(as.numeric(submitted_days - asOfDate))
    if (2 %in% daysAhead) {  
      msg <- "Have file to submit tomorrow."
      dayOfWeek <- format(asOfDate, "%a")
      if (dayOfWeek == "Fri") {
        if (!(all(c(3,4) %in% daysAhead))) {
          wend <- if (!(3 %in% daysAhead)) "Sunday"
          wend <- if (!(4 %in% daysAhead)) paste(wend, "Monday", sep=" and ")
          msg <- c(msg, "But missing the file for", wend)
        } else {
          msg <- c(msg, "OK")
        }
      } else {
        msg <- c(msg, "OK")
      }
    } else {
      msg <- paste("No MX file to submit for", format(asOfDate+2))
    }
    
  } else {
    msg <- paste("No files to process in the folder", config$dir)
  }

  msg
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(CEGbase)
require(reshape)


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

zone_config <- list(
   nepool=list(dir="//ceg/nmarketprod/New England/auditpath/load"))

msg <- .check(zone_config[["nepool"]])

if ( !("OK" %in% msg) ) {
  to <- paste(c("necash@constellation.com", 
    "robert.etzel@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, "WARNING: problem with NEPOOL MX demand bids file", msg)  
}


rLog(paste("Done at ", Sys.time()))
q(status = returnCode)


