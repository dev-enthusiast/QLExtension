# Download any new ISO files with the summary of capacity auctions
# Send the MRA to SecDb
# Run it every week to see if there are new files to process 
#
#


##############################################################
#
.send_mra_secdb <- function( x )
{
  uMRA <- unique(subset(x[,c("auction", "month")], auction=="MRA"))

  TT <- NULL
  for (m in 1:nrow(uMRA)) {
    xs <- merge(x, uMRA[m,])
    aux <- subset(xs, section == "Capacity Zone" &
      column %in% c("Clearing Price ($/kW-month)", "Capacity Zone Name"))
    bux <- cast(aux, rowIdx ~ column, I, fill=NA)
    ROP <- as.numeric( bux[which(bux$`Capacity Zone Name` == "Rest-of-Pool"),
                           "Clearing Price ($/kW-month)"])
    Maine <- as.numeric( bux[which(bux$`Capacity Zone Name` == "Maine"),
                           "Clearing Price ($/kW-month)"])
    bux <- suppressWarnings(cbind(uMRA[m,], data.frame(bux)))

    if (is.na(ROP) || is.na(Maine))
      stop("Something wrong when extracting prices!")

    TT <- rbind(TT, bux) 
  }
  
  
  TT  
}



##############################################################
##############################################################
#
options(width=600)
require(methods)  
require(CEGbase)
require(reshape)
require(xlsx)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.auctions.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FCM.R")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- Sys.Date()
yearStart <- as.numeric(format(Sys.Date(), "%Y"))-1
years <- seq(yearStart, yearStart+6, by=1)

files <- lapply(years, function(year) {
  rLog("Working on auction starting", year)
  download_all_FCM_files(year)
})
files <- unlist(files)

rLog("Archive them ...")
out <- update_archive_RData()

if (nrow(out) > 0 & ("MRA" %in% unique(out$auction)))
{
  TT <- .send_mra_secdb( out ) 
}


if (length(out) > 0) {
  to <- paste(c("adrian.dragulescu@constellation.com", 
    "kate.norman@constellation.com"), sep="", collapse=",")
  to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"

  sendEmail(from, to, "FCM archive updated ",
    c("Added the files:", files, "\n\n", 
      capture.output(print(TT, row.names=FALSE)) ))
}




rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

