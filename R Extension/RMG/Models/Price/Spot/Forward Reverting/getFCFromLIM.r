# This function retrieves historical forward curves for months 1:N out, not for specific futures contracts names.

#month.num = c(7:12,1:6)
#monthNames =  months(as.Date(paste(month.num, "/1/2000", sep = ""),format="%m/%d/%Y"), TRUE)
#FC <- getFCFromLIM("Close of NG", mosFwd=12, monthNames=monthNames,Verbose =T)

# FC is a list, FC[[1]] is a vector name of monthNames (numeric though).
# FC[[2]] is a vector of futures contracts prices.           

getFCFromLIM <- function(theSymbol="Close of NG", mosFwd=12,
                               monthNames=c("Jun","Jul","Aug","Sep","Oct","Nov","Dec","Jan","Feb","Mar","Apr","May"),
                               Verbose =F)
{
  if(!is.loaded("xmim_query_execute")) {
    dyn.load("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")
    source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
  }

  if(length(monthNames) != mosFwd) stop("The length of monthNames must be equal to the number of mosFwd")

  query <- "SHOW "
  for (i in 1:mosFwd) {
      query <- paste(query, "\n ", i, ": adjust_contract(", theSymbol, ",", i,",0,0)", sep="")
  }
  query <- paste(query, "\n WHEN Date is Close of NG last_data_day", sep="" )
  if (Verbose) cat(query)
  numCols <- 1+mosFwd
  dataBack <- read.LIM(query)
  theValues <- t(dataBack[1,2:numCols])
  return(data.frame(as.character(monthNames),theValues))

}
#• CL = base commodity symbol of choice.
#• 1 = N nearest. 3 would imply the 3rd nearest, 6 the 6th nearest.
#• 0 = This is “Number of days before Expiration”. Setting this number to “0” indicates the series will locate the
#next nearest contract when the contract expires. Setting this number to “4” would mean that the contract
#should roll to the next one 4 days before expiration of the current contract.
#• 12 = This is the contract month specification, Numerical values represent certain months: 12 equals
#December, 5 equals May, 7 equals July and so on for values between 1-12. Note: Setting this number to “0”
#makes the parameter inactive, meaning the other parameters will determine the behavior.