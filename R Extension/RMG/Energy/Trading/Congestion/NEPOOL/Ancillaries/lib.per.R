# Deal with Peak Energy Rents
# I could not find any trades with PER booked. Aug-12
#
# .pull_hourly_per
# .pull_marks_per
# .update_archive_monthly_per
#
#




################################################################
# 
#
.pull_hourly_per <- function(asOfDate, startDate=as.Date("2009-01-01"),
  endDate=currentMonth())
{
  # need to read them from, not in tsdb yet 
  # http://www.iso-ne.com/markets/othrmkts_data/fcm/reports/per/monthlyper/index.html

  
}


################################################################
# 
#
.pull_marks_per <- function(asOfDate=Sys.Date(), startDate=nextMonth(),
  endDate=NULL)
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(startDate, "%Y"))+5, "-12-01", sep=""))
  
  location    <- " "
  bucket      <- c("FLAT")
  serviceType <- c("PER")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  fwd <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates=FALSE)

  fwd
}


################################################################
# 
#
.update_archive_monthly_per <- function()
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")

  BASE_URL <- "http://www.iso-ne.com/markets/othrmkts_data/fcm/reports/per/monthlyper/"
  DIR_OUT  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/PER/Monthly/Raw/"

  year <- as.numeric(format(Sys.Date(), "%Y"))
  #year <- 2013
  url <- paste(BASE_URL,  year, "/index.html", sep="")
  links <- .getLinksInPage(url, pattern="\\.gz")    
  .download_and_archive(links, DIR_OUT=DIR_OUT)

  filename <- list.files(DIR_OUT, full.names=TRUE)
  reportName <- .get_reportname(filename[1]) 
  tab <- 1
  AA <- .archive_report(filename, tab=tab, save=TRUE)

  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  write.csv(AA, file="C:/temp/monthly_per.csv", row.names=FALSE)
  rLog("Wrote C:/temp/monthly_per.csv")

  uAA <- ddply(AA, c("Trading.Month", "Capacity.Zone.ID"), function(x) {
        subset(x, reportTimestamp == max(x$reportTimestamp))})
  uAA$Trading.Month <- as.Date(uAA$Trading.Month, "%m/%d/%Y")
  
  res <- cast(uAA, Trading.Month ~ Capacity.Zone.Name, I, fill=0,
              value="Monthly.PER")
  print(res, row.names=FALSE)  # paste into spreadsheet

  uAA <- uAA[order(uAA$Trading.Month),]
  xyplot(Monthly.PER ~ Trading.Month, data=uAA,
         groups=uAA$Capacity.Zone.Name,
         type=c("g", "o"),
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=2))
 
  
  
}




################################################################
################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(xlsx)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.per.R")

  .update_archive_monthly_per()
  

  FF <- .pull_marks_per()

  

  
  
  
}
