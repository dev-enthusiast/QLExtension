# This function retrieves historical forward curves for particular futures contracts names

# The function "get.hist.FwdCurve.from.CPSPROD" obtains the historical forward prices (if ShowPrices = TRUE)
# or log-returns ( if ShowPrices = FALSE) for a set of futures contracts
# It returns a list, whose first element is a dataframe of historical returns (no NA), the second one is the date
# of last available prices of format ("YYYY-mm-dd") and the last element is the latest forward curve.
# The function "get.hist.FwdCurve.from.CPSPROD" calls the following fncs "read.prices.from.CPSPROD", and "Uts.Other.contract.letters".


# PLEASE MAKE SURE THE FOLLOWING FUNCTIONS ARE SAVED IN THE APPROPRIATE DIRECTORY OR MAKE THE NECESSARY CHANGES!!
source("H:/user/R/RMG/Models/Gas/Gas.Products/PhysicalGasOptions/Utils.r")
source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.r")

# Application
#get.hist.FwdCurve.from.CPSPROD(commodity1="COMMOD NG EXCHANGE",start.mo.FC=9, start.year.FC=2006, num.months=6, start.date.hist="06/1/2006", Verbose=FALSE, ShowPrices=TRUE)

get.hist.FwdCurve.from.CPSPROD <- function(commodity1,start.mo.FC, start.year.FC, num.months, start.date.hist="05/31/2004", Verbose=FALSE, ShowPrices=TRUE)
{
  require(reshape)
  
  start.date.hist = as.Date(start.date.hist, format="%m/%d/%Y")-1
  start.mo.FC=as.character(start.mo.FC)
  start.year.FC = as.character(start.year.FC)

	# Make a sequence of dates from the first day of the start month (and year) to the date corresponding
	# to num.months later. month.seq and year.seq are the months and years corresponding to these dates.

	date.seq <- seq(as.Date(paste(start.year.FC,"-",start.mo.FC,"-1", sep="")), length=num.months, by="months")

  month.seq <- format(date.seq, "%m")
	year.seq <- format(date.seq, "%Y")

  year.temp = apply(matrix(year.seq,nc=1),1,Uts.String.right,2)

  # Get the contract month letters from "month number"
	month.letters <- as.vector(apply(data.frame(month.seq), 1, Uts.Other.contract.letters))
	
  # Make output column names
  contract.mo.names <- paste(month.letters, year.temp, sep="")
  
  # Populate function input parameters
  options=NULL
  options$pricing.dt.start  <- start.date.hist
  options$pricing.dt.end    <- Sys.Date() - 1
  options$contract.dt.start <- date.seq[1]
  options$contract.dt.end   <- date.seq[num.months]
  options$curve.names <- commodity1
  options$plot <- 0
  options$type <- c("corrected")
  
  cpsfc <- read.prices.from.CPSPROD(options)
  
  cpsfc.temp <- cpsfc[, -2]
  
  colnames(cpsfc.temp)[3] <- "value"
  
  data <- cast(cpsfc.temp, PRICING_DATE ~ START_DATE)
  
  d <- dim(data)
  l <- length(contract.mo.names)
  
  colnames(data)[2:d[2]] <- c(contract.mo.names)
  
  hist.returns <-  apply(log(data[,-1]), 2, diff)
  last.date <- as.Date(data[nrow(data), 1])
  current.fwd <- unlist(data[nrow(data),2:d[2]])
  res = list("Historical Returns"=hist.returns,"Last Available Date"=last.date, "Last Available Fwd Curve"=current.fwd)
  return(res)
  
}