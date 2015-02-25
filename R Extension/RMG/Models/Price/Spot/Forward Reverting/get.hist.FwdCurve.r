# This function retrieves historical forward curves for particular futures contracts names

# The function "get.hist.FwdCurve" obtains the historical forward prices (if ShowPrices = TRUE)
# or log-returns ( if ShowPrices = FALSE) for a set of futures contracts
# It returns a list, whose first element is a dataframe of historical returns (no NA), the second one is the date
# of last available prices of format ("YYYY-mm-dd") and the last element is the latest forward curve.
# The function "get.hist.FwdCurve" calls the following fncs "read.LIM", "Uts.String.right", and "Uts.Other.contract.letters".


# PLEASE MAKE SURE THE FOLLOWING FUNCTIONS ARE SAVED IN THE APPROPRIATE DIRECTORY OR MAKE THE NECESSARY CHANGES!!
source("S:/Risk/Temporary/Fundamentals/Utils.r")
source("S:/Risk/Temporary/Fundamentals/read.LIM.r")

# Application
#get.hist.FwdCurve(commodity1="Close of NG",start.mo.FC=9, start.year.FC=2006, num.months=6, start.date.hist="06/1/2006", Verbose=FALSE, ShowPrices=TRUE)

get.hist.FwdCurve <- function(commodity1,start.mo.FC, start.year.FC, num.months, start.date.hist="05/31/2004", Verbose=FALSE, ShowPrices=TRUE)
{

  start.date.hist = format(as.Date(start.date.hist, format="%m/%d/%Y")-1, "%m/%d/%Y")
  start.mo.FC=as.character(start.mo.FC)
  start.year.FC = as.character(start.year.FC)

	# Make a sequence of dates from the first day of the start month (and year) to the date corresponding
	# to num.months later. month.seq and year.seq are the months and years corresponding to these dates.

	date.seq <- seq(as.Date(paste(start.year.FC,"-",start.mo.FC,"-1", sep="")), length=num.months, by="months")

  month.seq <- format(date.seq, "%m")
	year.seq <- format(date.seq, "%Y")

  year.temp = apply(matrix(year.seq,nc=1),1,Uts.String.right,1)

  # Get the contract month letters from "month number"
	month.letters <- as.vector(apply(data.frame(month.seq), 1, Uts.Other.contract.letters))

	# Make the query for simple futures
  if(!is.na(commodity1)) {
    contract <- paste(month.letters, year.temp, sep="")
  ATTR <- paste(contract,": IF ", commodity1, "_", year.seq, month.letters, " is defined THEN ",
  commodity1, "_",  year.seq, month.letters, " ENDIF", "\n",  sep="")
  }

	query <- paste("SHOW", "\n", paste(eval(as.name(paste("ATTR", sep=""))), collapse="\n"), "\n", "WHEN Date is after ",
  start.date.hist, "\n",sep="")
	if (Verbose) cat(query)

	data.lim <- read.LIM(query)


	if(nrow(data.lim)==0)
	{
		print(data.lim)
		message("There are no prices for the specified period. Please select another day.")
		return()
	}

   	if(length(which(!is.na(unlist(data.lim[1,2:length(data.lim[1,])]))))==0 & length(data.lim[,1])==1)
 	{
 		junk <<- data.lim
 		message("There are no prices for the specified period. Please select another day.")
 		stop("Error")
 	}

   if(ShowPrices == TRUE){
  hist.prices  <- data.lim[,-1]
  rownames(hist.prices) <- data.lim[,1]
  last.date <-  as.Date(rownames(hist.prices)[nrow(hist.prices)])
  current.fwd <- unlist(hist.prices[nrow(hist.prices),])
  res = list("Historical Prices"=hist.prices,"Last Available Date"=last.date, "Last Available Fwd Curve"=current.fwd)
  return(res)
  }else{
  hist.prices  <- data.lim[,-1]
  rownames(hist.prices) <- data.lim[,1]
  hist.prices <- na.omit(hist.prices)
  hist.returns <- apply(log(hist.prices),2,diff)
  last.date <-  as.Date(rownames(hist.prices)[nrow(hist.prices)])
  current.fwd <- unlist(hist.prices[nrow(hist.prices),])
  res = list("Historical Returns"=hist.returns,"Last Available Date"=last.date, "Last Available Fwd Curve"=current.fwd)
  return(res)
  }
}

