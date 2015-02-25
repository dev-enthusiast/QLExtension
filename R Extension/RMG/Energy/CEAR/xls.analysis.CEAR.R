# Interface used for R-Server and Excel Client
#
# Written by Wang Yu on 7-Jul-2005

xls.analysis.CEAR <- function(BidsFile, SettleFile, OutDir, SimRounds,Trader, Region, Month)
  {
    rm(list=ls())

    Message <- "Analysis Starting"
    save <- NULL
    options <- NULL

    BidsFile <- "S:/All/Risk/Data/FTRs/NEPOOL/Jaya/JBFTRBidStack Jun 05.xls"
    OutDir  <-"S:/All/Risk/Analysis/Zonal.Risk/NEPOOL/CEAR/"
    SettleFile <- "S:/All/Risk/Data/FTRs/NEPOOL/ISO/FTRRSLT_JUN2005.xls"
    Region<-"NEPOOL"
    Trader<-"Jaya"
    Month<-"Jun 2005"
    SimRounds<-"100"

# Guard against different inputs format for dats in Excel.  Ask Excel to convert it to Excel 1900 System (An integer)
#   require(chron)
#   Month <- dates("01/01/1900") + as.numeric(Month)
#   Month <- paste(as.character(months(Month)), as.character(years(Month)),sep=" ")
#
    options$Region <- Region
    options$Trader <- Trader
    options$Month <- Month
    options$xls.inputfile <- BidsFile
    options$xls.settlefile <- SettleFile
    options$noSims        <- as.numeric(SimRounds)
    options$run.name <- paste("FTR", options$Trader, "Bids in", options$Region, "for", options$Month, sep=" ")
    options$tsdb.refresh  <- 0

# Guard against slash problem in file location
    if (substring(OutDir,nchar(OutDir)) != "/") {OutDir <- paste(OutDir,"/",sep="")}
    save$dir               <- OutDir

    source("H:/user/R/RMG/Energy/CEAR/set.parms.model.R")
    aux <- set.parms.model(save, options)
    save <- aux[[1]];  options <- aux[[2]]

  #  source("H:/user/R/RMG/Energy/CEAR/position.history.R")
#    if (options$Region == "NEPOOL"){}

    source("H:/user/R/RMG/Energy/CEAR/main.analysis.CEAR.R")
    aux <- main.analysis.CEAR(save, options)

    Position <- aux[[1]]; sim.prices <- aux[[2]]; hist.prices <- aux[[3]]
    save <- aux[[4]]; options <- aux[[5]]

    source("H:/user/R/RMG/Energy/CEAR/payoff.FTR.R")
    sim.unit.sum.Payoff <- payoff.FTR(sim.prices, Position)

    source("H:/user/R/RMG/Energy/CEAR/hist.payoff.FTR.R")
    hist.unit.sum.Payoff <- hist.payoff.FTR(hist.prices, Position)

    source("H:/user/R/RMG/Energy/CEAR/report.CEAR.R")
    report.CEAR(sim.unit.sum.Payoff, hist.unit.sum.Payoff, Position, save, options)

    source("H:/user/R/RMG/Energy/CEAR/make.pdf.R")
    Message <- make.pdf(save, options)

    return(Message)

}
