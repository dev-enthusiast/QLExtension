# Download from Yahoo the curent price for a ticker. 
#
#
# Written by Adrian Dragulescu on 7-Jan-2005

get.spot <- function(tickers)
{
  spot <- rep(NA, length=length(tickers))
  names(spot) <- tickers
  for (ticker in tickers){
    str <- paste("http://finance.yahoo.com/q?s=", ticker, "&d=t", sep="")
    aux <- file(str,open="r")     # open connection to read report
    bux <- readLines(aux)         # read the file in variable bux
    close(aux); unlink(str)       # close connection

    ind <- grep("Last Trade", bux)
    bux <- bux[ind]               # pick the right table

    cux <- strsplit(bux, "Last Trade:", extended=T)[[1]][2]
    dux <- strsplit(cux, "<b>")[[1]][2]
    eux <- strsplit(dux, "</b>")[[1]][1]
      
    spot[ticker] <- as.numeric(eux)
  }
  
  return(spot)
}

 
