# analyze NEPOOL regulation costs, requirements, etc.
#
# Location of existing marking spreadsheet 
# S:\All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates\Regulation\Regulation Marks.xlsx
# Heejin says to try reg ~ ng + spark.spread
#
# archive_mis_reg
# .get_hist_data
# .get_prelim_data
# .pull_hist_regulation      - read actual regulation costs from tsdb
# .pull_marks_regulation     - read regulation marks from SecDb
#
#

################################################################
# archive the MIS reports
#
archive_mis_reg <- function(months=NULL)
{
  if (is.null(months))
    months <- rev(seq(prevMonth(), by="-1 month", length.out=12))
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  reportName <- "SD_REGDTLASM"; tab <- 1

  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  AA


  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)

  FR <- subset(AA, Asset.ID %in% c(1691, 40327, 40328) & !is.na(Regulation.Payment) & Regulation.Payment > 0)
  FR$month <- format(FR$reportDay, "%Y-%m-01")
  res <- cast(FR, month ~ ., sum, value="Regulation.Payment")
  colnames(res)[2] <- "FR.Regulation.Payments"
  print(res, row.names=FALSE)
  
  
}



#####################################################################
#
.get_hist_data <- function(startDt, endDt)
{
  # NEPOOL/NEPOOL Data/NEPOOL SR REG Reports
  # hourly clearing price, 4 days older 
  reg.cp <- "NERpt_Reg_ClearingPrice"

  # have 5min data, but the symbol has gaps
  "nepool_rt5m_reg_cp" 

  # hourly pool regulation procured 
  reg.mw <- "NERpt_Reg_PoolReg"

  # do a rolling monthly avg of the symbols above and note 
  # that as the quantity has gone down, the price for 
  # regulation has come down too.

  symbols <- c(
   "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
   "NEPOOL_SMD_RT_4000_lmp",         "hub.rt",            
   "NeRpt_rtCus_PoolLoadObl",        "rt.load",              # hourly
   reg.cp,                           "reg.cp",
   reg.mw,                           "reg.mw"            
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)

  try({
    PR <- .get_prelim_data()               # last 7 days
    PR <- PM:::isotimeToPOSIXct(PR, extraHourToken="02X")
    pr <- zoo(PR$reg.cp, PR$datetime)
    HH$reg.cp[which(index(HH) %in% index(pr))] <- as.numeric(pr)
  })


  aux <- PM:::filterBucket(HH, buckets=c("5X16", "2X16H", "7X8"))
  DD <- lapply(aux, function(x){aggregate(x, as.Date(index(x)-1),
                                          mean, na.rm=TRUE)})

  
  # get the gas, gen out
  GG <- FTR:::FTR.load.tsdb.symbols(c("gasdailymean_algcg",
    "gasdailymean_tetm3"), startDt, endDt)
  colnames(GG) <- c("algcg", "tetm3")


  # add the gas in the mix
  DD <- lapply(DD, function(x, GG){merge(x, GG, all=c(TRUE, FALSE))}, GG)
  lapply(DD, head)
  
  DD
}


#####################################################################
# From NEPOOL web site
#
.get_prelim_data <- function(startDt=Sys.Date()-7, endDt=Sys.Date())
{
  url <- paste("http://www.iso-ne.com/markets/hrly_data/res/",
    "hourlyRES.do?report=rcp&subcat=prelim&submit=csv&startDate=",
    format(startDt, "%Y%m%d"), "&endDate=", format(endDt, "%Y%m%d"),
    sep="")
  data <- read.csv(url, skip=4, colClasses="character")
  data <- data[,-1]           # first column is the header
  data <- data[-nrow(data),]  # last row is the Total
  colnames(data) <- c("Date", "Hour.Ending", "reg.cp")

  data
}


#####################################################################
# From NEPOOL web site
#
.pull_marks_regulation <- function(asOfDate=Sys.Date(), startDate=nextMonth(),
  endDate=NULL)
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(startDate, "%Y"))+7,
                             "-12-01", sep=""))
      
  bucket      <- c("FLAT")
  serviceType <- c("REG")
  location    <- " "
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  fwd <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates=FALSE)

  
  fwd
}




#####################################################################
#####################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.regulation.R")

  fwd <- .pull_marks_regulation()
  

  
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  startDt <- as.POSIXct("2007-01-01 01:00:00")   # for hist data
  endDt   <- Sys.time()                          # for hist data 
  
  forecastStartDate <- as.Date("2009-01-01")  # <-- may want to change
  
  DD  <- .get_hist_data(startDt, endDt)  # daily by bucket


  x <- DD[["5X16"]]
  x <- cbind(day=index(x), data.frame(x))
  x$year <- format(x$day, "%Y")
  x$mon  <- format(x$day, "%m")


  
  x <- DD[["5X16"]]
  m <- aggregate(x, as.Date(format(index(x), "%Y-%m-01")), mean)

  
  
  require(lattice)
  xyplot(reg.cp ~ algcg, data=x, groups=year)
  
  xyplot(log(reg.cp) ~ algcg|year, data=x)

  xyplot(reg.cp ~ hub.da|year, data=x)

  

  
  
  plot(index(x), as.numeric(x$reg.cp))


  
  
  
}
