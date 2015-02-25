# This is an example how to call create.FEA.file()

setwd("S:\\Risk\\2007,Q4\\Structured Deals\\Natural Gas\\Monroe Storage\\rfiles")
source("H:\\user\\RMG\\R\\Models\\Gas\\Gas.Products\\Storage\\create.FEA.file.r")
create.FEA.file(secSymbol="PRC_NG_TEN500_PHYS", GDM="GasDailyMean_TN500", dateSt=as.Date("2004-1-1"),
    dateEnd=as.Date("2010-1-1"), output="C:/temp/", write=TRUE)
# secSymbol is the NTPLot forward curve symbol without expiration dates
# dateSt is the first date used for historic analysis
# dateEnd is the last expiration month of the deal
# write = TRUE saves results to a txt file in the current working directory.
# write = FALSE returns the values in R (can be assigned to a variable or printed on the screen.
