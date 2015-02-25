
## Historical Valuation of GD Option ################################################################################

# options<-NULL
# options$Index1<-"IGBEL00"
# options$Index2<-"IGBEL00"
# options$start.mm.dd<-"11/1"
# options$end.mm.dd<-"3/31"
# options$start.year<-2005
# options$end.year<-2006
# options$numCalls<-30
# options$callsper<-"term"
# options$numTrials<-1000
# options$maxVolume<-1000
# options$minVolume<-1000
# options$strType<-"gda"
# options$traderDist<-"tri"
# options$fwdCurve1<-"COMMOD NG TRZN6 NON PHYSICAL"
# options$priceDate<-"2006-12-05"
# options$deal.start.year<-2007
# options$deal.end.year<-2008

# need the following R packages: reshape, rodbc, triangle, chron, fcalendar


histValShell <- function(options, startmm, startdd, endmm, enddd)
{

 require(chron)
 require(fCalendar)
 require(reshape)
 require(RODBC)
 require(triangle)

 options$start.mm.dd <- paste(startmm,"/",startdd,sep="")
 options$end.mm.dd <- paste(endmm,"/",enddd,sep="")
   
 histValShell <- mainHistValGDOption(options)
 histValShell <- quantile(histValShell,probs=c(seq(0,1,by=0.05)))
 
}

###################################################################################################################

mainHistValGDOption <- function(options)
{
 # get series of months and series of days in each month
 subResult <- array(0,c(options$numTrials,1))
 dealMths <- monthsBetween(options$start.mm.dd,options$end.mm.dd,options$start.year,options$end.year)
 deallastDay <- as.numeric(as.Date(paste(options$end.mm.dd,"/",options$end.year-2000,sep=""),"%m/%d/%y"))
 numMths <- count(as.numeric(dealMths) < deallastDay)
 
 options$lastDaySeries <- NULL
 options$mthSeries <- NULL
 options$yearSeries <- NULL
 # options$dealYearSeries <- NULL
 
 for(mthCount in 1:length(dealMths)) {
  lastDayofMth <- timeLastDayInMonth(as.Date(dealMths[mthCount]))
  zz <- month.day.year(as.numeric(as.Date(attributes(lastDayofMth)$Data)))
  options$mthSeries <- cbind(options$mthSeries, zz$month)
  options$lastDaySeries <- cbind(options$lastDaySeries, zz$day)
  options$yearSeries <- cbind(options$yearSeries, zz$year)
 }
 
 dealYearSeries <- options$deal.start.year + c(0,cumsum(diff(as.numeric(options$yearSeries))))

 # historical valuation for "n" calls per term or "n" calls per month
 if(options$callsper=="term") mainHistValGDOption <- histValGDOption(options)
 else # if(options$callsper=="month") 
 { 
    for(mthCount in 1:length(dealMths)) {
      options$start.mm.dd <- paste(options$mthSeries[mthCount],"/","1",sep="") 
      options$end.mm.dd <- paste(options$mthSeries[mthCount],"/",options$lastDaySeries[mthCount],sep="") 
      options$start.year <- options$yearSeries[mthCount]
      options$end.year <- options$start.year
      options$deal.start.year <- dealYearSeries[mthCount] 
      options$deal.end.year <- options$deal.start.year
      subResult <- subResult + histValGDOption(options)
      
  }
  mainHistValGDOption <- subResult/length(dealMths)
 }
 
}

###################################################################################################################

histValGDOption <- function(options)
{

# get historical price data
histP <- getHistP(options$Index1, options$start.mm.dd, options$end.mm.dd, options$start.year, options$end.year)

# simulate P, price at which trader buys gas
P <- simP(histP$lowP,histP$midP,histP$highP,options$traderDist,options$numTrials)

# simulate V, volume exercised per day
numDays <- length(histP$midP)
V <- simV(options$minVolume, options$maxVolume, numDays, options$numTrials)

# strike; CCG receives this as payment during call event
X <- strike(histP$lowP, histP$midP, histP$highP, options$numTrials, options$strType, options$Index2, options$start.mm.dd, options$end.mm.dd, options$start.year, options$end.year, options$lastDaySeries,histP$nonTradingDayIndx)

# calculate unit payoff
options$numCalls <- min(options$numCalls,length(histP$midP))
payoff <- unitPayoff(P, V, X, options$numCalls, options$numTrials)

# normalize payoff to current price levels
histValGDOption <- normPayoff(options$fwdCurve1, options$priceDate, options$start.mm.dd, options$end.mm.dd, options$deal.start.year, options$deal.end.year,histP$midP,payoff)
# histValGDOption <- payoff
# return(quantile(histValGDOption,probs=c(0,0.15,0.5,1)))

}

###################################################################################################################

getHistP <- function(Index, start.mm.dd, end.mm.dd, start.year, end.year)
{
  # lib file
  source("H:\\user\\R\\RMG\\Models\\Gas\\Gas.Products\\PhysicalGasOptions\\get.Index.r") 
  
  # get historical low, mid, high price data from LIM
  histData <- na.omit(get.Index(Index, time.period=list(start.mm.dd, end.mm.dd,start.year) , Verbose=FALSE))
  
  startDate <- as.Date(paste(start.mm.dd,"/",start.year-2000),"%m/%d / %y")
  endDate <- as.Date(paste(end.mm.dd,"/",end.year-2000),"%m/%d / %y")
  startDay <- count(row.names(histData)<=startDate)
  endDay <- count(row.names(histData)<=endDate)
  
  lowP <- histData[startDay:endDay,3]
  highP <- histData[startDay:endDay,2]
  midP <- histData[startDay:endDay,1]
  
  # remove weekends and holidays
  nonTradingDayIndx <- which((diff(highP)+diff(midP)+diff(lowP))!=0) + 1
  lowP <- lowP[nonTradingDayIndx]
  midP <- midP[nonTradingDayIndx]
  highP <- highP[nonTradingDayIndx]
  
  getHistP <- data.frame(lowP,midP,highP,nonTradingDayIndx)
#  getHistP <- nonTradingDayIndx
  
}

###------------------------------------------------------

simP <- function(lo, mid, hi, distr="unif", numTrials)
# lo, mid, hi are vectors
# output is a matrix; columns are trials and rows are days
{
  simP <- numeric()
  numDays <- length(mid)

  if(distr=="unif") for(i in 1:numTrials) simP<-cbind(simP,runif(numDays,lo,hi))
  else if(distr=="tri") for(j in 1:numDays)  simP<-rbind(simP,rtriangle(numTrials,lo[j],hi[j],mid[j]))   
}

###------------------------------------------------------

simV <- function(minVolume, maxVolume, numDays, numTrials)
{
  simV <- numeric()
  for(i in 1:numTrials) simV<-cbind(simV,runif(numDays, minVolume, maxVolume))
}

###------------------------------------------------------

strike <- function(lo, mid, hi, numTrials, strType="gda", Index2, start.mm.dd, end.mm.dd, start.year, end.year, lastDaySeries, nonTradingDayIndx)
{
  # strike is equal to GDA
  if(strType=="gda")  strike <- array(mid,dim=c(length(mid),numTrials))

  # strike is equal to mean of mid and hi
  else if(strType=="meanMidHi") strike <- array((mid+hi)/2,dim=c(length(mid),numTrials))
  
  # strike = higher of 2 indices
  else if(strType=="maxTwoIndex")  {
    histIndex2 <- getHistP(Index2, start.mm.dd, end.mm.dd, start.year, end.year)
    strike <- array(pmax(mid,histIndex2$midP),dim=c(length(mid),numTrials))
    }
  # strike = lower of 2 indices
  else if(strType=="minTwoIndex") {
    histIndex2 <- getHistP(Index2, start.mm.dd, end.mm.dd, start.year, end.year)
    strike <- array(pmin(mid,histIndex2$midP),dim=c(length(mid),numTrials))  
    }
  # strike = FOM index
  else if(strType=="fomIndex") {
    histIndex2 <- getHistP(Index2, start.mm.dd, end.mm.dd, start.year, end.year)
    strike = NULL
    for(k in 1:length(histIndex2$midP))  strike <- rbind(strike,array(histIndex2$midP[k],dim=c(lastDaySeries[k],numTrials)))
    strike <- strike[nonTradingDayIndx]
    }
}
                                               
###------------------------------------------------------       

unitPayoff <- function(simP, simV, strike, numCalls, numTrials,start.mm.dd="11/1", end.mm.dd="3/31",start.year=2005)
{                                                        
  # n calls for entire deal term

  payoff <- (strike-simP)*simV
  
  # we may also make numTrials a variable = f(econ metric)
  unitPayoff <- numeric()
  
  for(j in 1:numTrials){
    i <- order(payoff[,j])
    pvMat <- cbind(payoff[i,j], simV[i,j])
    unitPayoff <- rbind(unitPayoff,sum(pvMat[1:numCalls,1])/sum(pvMat[1:numCalls,2]))     
  }
}

###------------------------------------------------------
                                                           
# write function here that normalizes the results to current price levels
normPayoff <- function(fwdCurve1, priceDate, start.mm.dd, end.mm.dd, deal.start.year, deal.end.year, midP, payoff)
{
 # needs reshape and rodbc packages 

 
 opt = NULL
 opt$pricing.dt.start  <- as.Date(priceDate) 
#  opt$pricing.dt.end  <- as.Date(options$priceDate) 
 opt$contract.dt.start <- as.Date(paste(start.mm.dd,"/",deal.start.year-2000),"%m/%d / %y")
 opt$contract.dt.end <- as.Date(paste(end.mm.dd,"/",deal.end.year-2000),"%m/%d / %y")
 opt$curve.names <- fwdCurve1
 opt$plot <- 0       # plot the results
 opt$type <- "raw"   # "corrected" or "raw"

 source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R")

 fwdP <- read.prices.from.CPSPROD(opt)
 
 normFactor <- mean(fwdP$PRICE)/mean(midP)
 
 normPayoff <- normFactor*payoff
 
}

###------------------------------------------------------

count <- function(x)
{
 length(x[x==TRUE])
}

###------------------------------------------------------

monthsBetween <- function(start.mm.dd, end.mm.dd, start.year, end.year)
{
  startDate <- as.Date(paste(start.mm.dd,"/",start.year-2000),"%m/%d / %y")
  endDate <- as.Date(paste(end.mm.dd,"/",end.year-2000),"%m/%d / %y")

  startDate <- month.day.year(as.numeric(startDate))
  endDate <- month.day.year(as.numeric(endDate))
  
  startDate <- paste(startDate$month,"/",startDate$day,"/",startDate$year)
  endDate <- paste(endDate$month,"/",endDate$day,"/",endDate$year)
  
  monthsBetween <- seq.dates(startDate, endDate, by = "months")
 
}

###------------------------------------------------------

