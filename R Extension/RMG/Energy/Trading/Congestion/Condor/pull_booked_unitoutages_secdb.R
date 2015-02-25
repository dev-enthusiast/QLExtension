# Pull unit outages from SecDb and gen unit db and send emails
#
# .compare_files
# .notify_outages - read outages from gen unit database
# .pull_one  - read outages from one unit entitlement object
#
# Written by AAD on 26-May-2011



#################################################################
#
.compare_files <- function(res)
{
  rLog("Reading existing outage schedule")
  DIR <- "S:/All/Structured Risk/NEPOOL/Position Management/Boston Gen/Outage Booking/"
  filename <- paste(DIR, "secdb_outage_schedule.xlsx", sep="")
  res0 <- read.xlsx(filename, sheetIndex=1)
  names(res0)[1] <- "asOfDatePrevious"
  colnames(res0) <- toupper(colnames(res0))
  
  out <- merge(res0, res, all=TRUE)
  out <- out[apply(out, 1, function(x){any(is.na(x))}),]

  if (nrow(out)>0){
    out <- c(paste("Changes", ":\n\n", sep=""), capture.output(print(out,
      row.names=FALSE)))
    to <- paste(c("necash@constellation.com",
      "steven.webster@constellation.com"),
      sep="", collapse=",")
    #to <- "adrian.dragulescu@constellation.com"
    from <- "OVERNIGHT_PM_REPORTS@constellation.com"
    sendEmail(from, to, "Outage info for units changed", out)
   
    rLog("Backup the old data")
    filename <- paste(DIR, "secdb_outage_schedule_", format(res0$asOfDate0[1]),
                      ".xlsx", sep="")
    write.xlsx(res0, filename, row.names=FALSE)
  }

  invisible()
}

#################################################################
#
.notify_outages <- function(notificationPeriod)
{
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))  

  all_units <- .get_all_ccg_units()

  units <- c(
    grep("^My", all_units, value=TRUE),        # all the Mystics
    grep("^Fore Ri", all_units, value=TRUE)    # all ForeRiver         
    #grep("^Kl", units, value=TRUE),        # no Kleen in there
    )

  out <- .get_planned_outages(units, asOfDate=asOfDate)
  out$OUTAGE_START_DATE <- as.Date(format(out$OUTAGE_START_DATE, "%Y-%m-%d"))
  
  out <- subset(out, OUTAGE_START_DATE > asOfDate)
  out <- out[order(out$OUTAGE_START_DATE), ]

  aux <- split(out, out$OUTAGE_START_DATE)
  
  outageDays <- unique(out$OUTAGE_START_DATE)
  for (d in 1:length(outageDays)){
    outageDay <- outageDays[d]
    if (as.numeric(outageDay - Sys.Date()) %in% notificationPeriod) {
      TT <- capture.output(print(aux[[format(outageDay)]], row.names=FALSE))
      to <- paste(c("necash@constellation.com",
        "steven.webster@constellation.com"),
        sep="", collapse=",")
      #to <- "adrian.dragulescu@constellation.com"
      from <- "OVERNIGHT_PM_REPORTS@constellation.com"
      sendEmail(from, to, "Notification for planned outages due", TT)
    }    
  }
  
}

#################################################################
#
.pull_one <- function(unit)
{
  rLog("Pulling outage for", unit)
  os <- secdb.getValueType(paste("Bal Close PWR", unit),
                           "Unit Outage Schedule")
  
  res <- cbind(unit=unit,
    do.call(rbind, lapply(os, as.data.frame)))
  # sometimes there's no outage data for this unit
  if (ncol(res) == 1) return(NULL)
  
  #res$Start.Time <- as.Date(format(res$Start.Time, "%Y-%m-%d"))
  #res$End.Time   <- as.Date(format(res$End.Time, "%Y-%m-%d"))
  #res$Notification.Time <- as.Date(format(res$Notification.Time, "%Y-%m-%d"))
  res$Start.Time <- format(res$Start.Time, "%Y-%m-%d %H:%M:%S")
  res$End.Time   <- format(res$End.Time, "%Y-%m-%d %H:%M:%S")
  res$Notification.Time <- format(res$Notification.Time, "%Y-%m-%d %H:%M:%S")
  colnames(res) <- toupper(colnames(res))
  
  res <- res[,c("UNIT", "AVAILABILITY", "NOTIFICATION.TIME",
                "START.TIME", "END.TIME", "TYPE")]
  res
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(CEGbase)
require(xlsx)
require(SecDb)
require(reshape)
require(RODBC)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.genunitdb.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0


rLog("Do we need to notify?  Reading GenUnit DB")
notificationPeriod <- c(97, 96, 52, 51, 14, 13, 6)  # days before outage
.notify_outages(notificationPeriod)


rLog("Pulling outages booked in SecDb")
asOfDate   <- Sys.Date()
units <- c("Foreriver", "Mystic 7", "Mystic 8", "Mystic 9",
         "Mystic Jet", "Kleen")

res <- lapply(as.list(units), .pull_one)
res <- do.call(rbind, res)
res <- cbind(asOfDate=asOfDate, res)
res <- res[order(res$UNIT, res$START.TIME),]

.compare_files(res)

DIR <- "S:/All/Structured Risk/NEPOOL/Position Management/Boston Gen/Outage Booking/"
filename <- paste(DIR, "secdb_outage_schedule.xlsx", sep="")
write.xlsx(res, filename, row.names=FALSE)
rLog("Wrote file", filename)



rLog(paste("Done at ", Sys.time()))
q(status = returnCode)




## # Initialize SecDb for use on Procmon
## DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
## Sys.setenv(SECDB_SOURCE_DATABASE=DB)
## Sys.setenv(SECDB_DATABASE="Prod")
