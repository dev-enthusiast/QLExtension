################################################################################
# This file generates two tables showing the largest volatility gainers and
# losers for a given day.
#
generateHistoricalVsImpliedData.main <- function()
{
  # Load the required overnight parameters
  rm(list=ls())
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  asOfDate   <- overnightUtils.getAsOfDate()
  options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)

  # Load the implied volatility data
  impliedVolFile = paste(options$save$datastore.dir,"Vols/hVols.",
    asOfDate,".RData", sep="")
  load(impliedVolFile)

  # Load the historical volatility
  historicalVolFile <- paste( options$save$prices.mkt.dir,
    "all.historicalVol.RData", sep="")
  load(historicalVolFile)

  # Merge the two tables and prep it for output
  fullTable = generateFullComparisonTable(hV, historicalVol)

  # Sort on the largest ratios and create an output table
  print("Sorting table and generating min/max results...")
  source("H:/user/R/RMG/Energy/Reports/Hist_Implied_Vols/sortTableByColumn.R")
  largest20Table = sortTableByColumn(fullTable, fullTable$Ratio, 20, TRUE)
  smallest20Table = sortTableByColumn(fullTable, fullTable$Ratio, 20, FALSE)

  print("Writing csv files...")

  largestFileName = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "LargestImpVsHistTable.csv", sep="")
  write.csv( largest20Table, file=largestFileName, row.names=F)
  print(paste("Wrote:",largestFileName))

  smallestFileName = paste(options$save$reports.dir, asOfDate,
    .Platform$file.sep, "SmallestImpVsHistTable.csv", sep="")
  write.csv( smallest20Table, file=smallestFileName, row.names=F)
  print(paste("Wrote:",smallestFileName))

  print("generateHistoricalVsImpliedData.main() Done.")

}

################################################################################
# This function takes the implied and historical volitility tables, cleans
# them up, and then combines them for comparison.
#
generateFullComparisonTable <- function(impliedVol, historicalVol)
{
  # We only want to compare monthly volatilities so extract those
  impliedVol = impliedVol[impliedVol$vol.type=="M",]

  # Combine the two data sets where the data in their columns match.
  # In this case it's a join on curve.name and contract.dt
  fullTable = merge(impliedVol, historicalVol)

  # remove the column for vol.type
  fullTable = fullTable[,-3]

  # set the column names so we can reference them later
  names(fullTable) = c("curve.name", "contract.dt", "Implied",
    "ClosingPrice", "PriceChange", "Historical" )

  # generate the ratio column of implied/historical
  fullTable$Ratio = fullTable$Imp / fullTable$Hist

  # reorder the columns for output
  fullTable = fullTable[,c(1,2,7,3,6,4,5)]

  # reformat the dates to be year/month only and make sure all imp & hist
  # data is in the proper numeric format
  fullTable$contract.dt = format(fullTable$contract.dt, "%Y-%m")
  class(fullTable$Implied)="numeric"
  class(fullTable$Historical)="numeric"
  class(fullTable$Ratio)="numeric"

  return(fullTable)
}

################################################################################
# Start the program
#
res = try(generateHistoricalVsImpliedData.main())
if (class(res)=="try-error"){
  cat("Failed Overnight Historical vs Implied Data Job.\n")
}
