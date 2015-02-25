# Analyze DA energy offers
#
# Strat's unit mapping (on sheet Asset ID mapping): 
#   S:\Strategies\Bid Offer Data\NEPOOL\NEPOOL_Unit Map merged with gen_sum_statistics.xls
#

##########################################################################
#
.load_power_gas <- function()
{
  symbs <- "GasDailyMean_ALGCG"
  startDate <- MIN_DATE
  endDate   <- Sys.Date()-1
  gas <- tsdb.readCurve(symbs, startDate-1, endDate)
  gas$gas <- gas$value; gas$value <- NULL
  gas <- merge(gas, data.frame(date=seq(startDate, endDate, by="1 day")), all=TRUE)

  # fill in the weekends, holidays with last value
  gg <- zoo(gas$gas, gas$date)
  gg <- na.locf(gg)
  gg <- data.frame(date=index(gg), gas=as.numeric(gg))
  
  pwr <- tsdb.readCurve("NEPOOL_SMD_DA_4000_LMP", startDate, endDate)
  pwr$hub <- pwr$value; pwr$value <- NULL
  pwr$date <- as.Date(format(pwr$time, "%Y-%m-%d"))

  res <- merge(pwr, gg[,c("date", "gas")], by="date", all.x=TRUE)
  res <- res[order(res$time), ]
  colnames(res)[2] <- "datetime"
  
  res
}

##########################################################################
#
.plot_historical_hr <- function()
{
  require(lattice)

  symbs <- "GasDailyMean_ALGCG"
  startDate <- MIN_DATE
  #endDate   <- seq(MAX_DATE, by="1 month", length.out=2)[2]-1
  endDate <- Sys.Date()-1
  gas <- tsdb.readCurve(symbs, startDate, endDate)
  gas$gas <- gas$value

  lmp <- PM:::ntplot.bucketizeByDay("nepool_smd_da_4000_lmp",
    "Peak", startDate, endDate)
  lmp$date <- lmp$day
  lmp$peak.lmp <- lmp$value

  HR <- merge(gas[,c("date", "gas")], lmp[,c("date", "peak.lmp")])
  HR$hr <- HR$peak.lmp/HR$gas
  HR$year <- format(HR$date, "%Y")
  HR$dayOfYear <- as.numeric(HR$date - as.Date(format(HR$date, "%Y-01-01")))
  HR$year <- as.factor(HR$year)
  HR$month <- format(HR$date, "%b")
  HR$dayOfMonth <- as.numeric(HR$date - as.Date(format(HR$date, "%Y-%m-01")))

  windows(8,6)
  xyplot(hr ~ dayOfYear, data=HR, groups=year, type=c("smooth", "p"),
    xlab="Day of year", ylab="Heat Rate, DA Peak/AlgonCG", span=1/8, lwd=2,
    key=list(points=Rows(trellis.par.get("superpose.symbol"), 1:3),
      text=list(levels(HR$year)), columns=length(levels(HR$year)))     
  )

  HR
}


##########################################################################
##########################################################################
#
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(SecDb)
  require(zoo)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.download.bidoffer.R")
  source(paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/ISO_Data/EnergyOffers/",
    "lib.investigate.energy.offers.R", sep=""))
  source(paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/ISO_Data/EnergyOffers/",
    "main_energy_offers.R", sep=""))
  
  {
    rootSite <- "http://www.iso-ne.com/markets/hstdata/mkt_offer_bid/da_energy/"
    pattern <- "WW_DAHBENERGY_ISO_"
    outdir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/EnergyOffers/"
    DIR_EO <<- paste(outdir, "RData/", sep="")
  }

  # download one month worth of data, and aggregate it into an RData file
  month <- as.Date("2010-03-01")  
  .process(month, outdir, rootSite, pattern, fun=.cMonth.EO) 

  # load one month
  filename <- paste(outdir, "RData/DB_201110.RData", sep="")
  load(filename) # loads res variable
  


  ##########################################################################
  # make the long file format (only PQ pairs for all units )
  files <- list.files(DIR_EO, full.names=TRUE, pattern="^DB_")
  for (f in files)
    EO.by_month(f, outdir)

  ##########################################################################
  # get all the data for a unit
  maskedNodeIds <- c(11515)
  for (id in maskedNodeIds)
    EO.by_maskedNodeId(id)  
  


  ##########################################################################

  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
  nfind <- function(x) MAP[grep(toupper(x), MAP$name),]

  MAX_DATE <<- as.Date(paste(gsub("DB_(.*)\\.RData", "\\1",
    sort(list.files(DIR_EO, pattern="^DB_"), decreasing=TRUE)[1]), "01", sep=""), "%Y%m%d")
  MIN_DATE <<- as.Date(paste(gsub("DB_(.*)\\.RData", "\\1",
    list.files(DIR_EO, pattern="^DB_")[1]), "01", sep=""), "%Y%m%d")

  
  # My main map.
  load(paste(DIR_DB, "../ptidToMask.RData", sep=""))


  # 
  aux <- subset(res, Must.Take.Energy > 0 & Claim.30 > 0 & nodeId %in% c(60311, 45632))
  sort(unique(aux$nodeId))
  write.csv(aux, file="C:/Temp/junk.csv")

  x <- unique(res[, c("nodeId", "Cold.Startup.Price")])
  x <- x[order(x$nodeId), ]

  unitMaxStartup <- x[which(x$Cold.Startup.Price == max(x$Cold.Startup.Price)), ]$nodeId


  A <- read.csv("C:/Temp/junk.csv")

  # example  must take energy
  x <- res
  x$date <- as.Date(format(x$datetime - 1, "%Y-%m-%d"))
  z <- ddply(x, .(date, nodeId), function(y){
#    browser()
    maxmin <- max(y$Must.Take.Energy) - min(y$Must.Take.Energy)
    if (maxmin > 1)
      y
    else
      NULL
  })
  z2 <- z[order(z$nodeId, z$datetime), c(1:8, 34)]
  
  
  # get unique participants by month ...
  uParticipantsMonth <- .get_unique_participantsMonth()
  cast(uParticipantsMonth, month ~ ., length, value="participantId")
  # number has steadily increased ... 


  # get the power and gas hourly
  HUB <- .load_power_gas()  

  # look at a HR expansion 
  hr <- c(6,7,8,9,10,11,14,17,19)
  files <- list.files(paste(DIR_EO, "Long", sep=""), full.names=TRUE,
    pattern="^long_")

  file <- files[5]
  file <- files[29]


  QQ <- get_cleared_mw_lessThanHR(file, HUB, hr)
  windows()
  xyplot(MW ~ datetime, data=QQ, groups=hr, type="l",
    main="May 2010", cex.main=1)

  # analyze Brayton Point
  analyze_Brayton()


  
  # analyze Northfield Montain
  maskedNodeIds <- c(53298, 60188, 66602, 83099)
  for (id in maskedNodeIds)
    EO.by_maskedNodeId(id)  # extract the Energy Offers


  
  analyze_NorthfieldMontain(X)

  # analyze Salem Harbor
  res <- analyze_SalemHarbor()

  
  fname <- paste(DIR_EO, "DB_201006.RData", sep="")
  load(fname)

  X <- res
  hour <- as.POSIXct("2010-06-01 18:00:00")


  # transpose example
  y <- melt(x, id=1:3)
  z <- cast(y, datetime + nodeId ~ variable + segment, I, fill=NA)

  z$date <- as.Date(format(z$datetime -1, "%Y-%m-%d"))

  z2 <- unique(z[,-1])

  unique(z[,"date"])
  
  
}




