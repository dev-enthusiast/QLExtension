#
#  source("H:/user/R/RMG/Models/AuctionRaroc/dealvars.r")
#

deal.var.wrap <- function() {
    require(reshape)
    require(SecDb)
    require(xlsReadWrite)
    file <- "H:/user/R/RMG/Models/AuctionRaroc/AuctionRaroc.xls"
    input.type <-  c("isodate", "character", "character", "character", "character", "character", "character", "character", "character", "character", "character", "character", "character")
    inputs <- read.xls(file, colClasses=input.type, sheet="InputToModel")
  
    asOfDate <- as.Date(inputs[1, 1])
    calcs <- subset(inputs, calculator_weight!='0')[, c("group", "trader", "calc")] #may need some additional selection posibilities here
    portfolio <- tolower(gsub(' ', '.', subset(inputs, portfolio_selected =='x')[1,"portfolio"]))  
    
    res <- get.raroc.inputs(calcs, portfolio)
    
    return(res)
}

get.calculators <- function() {
    cat("Get Calculctor Starts ... ")
    require(xlsReadWrite)  
    file <- "H:/user/R/RMG/Models/AuctionRaroc/AuctionRaroc.xls"
    input.type <-  c("isodate", "character", "character", "character", "character", "character", "character", "character", "character", "character", "character", "character", "character")
                   
    inputs <- read.xls(file, colClasses=input.type, sheet="InputToModel")

    calculators <- get.calcs(group=inputs[1,"group"], trader=inputs[1,"trader"], calc="")
    calculators$weight <-1
    head(calculators)
    cat("Get Calculctor Ends ... ")
    return(calculators)
}

get.portfolios <- function() {
   cat("Get Portfolio Starts ... ")
   require(RODBC)
   source.dir <- "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/"
   file <- paste(source.dir,"Base/schedule.overnight.xls", sep="")
   con <- odbcConnectExcel(file)
   portfolios <- sqlFetch(con, "RMG_R_VAR_RUN")               
   odbcCloseAll()
   
   portfolios <- data.frame(subset(portfolios, ACTIVE=='Y')[, "RUN_NAME"] )
   names(portfolios) <- "Portfolio"
   cat("Get Portfolio Ends ... ")
   return(portfolios)
}

get.calcs <- function(group="CPS Marketing Group", trader=c("DC PEP"), calc="Credit"){
  require(SecDb)
  source("//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Tools/utils.combine.VaR.R")
  source("H:/user/R/RMG/Utilities/Interfaces/SecDb/R/SecDb.R")

  calcs <- NULL
  for (i in 1:length(trader)) {
    all.calcs <- calculator.list(group, trader[i])
    for (j in 1:length(calc)) {
      ind <- grep(calc[j], all.calcs)
      calc.names <- all.calcs[ind]
      calcs <- rbind(calcs,cbind(rep(group, length(calc.names)), rep(trader[i], length(calc.names)), calc.names))
    }
  }
  colnames(calcs) <- c("group", "trader", "calc")
  return(as.data.frame(calcs))
}

get.deltas <- function(calcs){
  require(SecDb)
  source("//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Tools/utils.combine.VaR.R")
  source("H:/user/R/RMG/Utilities/Interfaces/SecDb/R/SecDb.R")
  expected.blocks <- rep(1,nrow(calcs))
  calcs <- cbind(calcs, expected.blocks)

  QQ <- NULL
  for (i in 1:nrow(calcs)) {
    Q <-  calculator.get.deltas(as.character(calcs$group[i]), as.character(calcs$trader[i]), as.character(calcs$calc[i]))
    Q$curve.name <- as.character(Q$curve.name)
    Q$contract.dt <- as.Date(Q$contract.dt)
    Q$delta <- Q$delta * calcs$expected.blocks[i]
    QQ <- rbind(QQ,Q)
  }

  return(QQ)
}

first.workday <- function(date){
# returns the first date before the given date that is not a holiday or a weekend.
  require(SecDb)
  date.series <- rev(seq(from=date-7, to=date, by="days"))
  ind <- which(secdb.isHoliday(date.series)==FALSE)[1]
  return(date.series[ind])
}

calc.var <- function(deltas.and.prices,year=F){
    absolute.returns <- deltas.and.prices[-(1:3)]
    value.changes <- deltas.and.prices$delta * (absolute.returns)
    factor.year <- as.numeric(substr(deltas.and.prices$contract.dt,1,4))
    total.daily.change <- colSums(value.changes)
    daily.sd <- sd(total.daily.change)
    VaR <- 4 * daily.sd
    if (year==T) {
      VaR <- c(VaR,4*apply(aggregate(value.changes,by=list(factor.year),FUN=sum),1,sd))
      names(VaR) <- c("Total", as.character(unique(factor.year)))   #need to make sure labels match right var's
#      VaR <- data.frame(c("Total", as.character(unique(factor.year))), VaR)
#   colnames(VaR)[1] <- "year"
    }
    return(as.data.frame(VaR))
}


get.raroc.inputs <- function(calcs, portfolio){

  require(reshape)
  require(SecDb)

  get.VaR.and.deltas <- function(calc){
    deltas <- get.deltas(calc) #same structure as QQ in Adrians code?
    curve.names <- unique(deltas$curve.name)
    contract.dts <- unique(deltas$contract.dt)
    histEnd.dt <- first.workday(Sys.Date()-1)
    histStart.dt <- histEnd.dt-(70/5*7+10)
    curveData = tsdb.futStrip(curve.names, histStart.dt, histEnd.dt, contract.dts )
    reshaped.data <- cast(curveData, curve.name + contract.dt ~ pricing.dt)
    prices <- reshaped.data[-(1:3)]
    prices <- prices[,max(1,(ncol(prices)-70)):ncol(prices)] # make sure we have data from 71 days
    absolute.returns <- t(apply(prices,1,diff))
    colnames(absolute.returns) <- colnames(prices)[2:ncol(prices)]
    transformed.data <- cbind(reshaped.data[,1:2], absolute.returns)
    combined.data <- merge(deltas, transformed.data)
    VaR <- calc.var(combined.data)
    curve.class <- sub("[0-9]X[0-9]+|[0-9]x[0-9]+H","",deltas$curve.name,ignore.case=TRUE)
    curve.class <- sub("  "," ",curve.class)
    aggregate.deltas <- aggregate(deltas$delta, by=list(curve.class), FUN=sum)
    colnames(aggregate.deltas) <- c("asset", "delta")
    res <- NULL
    res$aggregate.deltas <- cbind(matrix(rep(calc,nrow(aggregate.deltas)),ncol=3,byrow=T),aggregate.deltas)
    colnames(res$aggregate.deltas)[1:3] <- colnames(calc)
    res$VaR <- VaR
    res$deltas.and.price.changes <- combined.data
    return(res)
  }
  #calcs <- get.calcs(group, trader, calc) #may need some additional selection posibilities here
  VaR <- cbind(calcs, matrix(0,nrow=nrow(calcs),ncol=1))
  colnames(VaR)[4] <- "VaR"
  for (i in 1:nrow(calcs)){
    res <- get.VaR.and.deltas(calcs[i,])
    if (i==1) {
      aggregate.deltas <- res$aggregate.deltas
      deltas.and.price.changes <- res$deltas.and.price.changes
    }
    else {
      aggregate.deltas <-rbind(aggregate.deltas,res$aggregate.deltas)
      deltas.and.prices <- rbind(deltas.and.price.changes, res$deltas.and.price.changes)
    }
    VaR[i,4] <- res$VaR
  }
  deal.var <- calc.var(deltas.and.price.changes,year=T)
  colnames(deal.var) = "deal"
  ivar <- deal.plus.portfolio.var(deltas.and.prices, portfolio)
  colnames(ivar) <- "deal.and.portfolio"
  all.var <- merge(ivar, deal.var ,all=FALSE, by.x="row.names", by.y="row.names")
  rownames(all.var) <- all.var$Row.names
  all.var <- all.var[-1]
  portfolio.var <- portfolio.var2(portfolio)
  colnames(portfolio.var) <- "portfolio"
  all.var <- merge(all.var, portfolio.var, all=TRUE, by.x="row.names", by.y="row.names")
  colnames(all.var)[c(1)] <- "year"
  res <- NULL
  res$VaR <- VaR
  res$deltas <- aggregate.deltas
  res$port.VaR <- all.var
  return(res)
}

deal.plus.portfolio.var <- function(deal.data, portfolio="rmg.gandhi.total.east.portfolio",
    asOfDate=first.workday(Sys.Date()-1)){
# data consists of deltas and price changes as well as curve names and contract dates
  IVaR.base <- load.base.portfolio.IVaR(asOfDate, portfolio)  #portfolio deltas and price changes
  portfolio.data <- IVaR.base[c(-3,-(ncol(IVaR.base)-1), -ncol(IVaR.base))]
  colnames(portfolio.data)[3] <- "delta"
  pricing.dates <- as.Date(colnames(portfolio.data)[4:ncol(portfolio.data)])
  histEnd.dt <- max(pricing.dates)
  histStart.dt <- histEnd.dt-(70/5*7+10)

  all.data <- rbind(portfolio.data, deal.data)
  VaR <-  calc.var(all.data,year=T)
  return(as.data.frame(VaR))

}

portfolio.var <- function(base.portfolio="rmg.gandhi.total.east.portfolio",asOfDate=first.workday(Sys.Date()-1)){
  dir <- "S:\\All\\Risk\\Reports\\VaR\\prod\\"
  load(paste(dir,as.character(asOfDate),"\\",base.portfolio,"\\all.VaR.RData",sep=""))
  res <- NULL
  res <- rbind(VaR$Total, VaR$year$Total)
  rownames(res) <- res$year
  rownames(res)[1] <- "Total"
  res <- res[-1]
  return(as.data.frame(res))
}

portfolio.var2 <- function(portfolio="rmg.gandhi.total.east.portfolio",asOfDate=first.workday(Sys.Date()-1)){
  IVaR.base <- load.base.portfolio.IVaR(asOfDate, portfolio)  #portfolio deltas and price changes
  portfolio.data <- IVaR.base[c(-3,-(ncol(IVaR.base)-1), -ncol(IVaR.base))]
  colnames(portfolio.data)[3] <- "delta"
  pricing.dates <- as.Date(colnames(portfolio.data)[4:ncol(portfolio.data)])
  VaR <-  calc.var(portfolio.data,year=T)
  return(as.data.frame(VaR))
}
