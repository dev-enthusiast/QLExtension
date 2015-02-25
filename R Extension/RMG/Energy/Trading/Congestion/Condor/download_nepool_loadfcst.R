# Download the nepool short term load forecast
# And archive them to a csv file. 
#
# Written by Adrian Dragulescu on 2011-01-06

################################################################################
# 
#
.archive <- function(newF, compare=TRUE)
{
  if (compare)
    .compare_changes(newF)

  # save the latest forecast
  timeStamp <- as.POSIXct(strptime(newF$timeStamp[1], "%m/%d/%Y %I:%M %p"))
  fileout <- paste(DIR, "/Raw/st_loadforecast_", format(timeStamp,
    "%Y%m%d_%H%M%S"), ".csv", sep="")
  write.csv(newF, file=fileout, row.names=FALSE)
  rLog("Wrote", fileout)

  # append to the big file too
  fname <- paste(DIR, "nepool_st_load_forecast.csv", sep="")
  if (file.exists(fname)){
    oldF <- read.csv(fname)
    uTimeStamps <- sort(unique(oldF$timeStamp))
    if (newF$timeStamp[1] %in% uTimeStamps){
      res <- oldF                        # timestamp already there
    } else {
      res <- rbind(oldF, newF)           # append new timestamp
    }
  }
  res <- res[order(res$timeStamp, res$date, res$HE),]
  
  write.csv(res, file=fname, row.names=FALSE)  
  rLog("Updated file", fname)

}

################################################################################
# Prepare a side by side comparison of changes in load forecast
# Have to run this before archiving ...
#
.compare_changes <- function(newF)
{
  files <- list.files(paste(DIR, "Raw", sep=""))
  lastTstamp <- max(gsub("st_loadforecast_(.*)\\.csv", "\\1", files))
  thisTstamp <- format(as.POSIXct(strptime(newF$timeStamp[1], "%m/%d/%Y %I:%M %p")),
                                  "%Y%m%d_%H%M%S")
  if (lastTstamp == thisTstamp) 
    return(NULL)

  prevF <- read.csv(paste(DIR, "Raw/st_loadforecast_", lastTstamp, ".csv", sep=""))
  names(prevF)[4] <- lastTstamp  #paste(names(prevF)[4], lastTstamp)
  prevF$timeStamp <- prevF$Adjustment <- NULL
  if (class(prevF$HE)=="integer")
    prevF$HE <- sprintf("%02.0f", prevF$HE)
  
  names(newF)[4]  <- thisTstamp  #paste(names(newF)[4], thisTstamp)
  newF$timeStamp  <- newF$Adjustment <- NULL
   
  aux <- merge(prevF, newF, by=c("date", "HE"))
  aux$change <- aux[,4]-aux[,3]

  if (nrow(aux) > 0) {
    out <- capture.output(print(aux, row.names=FALSE))
    rLog("Email results")
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
      "NECash@constellation.com",
  #    "adrian.dragulescu@constellation.com",
      paste("NEPOOL load forecast change"), out)
  }
  
}


################################################################################
# An update comes by 10AM, 
#
.get_loadforecast <- function()
{
  link <- "http://www.iso-ne.com/markets-operations/system-forecast-status/three-day-system-demand-forecast"
  
  con <- url(link)
  LL  <- readLines(con)
  close(con)

  timeStamp <- strsplit(LL[grep("as of", LL)], "as of ")[[1]][2]
  timeStamp <- substr(timeStamp, 1, 19)
  
  ind <- grep("forecast_table", LL)[1]  # table starts here
  LL  <- LL[-(1:(ind-1))]
  ind <- grep("</table>", LL)[1]     # table ends here
  LL  <- LL[1:(ind-1)]

  ind <- grep("tbody", LL)
  ## if (length(ind) != 192){     # cannot do this for DST
  ##   sendEmail("adrian.dragulescu@constellation.com",
  ##             "adrian.dragulescu@constellation.com",
  ##             "Fix nepool load foreacst download!!!", "")
  ##   stop("Report format has changed!")
  ## }

  aux <- LL[(ind+1):length(LL)]
  aux <- aux[-grep(" +<.*tr>", aux)]
  aux <- aux[-length(aux)]  # last one is " "
  aux <- gsub(" +<td.*>(.*)</td>", "\\1", aux)

  
  aux <- matrix(aux, ncol=8, byrow=TRUE)
  colnames(aux) <- c("HE+1D", "NativeDemand+1D", "Adjustments+1D",
    "ForecastDemand+1D", "HE+2D", "ForecastDemand+2D",
    "HE+3D", "ForecastDemand+3D")

  day1 <- as.Date(substr(timeStamp, 1, 10), "%m/%d/%Y")
  res1 <- data.frame(date=day1, HE=aux[,"HE+1D"],
    ForecastedDemand=aux[,"NativeDemand+1D"], Adjustment=aux[,"Adjustments+1D"])
  res2 <- data.frame(date=day1+1, HE=aux[,"HE+2D"],
    ForecastedDemand=aux[,"ForecastDemand+2D"], Adjustment=0)
  res3 <- data.frame(date=day1+2, HE=aux[,"HE+3D"],
    ForecastedDemand=aux[,"ForecastDemand+3D"], Adjustment=0)
  
  res <- cbind(timeStamp=timeStamp, rbind(res1, res2, res3))
  res$ForecastedDemand <- as.numeric(res$ForecastedDemand)
  res$date <- as.character(res$date)

  res <- subset(res, HE != "")
  
  res
}



##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
Sys.setenv(tz="")

returnCode <- 99   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_nepool_loadfcst.R")
rLog("Start at ", as.character(Sys.time()))
DIR <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Load/"

try(res <- .get_loadforecast())
if (class(res) != "try-error"){
  .archive(res, compare=TRUE)
}

rLog("Done at ", as.character(Sys.time()))

if (as.numeric(format(Sys.time(), "%H")) >= 19)
  returnCode <- 0    # be happy because you ran 4 times during the day!

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


