# Deal with BOE downloads
#
# .read_from_boe
# .update_tsdb
#


#################################################################
# Get the data
# @param series the BOE series parameter 
# @return a data.frame
#
.read_from_boe <- function(startDt=Sys.Date()-30, endDt=Sys.Date(),
                     series=c("XUDLUSS"))
{
  startDt <- format(startDt, "%d/%b/%Y")
  endDt   <- format(endDt,   "%d/%b/%Y")
  series  <- paste(series, collapse=",")
  url <- paste("http://www.bankofengland.co.uk/boeapps/iadb/",
    "fromshowcolumns.asp?csv.x=yes&Datefrom=", startDt,
    "&Dateto=", endDt, "&SeriesCodes=", series, 
    "&CSVF=CN&UsingCodes=Y&VPD=Y&VFD=N", sep="")

  aux <- readLines(url)
  if (length(aux) < 1)
    return(NULL)

  res <- read.csv( textConnection(aux) )
  res$DATE <- as.POSIXct(res$DATE, format="%d %b %Y")

  
  res
}

#################################################################
# write to tsdb 
# @param fx a data.frame with ONE symbol as returned by .read_from_boe
# @return a data.frame
#
.write_to_tsdb <- function( fx )
{
  series <- unique(fx$SERIES)
  if (length(series) > 1) {
    rLog("Only one symbol at a time, cowboy!")
    return(1)
  }
  
  symb <- switch(series,
    "XUDLUSS" = "boe_gbpusd",
    "")

  if (!tsdbExists(symb)) {
    rLog("creating symbol", symb)
    path <- paste("T:/Data/pm/", symb, ".bin", sep="")
    description <- "Bank Of England, GBP/USD exchange rate"
    dataSource  <- "http://www.bankofengland.co.uk/boeapps/iadb/rates.asp"
    realTimeFlag <- FALSE
    tsdbCreate(symb, path, description, dataSource, realTimeFlag)
  } else {
    rLog("updating symbol", symb)
    tsdbUpdate(symb, data.frame(time=fx$DATE, value=fx$VALUE))
  }
    
  
}

###################################################################
###################################################################
#
.test <- function()
{

}

