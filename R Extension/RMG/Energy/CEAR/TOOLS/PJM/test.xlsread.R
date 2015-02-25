

options$dir <- "H:/Matlab/FTRs/PJM/Bellama, Brian/2005-06/"
options$filename <- "test.xls"
con    <- odbcConnectExcel(paste(options$dir, options$filename, sep=""))
tables <- sqlTables(con) 
data   <- sqlFetch(con, "FTR")
odbcCloseAll()
 
