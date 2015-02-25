# source("S:/All/Risk/Software/R/prod/Utilities/Reporting/FilterCSV.r")
# source("H:/user/R/RMG/Utilities/Reporting/FilterCSV.r")

FilterCSV <- function(filteredFile, filteredCol, unfilteredFile, unfilteredCol, outputFile)
{
  source( "//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R" )
  
  rLog("Inputting Data ...")
  filtered <- read.csv(filteredFile, header = TRUE)
  names(filtered) = toupper(names(filtered))
  filterList <- unique(filtered[[ toupper(filteredCol) ]])
  unfiltered <- read.csv(unfilteredFile, header = TRUE)
  names(unfiltered) = toupper(names(unfiltered))
  
  rLog("Filtering ...")
  filteredNew <- subset(unfiltered, unfiltered[[ toupper(unfilteredCol) ]] %in% filterList)
  
  rLog("Writing .csv file  ...")
  write.csv(filteredNew, outputFile, row.names = FALSE)
  rLog("Done!")
}

FilterAprimeByCVaRbooks <- function(Date, CVaR_Name)
{
  # takes a NewCVaR file as filtered and filters Aprime on it by book

  Date = as.Date( Date )
  
  filteredFile = paste( "//ceg/cershare/Risk/Trading Controls/Daily Reports/", 
    format( Date, "%m%d%y" ), " Daily Reports/New_", CVaR_Name, "_CVaR.csv", 
    sep="" )
  filteredCol = "Book_Name"
  
  unfilteredFile = paste( "//ceg/cershare/Risk/CollateralAllocation/", 
    format(Date, "%Y%m%d"), "/SourceData/AUTO_AllPos_AGMTHScale_preibt_", 
    toupper(format(Date, "%d%b%y")), ".csv", sep="")
  unfilteredCol = "book_name"
  
  outputFile = paste( "//ceg/cershare/Risk/CollateralAllocation/",
     format(Date, "%Y%m%d"), "/SourceData/Aprime filtered on ", CVaR_Name, " ", 
     Date, ".csv", sep="")
  
  FilterCSV(filteredFile, filteredCol, unfilteredFile, unfilteredCol, outputFile)
   
}

FilterNewCVaRbooks <- function(Date, CVaR_Namefilt, CVaR_Nameunfilt)
{
  # takes a NewCVaR file as filtered and filters another NewCVaR file
  
  Date = as.Date( Date )
  
  filteredFile = paste( "//ceg/cershare/Risk/Trading Controls/Daily Reports/", 
    format( Date, "%m%d%y" ), " Daily Reports/New_", CVaR_Namefilt, "_CVaR.csv", 
    sep="" )
  filteredCol = "Book_Name"
  
  unfilteredFile = paste( "//ceg/cershare/Risk/Trading Controls/Daily Reports/", 
    format( Date, "%m%d%y" ), " Daily Reports/New_", CVaR_Nameunfilt, "_CVaR.csv", 
    sep="" )
  unfilteredCol = "book_name"
  
  outputFile = paste( "//ceg/cershare/Risk/Trading Controls/Daily Reports/", 
    format( Date, "%m%d%y" ), " Daily Reports/New_", CVaR_Nameunfilt, 
      CVaR_Namefilt, "_CVaR.csv", sep="")
  
  FilterCSV(filteredFile, filteredCol, unfilteredFile, unfilteredCol, outputFile)
   
}  