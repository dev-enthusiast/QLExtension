# Procmon runs this file.
#
# The spreadsheet PM_node_mark.xls is used ONLY for the hardcoded nodes!
#
#


##############################################################################
# get the list of locations marked by PM  
#
get_locations_marked <- function( asOfDate=Sys.Date() )
{
  fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/Sent Files/",
    "NEPOOL Marks Nodal Spreads ", format(asOfDate, "%m-%d-%y"),
    ".csv", sep="")

  locations <-
    if (file.exists(fileName)) {
      aux <- read.csv(fileName, nrows=6)
      aux <- as.character(aux[2,3:ncol(aux)])

      gsub("2X16H ", "", aux[grep("^2X16H", aux)])
    } else {
      rLog("Cannot find file", fileName)
      rLog("Exiting ...")
      NULL
    }

  locations
}


##############################################################################
# get current marks 
#  
get_marks <- function(locations, asOfDate=Sys.Date())
{
  if (is.null(locations))
    return(NULL)
  
  rLog("Get the marks for all locations ... ")
  commodity <- "COMMOD PWR NEPOOL PHYSICAL"
  location  <- c("HUB", locations)
  bucket    <- c("5X16", "2X16H", "7X8")
  startDate <- nextMonth()
  endDate   <- as.Date(paste(as.numeric(format(Sys.Date(),"%Y"))+10,
    "-12-01", sep=""))    # 10 years out!
  marks   <- PM:::secdb.getElecPrices(asOfDate, startDate, endDate,
    commodity, location, bucket)

  aux <- marks[,c("location", "bucket", "month", "value")]
  hub <- subset(aux, location == "HUB")
  colnames(hub)[4] <- "hub"
  hub$location <- NULL

  bux <- merge(aux, hub)
  bux$value <- bux$value - bux$hub

  res <- subset(bux[,c("location", "bucket", "month", "value")],
               location != "HUB")
  res <- res[order(res$location, res$bucket, res$month), ]
  
  res
}

##############################################################################
# write the file
#  
write_file <- function( MM )
{
  if (is.null(MM))
    return()
  
  fileName <- paste("S:/All/Structured Risk/NEPOOL/Marks/PDS/",
    "NEPOOL Marks Nodal Spreads_", format(Sys.Date()), ".csv", sep="") 

  aux <- c("NEPOOL Nodal Spreads (Node - Hub)",
           paste("As of date:", format(Sys.Date())),
           capture.output(write.csv(MM, file="", row.names=FALSE)))
  
  writeLines(aux, con=fileName)
}


##############################################################################
##############################################################################
options(stringsAsFactors=FALSE)
require(CEGbase)
require(reshape)
require(SecDb)
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.mark.nodes.SecDb.R")


rLog("Pulling marks for PDS ... ")
returnCode <- 0        

locations <- get_locations_marked()

MM <- get_marks( locations )

write_file(MM)


rLog("Closing this window in 10 seconds ...")
Sys.sleep(10)

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}






## if (file.exists(fileName)){
##   rLog("Sending email...")
##   subject <- paste("Emailing: NEPOOL Marks Nodal Spreads ",
##   format(asOfDate, "%m-%d-%y"), ".csv", sep="")
##   body    <- paste(userName, "is sending", fileName, "to ExternalData.")
##   blatVal <- sendEmail(from=paste("adrian.dragulescu@constellation.com", sep=""),
##     to="External.Data2@constellation.com",
## #    to="adrian.dragulescu@constellation.com",
##     subject, body, attachments=fileName)
## }

