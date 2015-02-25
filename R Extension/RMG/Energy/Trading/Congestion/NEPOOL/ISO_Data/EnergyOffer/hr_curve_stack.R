# plot the hr curve stack for different historical years
# for the peak day in the month
#
#

##########################################################################
# just some dily-daly
#
.analyze_one_hour <- function()
{
  filename <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
                    "EnergyOffers/RData/DB_201006.RData", sep="")
  load(filename)  # the EO data for the month
  EO <- subset(EO, datetime == as.POSIXct("2010-06-28 15:00:00"))

  # not finished -- how to include the FLOWS, etc. 

  
}



##########################################################################
#
.fuel_stats <- function(gg)
{
  gg$year <- substr(gg$day, 1, 4)
  aux <- melt(gg[, c("year", "algcg", "fo6")], id=1)
  ff  <- cast(aux, year ~ variable, mean, na.rm=TRUE, fill=NA)
  ff$fo6 <- (ff$fo6/42)/0.15
  ff$ratio <- ff$fo6/ff$algcg 
  
  print(ff, row.names=FALSE)
  
}


##########################################################################
# loop over the monthly files and extract only the datetimes you want
# days is a data.frame
#
.read_EO_data <- function(files, days)
{
  res <- lapply(as.list(files), function(filename, days){
    rLog("Working on file ", filename)
    load(filename)

    aux <- subset(EO, datetime %in% days$datetime)
    aux$day <- format(aux$datetime, "%Y-%m-%d")
    aux <- merge(aux, gg[,c("day", "algcg")], all.x=TRUE, by="day")
    aux$hr <- aux$P/aux$algcg

    bux <- ddply(aux, .(day), function(x){
      y <- x[order(x$hr), c("datetime", "MW", "hr")]
      y$cumMW <- cumsum(y$MW)
      y
    })

    bux
  }, highDays)
  res <- do.call(rbind, res)
  
  res$year <- factor(format(res$datetime, "%Y"))

  res  
}

##########################################################################
#
.read_hist_data <- function(startDt, endDt, focusMonth=6)
{
  symbols <- c(
   "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
   "NEPOOL_SMD_RT_4000_lmp",         "hub.rt",            
   "nepool_rt_hr_sysdemand",         "rt.load"              # hourly
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)
  hh <- subset(HH, as.numeric(format(index(HH)-1, "%m"))==focusMonth)
  hh$year <- as.numeric(format(index(hh), "%Y"))
  hh$HE   <- as.numeric(format(index(hh), "%H"))+1
  hh <- cbind(datetime=index(hh), as.data.frame(hh))
  hh$day  <- format(hh$datetime, "%Y-%m-%d")
  rownames(hh) <- NULL
  
  # summary stats
  ddply(data.frame(hh), .(year), function(x){
    summary(as.numeric(x$rt.load), na.rm=TRUE)[1:6]})
  
  
  # get the gas, gen out
  GG <- FTR:::FTR.load.tsdb.symbols(c("gasdailymean_algcg",
    "gasdailymean_tetm3", "fo6_spot_northeast"),
    startDt, endDt)
  colnames(GG) <- c("algcg", "tetm3", "fo6")
  gg <- subset(GG, as.numeric(format(index(GG), "%m"))==focusMonth)
  gg <- cbind(day=format(index(gg)), data.frame(gg))
  rownames(gg) <- NULL
  
  return(list(hh, gg))
}




##########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(zoo)
  require(lattice)

  dir.root <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/"
  source(paste(dir.root, "ISO_Data/EnergyOffers/lib.investigate.energy.offers.R", sep=""))
  source(paste(dir.root, "ISO_Data/EnergyOffers/hr_curve_stack.R", sep=""))
  
  DIR_DB <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/EnergyOffers/RData/"

  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  startDt  <- as.POSIXct("2007-01-01 01:00:00")   # for hist data
  endDt    <- Sys.time()                          # for hist data 

  focusMonth <- 9
  focusYear  <- 2011
  
  aux <- .read_hist_data(startDt, endDt, focusMonth=focusMonth)
  hh  <- aux[[1]]
  gg  <- aux[[2]]

  # ratio of gas/fo6 fuels
  .fuel_stats(gg)

  
  ## q95 <- quantile(hh$rt.load, 0.95, na.rm=TRUE)  # 95% load
  ## highDays <- subset(hh, rt.load > q95)[,c("datetime", "day", "rt.load")]
  ## highDays <- ddply(highDays, .(day), function(x){x[which.max(x$rt.load),]})

  # pick top 2 days from each year
  highDays <- ddply(hh, .(year), function(x){
    y <- ddply(x, .(day), function(y){y[which.max(y$rt.load),]}) # max load by day
    tail(y[order(y$rt.load), ], 2)
  })
  
  files <- format(ISOdate(2007:2010, focusMonth, 1), "%Y%m")
  files <- paste(DIR_DB, "Long/long_", files, ".RData", sep="")
  res   <- .read_EO_data(files, highDays)

  # read the UPLAN stack
  require(xlsx)
  fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/Monthly Auction Analyses/",
    focusYear, "/", focusYear, "-", sprintf("%02.f", focusMonth), "/",
    "UPLAN Heat Rate.xlsx", sep="")
  uHR <- read.xlsx(fname, sheetIndex=1)
  uHR <- data.frame(day=Sys.Date(), datetime=Sys.time(),
    MW=uHR$cumMW, hr=uHR$cumHR, cumMW=uHR$cumMW, year="UPLAN")
  #uHR$cumMW <- uHR$cumMW - 1667  # take out HQPhII + NB
  #uHR <- subset(uHR, cumMW >= 0)
  

  # the plot
  X <- rbind(res, uHR)  
  sym.col <- trellis.par.get("superpose.symbol")
  X$col   <- sym.col$col[as.numeric(X$year)]
  
  xyplot(hr ~ cumMW, data=X, groups=day,
    xlab="Gen Offered MW", ylab="Heat rate, Algcg gas",
    main="Historical energy offer curves",
    xlim=c(5000, 30000), ylim=c(0, 40), type="l",
    panel=panel.superpose,      
    panel.groups = function(x, y, ..., subscripts){
      panel.grid(h=-1, v=-1, lty=2)
      #browser()
      lwd=1
      if (as.character(X$year[subscripts[1]])=="UPLAN"){ lwd=2 }
      panel.xyplot(x, y, type="l", col=X[subscripts[1],"col"], lwd=lwd)
    },
    key=list(points=Rows(sym.col, 1:length(levels(X$year))),
      text=list(levels(X$year)), columns=length(levels(X$year)))
  )


  



  
}


  
  
  ## sym.col$col <- c("blue", "red", "black", "green")
  ## trellis.par.set("superpose.symbol", sym.col)
  
