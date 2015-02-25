# Download NEPOOL wind forecast, send it to tsdb, etc. 
#
# .download_wind_forecast
# .update_tsdb_symbols
#
# Started on 3/10/2014 by AAD


################################################################################
# Download the wind forecast for a given day if the file exists.
# @param day an R Date
# @return 0 if success, 1 if fails, 99 if file exists already
#
.download_wind_forecast <- function(day=Sys.Date())
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Wind/Raw/"

  base_url <- "http://www.iso-ne.com/static-assets/documents/" 
  urlName  <- paste(base_url, format(day, "%Y/%m/"), 
    "aggregate_", format(day, "%m_%d_%Y"), ".csv", sep="")

  destfile <- paste(DIR, basename(urlName), sep="")
  if (file.exists(destfile)) {
    rLog("File already in archive.  Exiting.")
    return(99)
  }
  
  ## if ( !.isLink(urlName) ) {
  ##   rLog("Cannot find url ", urlName)
  ##   return(1)
  ## }

  res <- try( read.csv( urlName ))
  
  if ((class(res) == "try-error") || (ncol(res) == 1)) {
    1
  } else {
    write.csv(res[,1:5], file=destfile, row.names=FALSE) 
    0
  }
}


################################################################################
#
.get_raw_filename <- function(day=Sys.Date())
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Wind/Raw/"
  
  paste(DIR, "aggregate_", format(day, "%m_%d_%Y"), ".csv", sep="")  
}


################################################################################
# Not hour ending!  How it works on DST.
#
#                                        
.load_wind_forecast <- function(day=Sys.Date())
{
  fname <- .get_raw_filename(day)
  if (!file.exists(fname))
    return(NULL)
  
  aux <- read.csv(fname)
  aux$datetime <- ISOdatetime(aux[,1], aux[,2], aux[,3], aux[,4], 0, 0)
  aux$value <- aux$Total.MW
  
  aux[,c("datetime", "value")]
}



################################################################################
# Write the data to tsdb
# @param X a data.frame with 
#
.update_tsdb_symbols <- function(X)
{
  require(Tsdb)

  if (is.null(X) || nrow(X)==0)
    return()
  
  symb <- "nepool_wind_fcst"
  if (!tsdbExists(symb)) {
    rLog("creating symbol", symb)
    path <- paste("T:/Data/pm/", symb, ".bin", sep="")
    description <- "Nepool wind forecast"
    dataSource  <- "http://www.iso-ne.com/sys_ops/op_frcstng/7day_wind_pwr_frcst/index-p1.html"
    realTimeFlag <- TRUE
    tsdbCreate(symb, path, description, dataSource, realTimeFlag)
  }
  
  rLog("updating symbol", symb)
  tsdbUpdate(symb, data.frame(time=X$datetime, value=X$value)) 
}


################################################################################
################################################################################
# 
.test <- function()
{
  require(CEGbase)
  require(reshape)
  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.wind.R")

  day <- as.Date("2014-05-12")
  .download_wind_forecast( day )
  X <- .load_wind_forecast( day )

  .update_tsdb_symbols(X)

  
  days <- seq(as.Date("2014-01-01"), as.Date("2014-04-28"), by="1 day")
  lapply(days, function(day) {
    #browser()
    rLog("Working on day: ", format(day))
    if (.download_wind_forecast( day ) != 1) {
      X <- .load_wind_forecast( day )
      .update_tsdb_symbols(X)
    }
  })
  

  
  
}
