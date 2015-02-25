# Download from Yahoo the summary table for a ticker. 
#
#
# Written by Adrian Dragulescu on 7-Jan-2005



get.summary <- function(tickers)
{
  res <- vector("list", length(tickers))
  names(res) <- tickers
  for (ticker in tickers){
    str <- paste("http://finance.yahoo.com/q?s=", ticker, "&d=t", sep="")
    aux <- file(str,open="r")     # open connection to read report
    bux <- readLines(aux)         # read the file in variable bux
    close(aux); unlink(str)       # close connection

    ind <- grep(what, bux)
    bux <- bux[ind]               # pick the right table
    bux <- strsplit(bux, "<table")[[1]][-1]

    bux <- gsub(' class=\"yfnc_tabledata1\"', "", bux)
    bux <- gsub(' class=\"yfnc_tablehead1\"', "", bux)
    bux <- gsub(' class=\"end\"', "", bux)
    bux <- gsub(' width=\"48%\"', "", bux)
    bux <- gsub("</tr><tr>", "", bux)
    bux <- gsub("</*big>", "", bux)
    bux <- gsub("</*b>", "", bux)
    bux <- gsub("</*small>", "", bux)

    TT <- strsplit(bux, "</*td>")   # split by <td> and </td>
   
    TT <- lapply(TT, .clean.table)
    TT <- do.call(cbind, TT)

    aux <- data.frame(t(TT[,2]))
    
    res[[ticker]] <- TT
  }
  res <- do.call(rbind, res)

  res <- .format.table(res)   # transform to numerics
  names(res) <- gsub(" ", ".", tolower(names(res)))
  res <- cbind(ticker=rownames(res), res)
  rownames(res) <- NULL
  
  return(res)
}

 
.clean.table <- function(x){
  x <- x[-1]
  x <- x[which(x!="")]
  x <- x[-length(x)]
  x <- matrix(x, ncol=2, byrow=TRUE)
  x[3,2] <- gsub("<.*>", "", x[3,2])
  y <- data.frame(t(x[,2]))
  names(y) <- gsub(":","",x[,1])
  y
}

.format.table <- function(x){
  x[,c(1,4:5,8,14:15)] <- sapply(x[,c(1,4:5,8,14:15)], as.numeric)
  mult <- grep("B$", x[,13] )   # find the billions
  aux  <- as.numeric(gsub("[[:upper:]]", "", x[,13]))
  if (length(mult)>0) aux[mult] <- aux[mult]*1000
  x[,13] <- aux    # market value

  x[,11:12] <- sapply(x[,11:12], function(x){as.numeric(gsub(",","",x))})
  
  x
}
