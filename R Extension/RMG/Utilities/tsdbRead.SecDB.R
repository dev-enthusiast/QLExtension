# Calls secexpr to automatically download historical prices from TSDB
#
# Where options$tsdb.refresh=1 will download, otherwise look on disk
#
# Written by Adrian Dragulescu on 8-Feb-2005
#
# options=save=NULL
# options$hdata.startDate <- as.Date("2005-01-01")
# options$hdata.endDate   <- as.Date("2005-01-31")
# options$hdata.symbols   <- c("GasDailyMean_TRNY", "pjm_load_hist")   
# save$dir <- "c:/temp/"
# source("C:/R/Work/MyLibrary/Utilities/tsdbRead.SecDB.R")
# res <- tsdbRead.SecDB(options, save)


tsdbRead.SecDB <- function(options,save)
   {

      TMPdir <- "c:/temp/"
      if (length(options$hdata.filename)==0){options$hdata.filename <- "hdata.csv"}
      if (length(options$tsdb.refresh)==0){options$tsdb.refresh <- 0}
      TMPfilename <- paste(TMPdir, options$hdata.filename, sep="")
      symbolfilename <- paste(TMPdir,"tsdbsymbols.csv",sep="")
      filename <- paste(save$dir, options$hdata.filename, sep="")

      if (options$tsdb.refresh==1)
        {
          cat("Downloading tsdb historical data... "); flush.console()
          # Create Tsdb Symbols on the fly
          write.table(options$hdata.symbols,symbolfilename,row.names = F, col.names = F,quote = F)

          aux <- "secexpr -source UPSH -l SYMBOLFILE STARTDATE ENDDATE TEMPFILE -s \"YL: Get Tsdb Data\""
          aux <- gsub("STARTDATE", as.character(format(options$hdata.startDate, "%d%b%y")), aux)
          aux <- gsub("ENDDATE", as.character(format(options$hdata.endDate, "%d%b%y")), aux)
          aux <- gsub("SYMBOLFILE", symbolfilename, aux)
          aux <- gsub("TEMPFILE", TMPfilename, aux)
          res <- system(aux, minimized=T, invisible=T)
          browser()
          if (res == 0)
             {
               Tabular <- read.table(TMPfilename, header = T ,sep = ",", dec = ".", as.is = T)
               cat("Done! \n"); flush.console()
               cat("Updating Files for Caching \n"); flush.console()
               Tabular <- Tabular[-nrow(Tabular),]
               write.table(Tabular,filename,col.names = T,row.names = F,sep = ",")
             }
          else
             cat("Failed! \n"); flush.console()
             Tabular <- data.frame()
         }
      else
         {
          #--------------------------------------------Load data into R-----
          Tabular <- read.table(filename,header = T, sep = ",", dec = ".", as.is = T )
         }
      return(Tabular)
   }
