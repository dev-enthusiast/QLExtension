# source("S:/All/Risk/Software/R/prod/Utilities/Reporting/FilterGenToIBTs.r")
# source("H:/user/R/RMG/Utilities/Reporting/FilterGenToIBTs.r")

FilterGenToIBTs <- function(Date)
{
  Date = as.Date( Date )

  filepath = paste( "//ceg/cershare/Risk/CollateralAllocation/",
    format(Date, "%Y%m%d"), "/SourceData/Reduced Aprime ",
    toupper(format(Date, "%d%b%y")), ".csv", sep="" )
  Aprime = read.csv(filepath, header = TRUE)

  GenIBTs = subset(Aprime, substr(Aprime[["BOOK_NAME"]],1,3) == "PGD")
  
  outputpath = paste("//ceg/cershare/Risk/CollateralAllocation/",
    format( Date, "%Y%m%d" ), "/SourceData/AprimeGenIBTs_", 
    toupper(format( Date, "%d%b%y" )), ".csv", sep="")
  write.csv(GenIBTs, outputpath, row.names = FALSE)
}
