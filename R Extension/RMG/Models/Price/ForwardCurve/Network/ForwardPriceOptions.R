################################################################################
# These are the generic options used in for Forward Price simulation
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
try(load.packages( c("SecDb", "MASS", "rstream", "filehash") ))
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")
source("H:/user/R/RMG/Statistics/estimate.OU.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")


################################################################################
# File Namespace
#
ForwardPriceOptions = new.env(hash=TRUE)


################################################################################
# This is called create and not get because we're not actually storing the
# options list and returning it to them. 
# 
ForwardPriceOptions$create <- function(asOfDate=NULL, isInteractive=FALSE,
                                       run.name=NULL)
{
  optionsList <- NULL
  
  ## simulate from last business day
  if(is.null(asOfDate))
  {
    optionsList$asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
  } else
  {
    optionsList$asOfDate = as.Date(asOfDate)
  }  
  optionsList$isInteractive <- isInteractive
  optionsList$run.name <- run.name
  if (!is.null(run.name)){run.name <- paste(run.name, "/", sep="")}
  
  ## number of historical days used
  optionsList$start.dt.hist <- as.Date("2006-01-03") # when the hash starts
  optionsList$N.hist <- as.numeric(optionsList$asOfDate - 
                                   optionsList$start.dt.hist)  
  
  ## number of future contracts simulated 
  optionsList$D <- 48 
  
  ## number of simulations
  optionsList$nsim <- 1000 
  
  ## number of principal components used for PCA over contract dates
  optionsList$Kpc.month <- 15 
  
  ## number of PCs used for PCA accros curves, after the PCA over
  ## contract dates is done
  optionsList$Kpc.curve <- 5 
  
  ## the mode that the simulator will run in, basically where it writes data
  runMode = ifelse(isInteractive, "/interactive/", "/overnight/")
  
  ## dir for RData files 
  optionsList$srcdata.dir <- "S:/All/Risk/Data/VaR/prod/Prices/Market/"
  optionsList$hpdata.dir  <- paste( "R:/SimPrices/Network/",
    optionsList$asOfDate, "/overnight/swap/", sep="")
  
  ## target dir for data saving
  optionsList$targetdata.dir <- paste( "R:/SimPrices/Network/", 
    optionsList$asOfDate, runMode, run.name, "result/", sep="" )
  if (file.access(optionsList$targetdata.dir)!=0){ # needed for interactive
    mkdir(optionsList$targetdata.dir)} 
  
  ##  dir for saving temporary data
  optionsList$swapdata.dir <- paste( "R:/SimPrices/Network/", 
    optionsList$asOfDate, runMode, run.name, "swap/", sep="" )
  if (file.access(optionsList$swapdata.dir)!=0){ # needed for interactive
    mkdir(optionsList$swapdata.dir)}
  
  if (file.access(optionsList$hpdata.dir)!=0){ # needed for interactive
    mkdir(optionsList$hpdata.dir)} 
  
  ##  dir for report files
  optionsList$report.dir <- paste( "R:/SimPrices/Network/", 
    optionsList$asOfDate, runMode, run.name, "report/", sep="" )
  
  ## forward pricing dates
  optionsList$sim.days <- makePricingDates(optionsList$asOfDate) 

  ## forward contract dates (next 48 months, exclude the cash month)
  ct.mon <- seq(as.Date(format(optionsList$asOfDate, "%Y-%m-01")), by="month",
                length.out=2)[2]
  optionsList$contract.dts <- seq(ct.mon, by="month", length.out=optionsList$D)

  filehashOption(defaultType = "RDS")
  dbCreate(optionsList$targetdata.dir)
  
  return(optionsList)
}


