#
# Vita has NOT booked the TrSch2 in SecDb.  We keep a separate line-adjustment with Riker.
# We should fix this -- AAD 9/12/2013
#
#
# Historical cost info in the spreadsheet from:
#   http://www.iso-ne.com/stlmnts/iso_rto_tariff/rate_dev/index.html 
#
# TR Sch2 is customer volumetric charge 
# TR Sch3 is customer non-coincident peak load charge
#
# get_positions_trsch23
# pull_marks_trsch23
#


################################################################
# Calculate PnL impact on marks change
# Where qq is a data.frame with columns c("service", "month", "location", "qty")
#
calc_pnl_impact_trsch2 <- function(day1, day2, 
   endDate=as.Date("2017-12-01"), do.format=TRUE)
{
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg.load",
                 get_positions_trsch23(day1, books, service="TR SCH2"))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne.load",
                 get_positions_trsch23(day1, books, service="TR SCH2"))
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Gen Portfolio" )
  qqC   <- cbind(company="ccg.gen",
                 get_positions_trsch23(day1, books, service="TR SCH2"))
  qq1   <- rbind(qqA, qqB, qqC)
  qq1   <- subset(qq1, month >= nextMonth())

  
  PP1 <- pull_marks_trsch23(day1,  nextMonth(), endDate, service="TR SCH2")
  colnames(PP1)[which(colnames(PP1)=="value")] <- "P1"

  PP2 <- pull_marks_trsch23(day2, day2, endDate, service="TR SCH2")
  colnames(PP2)[which(colnames(PP2)=="value")] <- "P2"

  aux <- calc_pnl_pq(PP1, PP2, qq1, qq1,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  
  rLog("Total PnL change is", sum(gPnL$PnL))
  
  aux
}


################################################################
# get positions for a given portfolio
# or "TR SCH3"
get_positions_trsch23 <- function(asOfDate, books, service="TR SCH2")
{
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, fix=TRUE,
    service=service, books=books)

  qq <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
    value="notional")
  names(qq) <- c("month", "location", "qty")

  qq <- cbind(service=service, as.data.frame(qq))

  subset(qq, location != "NULL")  # you can end up with crap
}



#########################################################################
#
#
pull_marks_trsch23 <- function(asOfDate=Sys.Date(),
   startDate=as.Date(format(Sys.Date(), "%Y-%m-01")),
   endDate=as.Date("2020-12-01"), service=c("TR SCH2", "TR SCH3"))
{
  rLog("Pull TRSCH2,3 marks as of", format(asOfDate))
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- " "      # <-- "" does not work!!!
  bucket      <- c("FLAT")
  DD <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType=service, useFutDates)
  
  DD
}






#########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)
  require(lattice)
  require(PM)

  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Ancillaries/lib.trsch23.R")
 
  asOfDate  <- as.Date("2013-12-20")
  day1 <- Sys.Date()

  PP <- pull_marks_trsch23(service="TR SCH2")
  print(PP[,c("month", "value")], row.names=FALSE)
  
  
  portfolio <- "Nepool Load Portfolio"
  portfolio <- "PM East Retail Portfolio"
  portfolio <- "Nepool Gen Portfolio"
  books <- secdb.getBooksFromPortfolio(portfolio)
  books <- "MYSTC89M"
  # Vita has not booked the TrSch2 in SecDb.  We keep a separate line-adjustment with Riker.
  # We should fix this -- AAD 9/12/2013
  QQ <- get_positions_trsch23(asOfDate, books=books)

  res <- cast(QQ, month ~ service, I, fill=0, value="qty")
  print(res, row.names=FALSE)  # put into spreadsheet
  

  calc_pnl_impact( as.Date("2012-05-15"), as.Date("2012-05-16"),
     portfolio=portfolio )

  


  

  
    
  #====================================================================
  # get marks  
  asOfDate  <- as.Date("2013-05-03")
  DD <- pull_marks_trsch23(asOfDate=asOfDate, service="TR SCH2")
  
  DD <- pull_marks_trsch23(asOfDate=asOfDate, endDate=as.Date("2020-12-01"))


  xyplot(value ~ month, data=DD, groups=serviceType, type=c("g", "b"),
    ylab="TrSch 2 and 3, $/MWh",      
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(DD$location))))

  dd <- cast(DD, month ~ serviceType, I, fill=0, value="value")
  print(dd, row.names=FALSE)
  
  


  

}


  ## startDate <- as.Date("2012-07-01")
  ## endDate   <- as.Date("2012-12-01")
  ## serviceType <- "TR SCH2"
  

