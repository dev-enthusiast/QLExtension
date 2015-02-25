# xls wrapper of price.CDS.R 
#
#
# Written by Adrian Dragulescu on 12-Mar-2007


archive.results <- function(data, options){
  run <- NULL
  run$run.time <- Sys.time()
  run$user.ID  <- Sys.getenv("USERNAME")
  run$official <- options$is.official      # when explicitly saved from the excel GUI
  run$data     <- data
  file <- "S:/All/Risk/Analysis/CDS/archive.cds.results.RData"
  if (file.exists(file)){load(file)}
  if (!exists("arch")){arch <- NULL}
  arch <- c(arch, run)
  save(arch, file=file)
  cat("Archived run results successfuly.\n")
}


xls.price.CDS <- function(parms, PD){
  require(RODBC)
  rownames(parms) <- parms[,1]
  parms[,1] <- as.character(parms[,1]); parms[,2] <- as.character(parms[,2]) 
  data <- NULL
  data$asOfDate     <- as.Date("2006-01-01") + as.numeric(parms["as of date","Value"]) - 38718 
  data$start.date   <- as.Date("2006-01-01") + as.numeric(parms["start date","Value"]) - 38718 
  data$end.date     <- as.Date("2006-01-01") + as.numeric(parms["end date","Value"]) - 38718
  data$notional     <- as.numeric(parms["notional","Value"])
  data$deal.spread  <- as.numeric(parms["deal spread (bps)","Value"])
  data$buy.sell     <- as.character(parms["buy/sell","Value"])
  data$payment.freq <- as.character(parms["premium payment frequency","Value"])
  data$recovery.rate <- as.numeric(as.character(parms["recovery rate","Value"]))  
  data$prob.default  <- na.omit(data.frame(time=PD[,1], cdf=PD[,2]))
  if (data$prob.default$time[1]!=0){
     data$prob.default  <- rbind(c(0,0), data$prob.default)}  # start from zero! 
  
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
  source("H:/user/R/RMG/Finance/FixedIncome/price.CDS.R")
  options <- NULL; options$errors <- "None"
  res <- try(price.CDS(data, NULL, options))
  return(res)
}
