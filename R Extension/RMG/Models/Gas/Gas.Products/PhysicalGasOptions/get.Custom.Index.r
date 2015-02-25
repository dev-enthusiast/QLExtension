# The function "get.Index" retrieves an Index prices, its low and high prices for a particular location
# Weekend prices and holidays are the last trading day prices.
# Index "IGBBL00" is Henry Hub

get.Custom.Index <- function(Index= "(Index of IGBBL00+Index of IGBEK00)/2", time.period=list(start.mm.dd="9/1", end.mm.dd="10/31",start.year=1996) , Verbose=FALSE)
{
  # PLEASE MAKE SURE THE FOLLOWING FUNCTIONS ARE SAVED IN THE APPROPRIATE DIRECTORY OR MAKE THE NECESSARY CHANGES!!
  source("H:\\user\\R\\RMG\\Models\\Gas\\Gas.Products\\PhysicalGasOptions\\read.LIM.r")   # Used to be:"S:\\Risk\\Temporary\\Fundamentals\\read.LIM.r"
  source("H:\\user\\R\\RMG\\Models\\Gas\\Gas.Products\\PhysicalGasOptions\\Utils.r")   # Used to be:"S:\\Risk\\Temporary\\Fundamentals\\read.LIM.r"

  if(!is.na(Index)) {
    query <- paste("%exec.missingdatananfill: Fill_Forward \n", "%exec.holidaynanfill: Fill_Forward \n",
    "SHOW", "\n",
    "Index: ", Index, "\n", 
    "Hi: " ,Uts.String.sub("Index", "High", Index) , "\n", 
    "lo: " ,Uts.String.sub("Index", "Low", Index), "\n", 
    "WHEN Date is from ", time.period[[1]], "/_ to ", time.period[[2]], "/_", "\n",
    "and Date is after ",time.period[[3]]-1 ,sep="")
  }

  if (Verbose) cat(query)

	data.lim <- read.LIM(query)
  data.lim=na.omit(data.lim)

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

  prices  <- data.lim[,-1]
  rownames(prices) <- data.lim[,1]
  return(prices)
}

