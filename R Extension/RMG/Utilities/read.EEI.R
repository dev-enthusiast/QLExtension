# Read files in EEI format.  NEPOOL load is in this format. 
#
#   http://www.iso-ne.com/trans/celt/fsct_detail/2009/iso_eei.txt
# published in 2010-04-07
#   http://www.iso-ne.com/trans/celt/fsct_detail/2010/iso_eei.txt  

# fileEEI <- "C:/Temp/Downloads/txt_iso_eei10.txt"
# fileOut <- "C:/Temp/celt2010-nepool-load.csv"
# 

read.EEI <- function(fileEEI, fileOut=NULL)
{
  aux <- readLines(fileEEI)
  data <- substring(aux, 21)        # data starts in column 21
  noChars      <- nchar(data[1])
  recordLength <- noChars/12

  # process the datetime.  The entries don't have to be contiguous.
  dates <- as.Date(substr(aux, 1, 6), "%m%d%y")
  ampm  <- substr(aux, 7, 7)
  HRS <- data.frame(ampm= "1", hrs=paste(0:11,  ":00:00", sep=""))
  HRS <- rbind(HRS, data.frame(ampm="2", hrs = paste(12:23, ":00:00", sep="")))

  datetimes <- merge(data.frame(dates, ampm), HRS, by="ampm")
  datetimes$dt <- as.POSIXct(paste(datetimes$dates, datetimes$hrs), tz="GMT") + 3600 # HE
  datetimes <- datetimes[order(datetimes$dt),] 

  # extract the load data.  split by characters, format as matrix,
  # paste rows together to get the entry. 
  YY <- sapply(data, function(x){strsplit(x, "")})
  ZZ <- unlist(lapply(YY, function(x, recordLength){
    y <- matrix(x, nrow=recordLength)
    as.numeric(apply(y, 2, paste, sep="", collapse=""))}, recordLength),
      use.names=FALSE)
  
  # make a zoo object
  #ZZ <- zoo(ZZ, datetimes$dt)
  res <- data.frame(datetime=as.character(datetimes$dt), load=ZZ)

  if (!is.null(fileOut)){
    write.csv(res, file=fileOut, row.names=FALSE)
  }

}

###########################################################################
#
.main <- function()
{
  fileEEI <- "C:/Temp/Downloads/iso_eei11.txt"
  fileOut <- "C:/Temp/Downloads/iso_eei11.csv"

  res <- read.EEI(fileEEI, fileOut)

  require(Tsdb)
  symbol <- "nepool_celt_2011_fcst"
  ## tsdbCreate(symbol, "T:/Data/pm/nepool/nepool_celt_2011_fcst.bin",
  ##   "CELT pool load forecast",
  ##   "http://www.iso-ne.com/trans/celt/fsct_detail/2011/index.html", TRUE)
  x <- data.frame(time=as.POSIXct(res$datetime), value=res$load)
  tsdbUpdate(symbol, x)
  
}
