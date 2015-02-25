################################################################################
# This file generates two tables showing the largest volatility gainers and
# losers for a given day.
#
generateGainersLosersData.main <- function()
{
  # Load the required overnight parameters
  rm(list=ls())
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  asOfDate   <- overnightUtils.getAsOfDate()
  options = overnightUtils.loadOvernightParameters(asOfDate)

  # Load today's implied volatility data
  todaysImpliedVolFile = paste(options$save$datastore.dir,"Vols/hVols.",
    asOfDate,".RData", sep="")
  load(todaysImpliedVolFile)
  today = hV
  
  # Load the previous business day's implied volatility data
  previousBusinessDay = options$calc$previous.day
  previousImpliedVolFile = paste(options$save$datastore.dir,"Vols/hVols.",
    previousBusinessDay,".RData", sep="")
  load(previousImpliedVolFile)
  yesterday = hV
  
  # Merge the two days' tables
  joinedData = merge(today, yesterday)
  names(joinedData) = c("curve.name", "contract.dt", "vol.type", "ImpliedVol",
    "PreviousImpliedVol")
  
  # Make it a percentage
  joinedData$PercentChange = ( ( joinedData$ImpliedVol - joinedData$PreviousImpliedVol ) 
    / joinedData$PreviousImpliedVol ) * 100
    
  # Sort on the largest +/- PercentChange and create the output tables
  print("Sorting table and generating min/max results...")
  source("H:/user/R/RMG/Energy/Reports/Hist_Implied_Vols/sortTableByColumn.R")
  gainersTable = sortTableByColumn(joinedData, joinedData$PercentChange, 20, TRUE)
  losersTable = sortTableByColumn(joinedData, joinedData$PercentChange, 20, FALSE)

  # Format it for output
  gainersTable$PercentChange = sprintf("%.2f%%", gainersTable$PercentChange)
  losersTable$PercentChange = sprintf("%.2f%%", losersTable$PercentChange)
  
  print("Writing csv files...")
  gainersFileName = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "VolatilityLargestGainersTable.csv", sep="")
  write.csv( gainersTable, file=gainersFileName, row.names=F)
  print(paste("Wrote:",gainersFileName))
    
  losersFileName = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "VolatilityLargestLosersTable.csv", sep="")
  write.csv( losersTable, file=losersFileName, row.names=F)
  print(paste("Wrote:",losersFileName))
  
  print("generateGainersLosersData.main() Done.")

}


################################################################################
# Start the program
#
res = try(generateGainersLosersData.main())
if (class(res)=="try-error"){
  cat("Failed Generate Gainers Losers Data Job.\n")
}
