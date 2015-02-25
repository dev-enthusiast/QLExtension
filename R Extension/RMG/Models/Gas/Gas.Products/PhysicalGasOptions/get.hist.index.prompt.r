# Obtain historical Prompt months of a given commodity and corresponding index prices

get.hist.index.prompt <- function(Index= "Index of IGBBL00", Prompt="Close of NG", start.date.hist="05/31/2004", Verbose=FALSE)
{
  # PLEASE MAKE SURE THE FOLLOWING FUNCTIONS ARE SAVED IN THE APPROPRIATE DIRECTORY OR MAKE THE NECESSARY CHANGES!!
  source("H:\\user\\R\\RMG\\Models\\Gas\\Gas.Products\\PhysicalGasOptions\\read.LIM.r")     #          S:\\Risk\\Temporary\\Fundamentals\\
  
  start.date.hist = format(as.Date(start.date.hist, format="%m/%d/%Y")-1, "%m/%d/%Y")

	# Make the query for simple futures
  if(!is.na(Index) & !is.na(Prompt)) {
    query <- paste("SHOW", "\n","Index:", Index, "\n", "Futures:" , Prompt, "\n", "WHEN Date is after ",
  start.date.hist, "\n",sep="")
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


