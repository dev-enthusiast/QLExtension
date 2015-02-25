# Calls matlab to automatically download historical prices from TSDB
#
# Where options$tsdb.refresh=1 will download, otherwise look on disk
#
# Written by Adrian Dragulescu on 8-Feb-2005
#
# options=save=NULL
# options$hdata.startDate <- as.Date("2005-01-01")
# options$hdata.endDate   <- as.Date("2005-01-31")
# options$hdata.symbols   <- c("NRC_BEAVER_VALLEY_1_LVL",
#                              "NRC_CALVERT_CLIFFS_1_LVL",
#                              "NRC_FITZPATRICK_LVL")
# save$dir <- "c:/temp/"
# source("C:/R/Work/MyLibrary/Utilities/tsdbRead.R")
# res <- tsdbRead(options, save)


tsdbRead <- function(options,save){

if (length(options$HS)==0){options$HS <- 0}
if (length(options$hdata.filename)==0){options$hdata.filename <- "hdata.csv"}
if (length(options$tsdb.refresh)==0){options$tsdb.refresh <- 0}

if (options$tsdb.refresh==1){  
  cat("Downloading tsdb historical data... "); flush.console()
  aux=NULL
  aux[1] <- "startDate  = datenum('SSSSS'); endDate  = datenum('EEEEE');"
  aux[2] <- "symbols    = strvcat(BBBBB); filename   = ['FFFFF'];"
  aux[3] <- "cd 'DDDDD'; options.HS = 0.0000; options.merge=1;"
  aux[4] <- "addpath 'H:\\Matlab\\MyLibrary\\Energy\\DataExport';"
  aux[5] <- "save_TSDB_to_CSV(startDate, endDate, symbols, filename, options);"
  aux[6] <- "exit;"
  bux <- gsub("SSSSS",
              as.character(format(options$hdata.startDate, "%d-%b-%Y")), aux)
  cux <- gsub("EEEEE",
              as.character(format(options$hdata.endDate, "%d-%b-%Y")), bux)

  qsymb <- paste("'", options$hdata.symbols, "'", sep="")
  str <- qsymb[1]
  s   <- 1
  while (s<length(options$hdata.symbols)){
      s   <- s+1
      str <- paste(str, qsymb[s], sep=", ")
  }
  dux <- gsub("BBBBB", str, cux)
  eux <- gsub("DDDDD", save$dir, dux)
  if (options$HS==1){eux <- gsub("0.0000", "1", eux)}
  eux <- gsub("FFFFF", options$hdata.filename, eux)
#browser()
  #--------------------------------------------Launch Matlab--------
  m.filename <- paste(save$dir, "tsdb_call.m", sep="")
  writeLines(eux, m.filename)
  m.command <- paste("run('",save$dir,"tsdb_call.m')", sep="")
  system(paste("matlab -minimize -nosplash -nodesktop -r", m.command))
  cat("Done! \n"); flush.console()
  file.remove(m.filename)
}
#--------------------------------------------Load data into R-----
res <- read.csv(paste(save$dir, options$hdata.filename, sep=""),
                colClasses="numeric", na.strings="NaN")
return(res)

}
