# Calls secexpr to  automatically download historical prices from TSDB
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

    download.onesymbol <- function(symbol, options)
    {
      aux <- "secexpr -source UPSH -l SYMBOL STARTDATE ENDDATE TEMPFILE -s \"YL: Get Tsdb Data Old\""
      aux <- gsub("STARTDATE", as.character(format(options$hdata.startDate, "%d%b%y")), aux)
      aux <- gsub("ENDDATE", as.character(format(options$hdata.endDate, "%d%b%y")), aux)
      symbol <- paste("\"",symbol,"\"",sep="")
      aux <- gsub("SYMBOL", symbol, aux)
      aux <- gsub("TEMPFILE", TMPfilename, aux)
      res <- system(aux, minimized=T, invisible=T)
      res <- read.csv(TMPfilename, header=F)
      return(res)   
    }

TMPdir <- "c:/temp/"
if (length(options$hdata.filename)==0){options$hdata.filename <- "hdata.csv"}
if (length(options$tsdb.refresh)==0){options$tsdb.refresh <- 0}
TMPfilename <- paste(TMPdir, options$hdata.filename, sep="")
filename <- paste(save$dir, options$hdata.filename, sep="")

if (options$tsdb.refresh==1)
  {  
   cat("Downloading tsdb historical data... "); flush.console()
   Initialized <- 0
   
   for (s in options$hdata.symbols)
      {
       res <- download.onesymbol(s, options)
       INdate <- as.character(res[,1])
       INQuat <- res[,2]

       if (Initialized == 0)
         {
           if (nchar(INdate[1]) >= 10)
              {
                 cat("\n Hourly Data \n");
                 HourlyDaily <-  0
                 DatePosix <- seq(strptime(options$hdata.startDate,"%Y-%m-%d"),strptime(options$hdata.endDate,"%Y-%m-%d")+86400,"hour")
                 YearPosix <-  as.numeric(format.POSIXct(DatePosix,"%Y"))
                 MonthPosix <-  as.numeric(format.POSIXct(DatePosix,"%m"))
                 DayPosix <- as.numeric(format.POSIXct(DatePosix,"%d"))
                 WeekdayPosix <- as.numeric(format.POSIXct(DatePosix,"%A"))
                 HourPosix <- as.numeric(format.POSIXct(DatePosix,"%H")) 
                
                 Tabular <- data.frame(date = DatePosix, year=YearPosix,month=MonthPosix,day=DayPosix,hour=HourPosix)
              }
           else
              {
                 cat("\n Daily Data \n");
                 HourlyDaily <-  1
                 DatePosix <- seq(strptime(options$hdata.startDate,"%Y-%m-%d"),strptime(options$hdata.endDate,"%Y-%m-%d")+86400,"day")
                 YearPosix <-  as.numeric(format.POSIXct(DatePosix,"%Y"))
                 MonthPosix <-  as.numeric(format.POSIXct(DatePosix,"%m"))
                 DayPosix <- as.numeric(format.POSIXct(DatePosix,"%d"))
                 WeekdayPosix <- as.numeric(format.POSIXct(DatePosix,"%A"))
                 HourPosix <- as.numeric(format.POSIXct(DatePosix,"%H")) 
          
                 Tabular <- data.frame(date = DatePosix, year = YearPosix, month = MonthPosix, day = DayPosix)
              }
           Initialized <- 1
         }
       if (length(Tabular$date) > length(INdate))
         {
           Tmpdate <-  strptime(INdate,"%d-%b-%y %H:%M")
           StartingIndex <-  which(Tabular$date==Tmpdate[1])
           EndingIndex <-  which(Tabular$date==Tmpdate[length(INdate)]) # length(Tmpdate) is not correct, maybe a bug
           TmpQuat = array (data=NA,dim = length(Tabular$date))
           indexIn = 1
           for (indexT in StartingIndex:EndingIndex)
             {
               TmpQuat[indexT] <- INQuat[indexIn]
               indexIn <-  indexIn + 1
             }
           INQuat <- TmpQuat
         }
       Tabular <-  cbind(Tabular, INQuat)
       colnames(Tabular)[length(colnames(Tabular))] = s
      }
    cat("Done! \n"); flush.console()
    cat("Updating Files for Caching \n"); flush.console()
    Tabular <- Tabular[-nrow(Tabular),]
    Tabular <- Tabular [,-which(colnames(Tabular)=="date")]
    write.table(Tabular,filename,col.names = T,row.names = F,sep = ",")
  }
#--------------------------------------------Load data into R-----
else
{
 Tabular <- read.table(filename,header = T, sep = ",", dec = "." )
}
return(Tabular)
}
