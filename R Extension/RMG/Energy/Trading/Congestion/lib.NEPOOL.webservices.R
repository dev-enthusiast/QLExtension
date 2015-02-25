# Tools to get data from NEPOOL webservices
#
# .convert_timestamp
# .request
#   .to_tsdb_ncpc_lscpr
# .ws_get_ncpc
#


########################################################################
# Convert from a string, e.g. "2015-01-01T00:03:58.000-05:00" to a POSIXct
# @param x a vector of strings
# @return a vector of POSIXct
#
.convert_timestamp <- function(x, fmt="%Y-%m-%dT%H:%M:%OS%z")
{
  # this is the simplest test I know
  if (all(nchar(x) == 29)) {
    # need to get go from -05:00 to -0500 that R can digest!
    x <- paste(substring(x, 1, 26), substring(x, 28, 29), sep="")
  }
  
  y <- strptime(x, format=fmt)

  
  y
}



########################################################################
# Make the call to the webservices
#
.request <- function(URL, count=0)
{
  r <- GET(URL, content_type_xml(), 
           config=list(userpwd = "adrian.dragulescu@constellation.com:OdiRed19Fr",
                       header=TRUE,
                       httpheader=c(Accept = "application/xml"),
                       ssl.verifyhost = FALSE,
                       ssl.verifypeer = FALSE,
                       followlocation = TRUE,
                       useragent = "Mozilla/4.0"))  
                       ## ssl.cipher.list = c("RC4-SHA:RC4-MD5"),  
                       ## sslversion = 3))

  # first call fails, the second succeeds ... weird
  if ((count < 1) && (r$status != 200) ) {
    rLog("Try again", URL)
    count <- count + 1
    r <- .request(URL, count)
  }
      
  r
}





########################################################################
# Upload to tsdb the NCPCP LSCPR curves
# @param X a data.frame with daily data
##   NCPCType                     BeginDate DaNcpcCharge RtNcpcCharge RtNcpcLoadObligation DaNcpcChargeRate RtNcpcChargeRate Location..attrs
## 1    LSCPR 2014-11-01T00:00:00.000-04:00     -1634.32            0            -31153.34             0.05               NA            9001
## 2    LSCPR 2014-11-01T00:00:00.000-04:00     -1480.85            0            -28228.47             0.05               NA            9002
## 3    LSCPR 2014-11-01T00:00:00.000-04:00     -1013.41            0            -19317.67             0.05               NA            9005
## 4    LSCPR 2014-11-01T00:00:00.000-04:00     -1882.59            0            -35885.98             0.05               NA            9006
## 5    LSCPR 2014-11-01T00:00:00.000-04:00     -3170.84            0            -60442.93             0.05               NA            9008
## 6    LSCPR 2014-11-02T00:00:00.000-04:00      -491.42            0            -32264.26             0.02               NA            9001
## > 
#
#
.to_tsdb_ncpc_lscpr <- function(X)
{
  if (X$NCPCType[1] != "LSCPR")
    stop("You cannot insert type ", X$NCPCType[1], " into LSCP symbols!")
  
  # NE = New England, d = daily 
  ddply(X, "Location..attrs", function(x) {
    zone <- x[1,"Location..attrs"]
    dt <- as.POSIXct(paste(substr(x$BeginDate, 1, 10), "00:00:00"))
                      # data column           # tsdb symbol
    symbs <- matrix(c("DaNcpcCharge",         "PM_NE_dDaNcpcLscpCharge_", 
                      "RtNcpcCharge",         "PM_NE_dRtNcpcLscpCharge_",
                      "RtNcpcLoadObligation", "PM_NE_dRtNcpcLscpLoad_"),
                    ncol=2, byrow=TRUE)
    for (r in 1:nrow(symbs)) {
      symb <- paste(symbs[r,2], zone, sep="")
      rLog("Inserting ", symb)
      
      if (nchar(symb) > 31)
        stop("--->  tsdb symbol too long!")
      if ( !tsdbExists( symb ) ) {
        path <- paste("T:/Data/pm/nepool/", symb, ".bin", sep="")
        description <- paste("Daily NEPOOL LSCP", symbs[r,1], "for", zone) 
        dataSource <- "ISO web services /ncpc/{NCPCType}/month/{month}"
        rLog("Creating symbol", symb)
        tsdbCreate(symb, path, description, dataSource, realTimeFlag=FALSE)
      }
      #browser()
      aux <- data.frame(time=dt, value=as.numeric( x[,symbs[r,1]] ))
      aux <- aux[which(!is.na(aux$value) & aux$value != 0),]  # eliminate NA's and zeros
      if (nrow(aux) > 0) {
        tsdbUpdate(symb, aux)
      } else {
        rLog("No data to insert!")
      }         
    }
  })

  invisible()
}


########################################################################
# Daily ncpc by type, month, region
# @param month a vector of months, bom Date
# @param type a vector of allowed NCPC Types
# @result a data.frame
.ws_get_hbdayaheadenergyoffer <- function( month=prevMonth(),
                          type=c("GPA",         # winter reliability testing
                                 "VAR",
                                 "HIGHVAR",
                                 "EXTLOCECONOMIC",
                                 "LSCPR",
                                 "ECONOMIC"),
                          ws_version = "1.1")
{
  
}



########################################################################
# Daily ncpc by type, month, region
# @param month a vector of months, bom Date
# @param type a vector of allowed NCPC Types
# @result a data.frame
.ws_get_ncpc <- function( month=prevMonth(),
                          type=c("GPA",         # winter reliability testing
                                 "VAR",
                                 "HIGHVAR",
                                 "EXTLOCECONOMIC",
                                 "LSCPR",
                                 "ECONOMIC"),
                          ws_version = "1.1")
{
  if (length(type) != 1)
    stop("Only one NCPC type at a time, cowboy!")
  
  res <- ldply(month, function(mon) {
    rLog("working on month", format(mon))
    URL <- tolower(paste("https://webservices.iso-ne.com/api/v",
      ws_version, "/ncpc/", type, "/month/", format(mon, "%Y%m"), sep=""))

    r <- .request( URL )

    x <- content(r, "text")

    if (grepl("xml", x)) {
      x <- gsub("(.*)(<?xml .*)", "<?\\2", x)
      data <- xmlToList( xmlParse(x) )
      res <- ldply(data, function(x){as.data.frame(x)})
      
    } else if (grepl("json", x)) {
      res <- NULL
      y <- gsub("(.*Connection: close)(\\2)", "", x)  # get rid of the header
      y <- fromJSON(y)
      if ("NCPCs" %in% names(y)) {
        y <- y[["NCPCs"]]
        if ("NCPC" %in% names(y)) {
          res <- y[["NCPC"]]
          res$Location..attrs <- res$Location[,"@LocId"]   # Note that Location is a data.frame!
          res$Location <- NULL
        }
      } 
      

    } else {
      stop("Unknown content type!")
    }


    res
  })
  #as.Date(strptime("2013-01-04T00:00:00.000-0500", format="%Y-%m-%dT%H:%M:%OS%z"))
   
  
  res
}




########################################################################
########################################################################
#
.test <- function()
{
  library(CEGbase)
  library(httr)
  library(XML)
  library(plyr)
  library(SecDb)
  library(Tsdb)
  library(jsonlite)
  
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.interval.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.webservices.R")

  month <- toMonthly( Interval("2014-11-01", "2014-12-01") )
  X <- .ws_get_ncpc(month, type="LSCPR")
  
  .to_tsdb_ncpc_lscpr( X )


  X <- .ws_get_ncpc(month, type="ECONOMIC")


  
}  



  
  

  ########################################################################
  # Alternative ways
  ## r <- GET("https://webservices.iso-ne.com/api/v1.1/ncpc/lscpr/current",
  ##          config=list(userpwd = "adrian.dragulescu@constellation.com:OdiRed19Fr",
  ##                      header=TRUE,
  ##                      httpheader=c(Accept = "application/xml"),
  ##                      ssl.verifyhost = FALSE,
  ##                      ssl.verifypeer = FALSE,
  ##                      followlocation = TRUE,
  ##                      useragent = "Mozilla/4.0",   
  ##                      sslversion = 3))
  ## require(RCurl)
  ## res <- getURL("https://webservices.iso-ne.com/api/v1.1/ncpc/lscpr/current",
  ##               userpwd="adrian.dragulescu@constellation.com:OdiRed19Fr",
  ##               verbose=TRUE,
  ##               ssl.verifyhost = FALSE,
  ##               ssl.verifypeer = FALSE,
  ##               followlocation=TRUE,
  ##               header=TRUE,
  ##               sslversion = 3)
  
  
  ## URL <- "https://webservices.iso-ne.com/api/v1.1/ncpc/lscpr/current"
  ## cmd <- paste("curl -SL -k --user adrian.dragulescu@constellation.com:OdiRed19Fr",
  ##              URL)
  ## res <- system(cmd)


  
