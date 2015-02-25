# Get BalmoDlyPrices
#
#

secdb.balmoDlyPrices <- function( asOfDate, region="NEPOOL", location="Hub",
  bucket=NULL, forDate=NULL)
{
  cmd <- paste('link("_lib elex strip addin fns"); res = @ElecStrip::BalmoDlyPrices(',
    'Date("', format(asOfDate, "%d%b%y"), '"), "', region, '", ',
    '"', location,  '"', sep="")
  
  if ( !is.null(forDate) ) {
    if ( !is.null(buckets) ){
      cmd <- paste(cmd, ', ["', paste(buckets, collapse='", "', sep=""), '"]', sep="")
    }
    cmd <- paste(cmd, ', "', format(forDate, "%d%b%y"), '"', sep="")
  }

  ## if (bizDay != 0) {  # will have to pass null for buckets, etc. 
  ##   cmd <- paste(cmd, ', 1', sep="")
  ## }

  cmd <- paste(cmd,  ');',  sep="")
  
  res <- secdb.evaluateSlang(cmd)
  
  aux <- data.frame(do.call(rbind, lapply(res[-1], as.numeric)))
  colnames(aux) <- as.character(res[[1]])
  aux$Date <- as.Date(secdb.convertTime(aux$Date))
  
  aux
}


.test.balmoDlyPrices <- function()
{
  require(SecDb)
  
  asOfDate <- as.Date("2012-07-24")
  region   <- "NEPOOL"
  location <- "Hub"
  
  res <- secdb.balmoDlyPrices( asOfDate, region=region,
    location=location)

  buckets  <- c("2x16H", "7x8")
  forDate  <- as.Date("2012-08-03") 
  res <- secdb.balmoDlyPrices( asOfDate, region=region,
    location=location, bucket=buckets, forDate=forDate)

  
  
}


  ## require(SecDb)
 ## SecDBLib  <- "_Lib Elex Strip Addin Fns"
 ##  SecDBFun  <- "ElecStrip::BalmoDlyPrices"
 ##  res <- secdb.invoke(SecDBLib, SecDBFun, Sys.Date(), "NEPOOL", "CT",
 ##    NULL, as.Date("2012-08-03"), 0.0)  # the second NULL gets mapped to 0!
  


  ## res <- secdb.invoke(SecDBLib, SecDBFun, as.Date("2012-07-24"), "NEPOOL", "CT",
  ##   NULL, as.Date("2012-07-28"), 0.0)

  ## res <- secdb.invoke(SecDBLib, SecDBFun, asOfDate, region, location,
  ##   buckets, forDate, bizDay)





