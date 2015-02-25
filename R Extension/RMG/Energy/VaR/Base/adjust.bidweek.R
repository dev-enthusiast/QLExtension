# Adjust bidweek gas positions.  Load adjustments from a spreadsheet.
#
#
# Written by Adrian Dragulescu on 23-Jan-2006

adjust.bidweek <- function(IVaR, options){

  cat("Adjust bidweek positions ... ")
  sheetname <- format(options$asOfDate, "%b%y") 
  filename  <- paste(options$save$datastore.dir, "bidweek.adjustment.xls", sep="")
  con    <- odbcConnectExcel(filename)
  sheets <- sqlTables(con)
  if (!is.element(sheetname, gsub("\\$", "", sheets$TABLE_NAME))){
    cat("Cannot find sheet", sheetname, "in the bidweek spreadsheet.\n")
    cat("Positions are left unajusted.\n")
    return(IVaR)
  }
  AA <- sqlFetch(con, sheetname)           # adjustments
  names(AA) <- gsub("#", ".", names(AA))  
  ind <- grep("-", names(AA))
  odbcClose(con)
  AA[,1:2] <- sapply(AA[,1:2], toupper)
  ind <- which(apply(AA, 1, function(x){all(is.na(x))}))
  if (length(ind)>0){AA <- AA[-ind,]}    # clean up empty rows...

  ind <- which(names(AA) == format(options$asOfDate, "%d-%b"))
  if (length(ind)==0){
    cat("\nInside adjust.bidweek: As of date not within spreadsheet columns!\n")
    return(IVaR)
  }
  focus.month <- seq(as.Date(format(options$asOfDate, "%Y-%m-01")),
                        by="month", length.out=2)[2]
  AA$contract.dt <- focus.month
  AA <- AA[,c(which(names(AA) %in% c("curve.name", "contract.dt")), ind)]
  names(AA)[3] <- "adjustment"
  
  aux <- cbind(IVaR$Prices, ind.row=1:nrow(IVaR$Prices))
  aux <- merge(aux, AA, all.x=T) 
  aux$original.position <- aux$position
  ind.bidday <- which(names(aux) == "adjustment")
  aux[,ind.bidday] <- ifelse(is.na(aux[,ind.bidday]),0, aux[,ind.bidday])

  fp.adjustment <- sum(aux$position*aux[,ind.bidday])
  aux$position <- aux$position*(1-aux[,ind.bidday])  # take the adjustment
##   ind.NYMEX <- which(aux$curve.name=="COMMOD NG EXCHANGE" &
##                      is.na(aux$vol.type) & aux$contract.dt==focus.month)
##   if (length(ind.NYMEX)>0){
##     aux$position[ind.NYMEX] <- aux$position[ind.NYMEX] + fp.adjustment}

  aux <- aux[order(aux$ind.row),]; aux$ind.row <- NULL
  IVaR$Prices <- aux
  IVaR$Changes$position <- aux$position
  cat("Done.\n")
  return(IVaR)
}

##   names(AA)[ind] <- as.character(as.Date(paste(names(AA)[ind], "-",
##     format(options$asOfDate, "%Y"), sep=""), format="%d-%b"))

  
  

