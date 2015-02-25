# Download from Yahoo the curent price for a ticker. 
#
#
# Written by Adrian Dragulescu on 7-Jan-2005

xls.get.spot <- function(ticker){

  str <- paste("http://finance.yahoo.com/q?s=", ticker, "&d=t", sep="")
  aux <- file(str,open="r")     # open connection to read report
  bux <- readLines(aux)         # read the file in variable bux
  close(aux); unlink(str)       # close connection

  ind <- grep("Last Trade", bux)
  bux <- bux[ind]               # pick the right table
#  bux <- gsub(" *"," ",bux)     # get rid of unwanted spaces ?? DUNNO Apr-05 
  cux <- strsplit(bux, " ", extended=T)[[1]]
  ind <- grep("Trade:", cux)
  dux <- cux[ind+1]
  eux <- strsplit(dux,">*</")[[1]][1]
  spot <- list(value=as.numeric(strsplit(eux, "b>")[[1]][2]))
  
  return(spot)
}

