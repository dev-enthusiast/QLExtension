# Keep some useful functions behind the scenes.
#
#
#

# Make an IVaR data.frame from a general set of positions.
# It's a copy of main.run.VaR.R.  Not a good practice to
# duplicate code.  
#
make.IVaR.from.positions <- function(QQ, asOfDate, run.name){

  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  options <- overnightUtils.loadOvernightParameters(asOfDate)
  
  cat(paste("Loading curve info ... "))
  load(options$save$curve.info)    # has denomination and region mapping, etc. 
  cat("Done!\n")
  cat(paste("Loading fx spot data ... "))
  load(options$save$fx.file)
  cat("Done!\n")

  run <- NULL
  run$run.name <- run.name
  run$specification <- data.frame(PORTFOLIO="", BOOK="", CLASS=NA)
  res <- set.parms.run(run, options)
  run <- res[[1]]; options <- res[[2]]

  names(QQ)[3] <- "position"    # <-- may not be in column no. 3!
  QQ$vol.type  <- "NA"
  class.Factors <- make.class.Factors(QQ, run, options)

  hPP <- hVV <- PnL <- NULL
  hPP <- aggregate.run.prices.mkt(QQ, options)
  hVV <- aggregate.run.vols.mkt(QQ, options)
  
  if (length(hVV)!=0){IVaR <- rbind(hPP, hVV)} else {IVaR <- hPP}
  options$parallel$is.necessary <- FALSE
  
  IVaR <- convert.prices.to.USD(IVaR, den, hFX)   
  IVaR <- add.price.vol.fillings(IVaR, options)  # filling of last resort
  if (length(options$run$is.bidweek.day)>0 && options$run$is.bidweek.day){
    IVaR <- adjust.bidweek(IVaR, options)}
  
  IVaR$Changes <- cbind(IVaR$Changes, CVaR=NA)
  return(list(IVaR, options, run))
}

#================================================================
#
report.VaR <- function(PnL, run, options){
  PnL <- list(PnL)   # pretend you have only 1 packet
  cat("Start VaR tables ... ")    
  source(paste(options$source.dir,"Base/make.VaR.tables.R",sep=""))
  make.VaR.tables(PnL, run, options); cat("Done!\n")
  if (options$calc$do.CVaR){
    cvar <- calc.CVaR(PnL, IVaR, run, class.Factors, options)}
    IVaR$Changes$CVaR <- cvar
  if (options$calc$do.incremental.VaR){
    calc.incremental.VaR(PnL, IVaR, run, class.Factors, options)}
  if (options$calc$do.corr.sensitivity){
    calc.correlation.sensitivity(IVaR, options)}
  
  setwd(options$save$reports.dir.run)
  source(paste(options$source.dir,"Base/make.pdf.R",sep=""))
  cat("Start pdf ... ")
  make.pdf(run, options)
  cat("Done!\n")
  
  if ((!options$parallel$is.necessary) &&  #-----save IVaR results
     (length(options$save$IVaR.filename)>0)){
#    write.xls(IVaR$Prices, options$save$IVaR.filename, sheetname="Prices")
    write.xls(IVaR$Changes, options$save$IVaR.filename, sheetname="Changes")
  }
}

#================================================================
#
load.base.portfolio.IVaR <- function(asOfDate, base.portfolio){

  require(RODBC)
  run.name <- gsub(" ", ".", tolower(base.portfolio))
  filename <- paste("S:/All/Risk/Reports/VaR/prod/", asOfDate,
    "/", run.name, "/IVaR.", run.name, ".xls", sep="")

  if (file.exists(filename)){
    con  <- odbcConnectExcel(filename)
    IVaR <- sqlFetch(con, "Changes")
    odbcCloseAll()
  } else {
    stop(paste("Cannot find IVaR file:", filename))
  }
  names(IVaR) <- gsub("#", ".", names(IVaR))
  IVaR$curve.name  <- as.character(IVaR$curve.name)
  IVaR$contract.dt <- as.Date(IVaR$contract.dt) 
  IVaR$vol.type    <- as.character(IVaR$vol.type)
  IVaR$vol.type[is.na(IVaR$vol.type)] <- "NA"  # bad legacy code
  return(IVaR)
}

