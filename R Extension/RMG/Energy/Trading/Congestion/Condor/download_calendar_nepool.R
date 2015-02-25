# Download the nepool calendar
# And archive them to a csv file. 
#
# Written by Adrian Dragulescu on 2011-01-06

################################################################################
# 
#
.archive_calendar <- function(res)
{
  fname <- paste(DIR, "nepool_calendar.csv", sep="")
  if (file.exists(fname)){
    old <- read.csv(fname)
    aux <- merge(old[,c("eventId", "date")], res[,c("eventId", "date")])
    if (nrow(aux)==3)
      stop("eventId used by NEPOOL is not unique!")
    old <- old[!(old$eventId %in% res$eventId),]
    res <- rbind(old, res)
  }
  res <- res[order(res$date, res$eventId),]
  
  write.csv(res, file=fname, row.names=FALSE)  
  rLog("Updated file", fname)
  
}


################################################################################
# Nepool is really cool about changing the filenames based on "their" report
# time!  So I need to find them in the html page source ... 
#
.getEventsForMonth <- function(month)
{
  rootSite <- "http://www.iso-ne.com/calendar/list.action?date="
  yyyymmdd <- format(month, "%Y%m01")
  mainLink <- paste(rootSite, yyyymmdd, sep="")

  rLog("Working on", yyyymmdd)
  con <- url(mainLink)
  LL  <- readLines(con)
  close(con)

  pattern = "/calendar/detail\\.action"
  ind <- grep(pattern, LL)  
  if (length(ind)==0)
    stop(paste("Could not find any files with the pattern", pattern))

  aux <- LL[ind]
  aux <- strsplit(aux, "dayHeader")[[1]]  # split by days! 
  aux <- aux[!grepl("table border", aux)]

  # pairs of days and events within the day
  indDays <- grep("day.action", aux)

  # loop over the days
  res <- vector("list", length=length(indDays))
  for (i in 1:length(indDays)){
    thisDay <- gsub('.*date=(.*)\\&cats.*', "\\1", aux[indDays[i]])

    res[[i]] <- cbind(date=thisDay, .parseOneDay(aux[indDays[i]+1]))
  }
  res <- res[sapply(res, function(x){ncol(x)==4})]
  res <- do.call(rbind, res)
  
  res
}

##################################################################
#
  .parseOneDay <- function(x)
  {
    bux <- strsplit(x, "<li")[[1]]
    if (grepl("No events", bux[2]))
      return(NULL)
    
    eventId <- gsub(".*eventId=(.*)\\&date.*", "\\1", bux)
    eventTime <- gsub(".*Event>(.*)<br/><a onclick=.*", "\\1", bux)
    eventDescription <- gsub(".*yes>(.*)</a><br/>.*<br/>.*", "\\1", bux)

    z <- data.frame(eventId=eventId, eventTime=eventTime,
                    eventDescription=eventDescription)
    ind.na <- (grepl("meeting|training", tolower(eventDescription)) &
         grepl("cancelled", tolower(eventDescription)))
    z$eventDescription[ind.na] <- "Cancelled Meeting or Training"
    options(warn=-1)
    z$eventId <- as.numeric(z$eventId)
    options(warn=0)
    z <- z[!is.na(z$eventId),] 

    z
  }


##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
Sys.setenv(tz="")

returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_calendar_nepoool.R")
rLog("Start at ", as.character(Sys.time()))
DIR <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/"

asOfDate <- Sys.Date()
startDt <- asOfDate-20; endDt <- asOfDate
#startDt <- as.Date("2009-01-01"); endDt <- as.Date("2010-12-01")
months <- seq(startDt, endDt, by="month")

res <- vector("list", length=length(months))
for (i in 1:length(months)){
  res[[i]] <- .getEventsForMonth(months[i])
}
res <- do.call(rbind, res)

.archive_calendar(res)

# res[nchar(res$eventDescription)>300,]

rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


