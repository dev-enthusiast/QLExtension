# source("S:/All/Risk/Software/R/prod/Utilities/Reporting/FilterRedAprimeToJV.r")
# source("H:/user/R/RMG/Utilities/Reporting/FilterRedAprimeToJV.r")

FilterRedAprimeToJV <- function(Date)
{
  Date = as.Date( Date )

  filepath = paste( "//ceg/cershare/Risk/CollateralAllocation/",
    format(Date, "%Y%m%d"), "/SourceData/Reduced Aprime ",
    toupper(format(Date, "%d%b%y")), ".csv", sep="" )
  Aprime = read.csv(filepath, header = TRUE)

  JV = subset(Aprime, substr(Aprime[["CER_GROUP_2"]],1,2) == "JV")

  outputpath = paste("//ceg/cershare/Risk/CollateralAllocation/",
    format(Date, "%Y%m%d"), "/SourceData/RedAprimeJV_", toupper(format(Date, "%d%b%y")),
    ".csv", sep="")
  write.csv(JV, outputpath, row.names = FALSE)
}