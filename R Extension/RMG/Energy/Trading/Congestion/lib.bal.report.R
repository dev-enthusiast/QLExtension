# Functions to deal with SecDb bal report 
#
# bal_summary
# get_bal_report
# find_implied_position
# .read_xlsx_archive
#


####################################################################
# bal is a list of data.frames one for each book
#
bal_summary <- function( bal ) 
{
  res <- do.call(rbind, bal)
  rownames(res) <- NULL

  tot <- ddply(res, .(period), function(x){
    apply(x[,3:21], 2, function(y)round(sum(y)) )
  })


  
  res
}


####################################################################
# works with a set of books, and days
#
get_bal_report <- function(days, books=c("NPFTRMT1", "CONGIBT",
  "NEPPWRBS", "NESTAT", "CONGIBT2") )
{
  res <- lapply(books, function(book) {
    secdb.runBalReport(days, book)
  })
  names(res) <- books
  
  res
}

  
####################################################################
# works with a set of books, and days
#
find_implied_position <- function( tot )
{
  require(lattice)
  
  days <- tail(tot, 30)$period
  #startDt <- nextMonth()
  #endDt <- seq(startDt, length.out=2, by="11 month")[2]
  startDt <- as.Date("2013-01-01")
  endDt   <- as.Date("2013-12-01")
  pwr <- PM:::ntplot.estStrip("prc_pwx_5x16_phys", startDt, endDt)
  colnames(pwr)[c(2,3)] <- c("period", "pwr.12mth.5x16.strip")
  pwr$dPrice <- NA
  pwr$dPrice[2:nrow(pwr)] <- pwr[2:nrow(pwr),3] - pwr[1:(nrow(pwr)-1),3]
  pwr$symbol <- NULL

  
  out <- merge(tot, pwr, by="period", all.x=TRUE)
  out <- subset(out, abs(subtotals) <= 1000000 )
  
 
  xyplot(subtotals/1000000 ~ dPrice, data=out,
         type=c("g", "p"),
         xlab="Change in Cal13 strip, $",
         ylab="Bal Report subtotals, M$",
         main="",
         cex=1.3)
  

  
}



####################################################################
# I store a longer history of bal reports in this spreadsheet.
#
.read_xlsx_archive <- function( books="CONGIBT" )
{
  require(xlsx)

  fname <- "H:/myTrades.xlsx"

  pnl <- lapply(books, function(book) {
    aux <- cbind(book=book, read.xlsx2(fname, sheetName=book, colClasses=c("Date",
      rep("numeric", 19), "character")))
    colnames(aux) <- tolower(colnames(aux))
    aux <- subset(aux, is.finite(aux$period))
    aux[,1:21]  # no comments column  
  })
  names(pnl) <- books

  pnl
}



####################################################################
#
#
.main <- function()
{
  require(CEGbase)
  require(plyr)
  require(SecDb)
  require(PM)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.bal.report.R")  

  books <- c("NPFTRMT1", "CONGIBT", "NEPPWRBS", "NESTAT",
             "VIRTSTAT", "CONGIBT2")

  #bal <- .read_xlsx_archive( books ) 
  #lapply(bal, tail) 

  
  #days <- seq(Sys.Date()-8, Sys.Date()-1, by="1 day")
  days <- seq(as.Date("2014-01-28"), as.Date("2014-03-17"), by="1 day")
  bal  <- get_bal_report(days, books)
  invisible(mapply(function(x, y){
    print("\n\n")
    print(y)
    print(x, row.names=FALSE)},
      bal, names(bal), SIMPLIFY=FALSE))
  

  
  
  ## res <- parse_bal_report("C:/Temp/junk.txt")
  ## print(res, row.names=FALSE)


  
  


  

  
  #filename <- paste(DIR, "bal_report_CONGIBT_2010-01_2010-12.txt", sep="")
  #res <- parse_bal_report(filename)


  

}









# I had this for James a while ago!


## ####################################################################
## # filename - ONE txt file
## #
## .parse_one_bal_report <- function(filename, filterPnL=0)
## {
##   aux <- readLines(filename)
##   aux <- aux[grep("^To[[:digit:]]", aux)]
##   aux <- gsub(",", "", aux)
##   bux <- strsplit(aux, " +")
##   bux <- data.frame(do.call(rbind, bux))
##   if (ncol(bux) != 21)
##     stop("Columns have changed!")
  
##   bux <- bux[,-21]  # last column repeats the period
##   colnames(bux) <- c("period", "yesterday", "prev.mark", "time",
##     "commod", "locations", "fx.spot", "rates", "vols", "corrs",
##     "data.fixes", "qty.fcts", "subtotals", "rolls", "new.trades",
##     "expires", "mismatch", "PnL", "growth", "today")
##   bux[,2:20] <- sapply(bux[,2:20], as.numeric)
##   bux[,1] <- gsub("To", "", bux[,1])
##   day <- as.numeric(substr(bux[,1], 1, 2))
##   bux[,1] <- SecDb:::format.dateMYY(substr(bux[,1], 3, 5)) + day - 1
    
##   bux
## }

## ####################################################################
## # filenames - a vector of txt filenames (corresponding to the same
## #   book/portfolio)
## #
## parse_bal_report <- function(filenames, filterPnL=0)
## {
##   res <- vector("list", length=length(filenames))
##   for (i in 1:length(filenames)){
##     res[[i]] <- .parse_one_bal_report(filenames[i])
##   }
##   res <- do.call(rbind, res)
##   res <- res[!duplicated(res$period),]
##   res <- res[order(res$period),]
  
##   if (filterPnL != 0){
##     res <- subset(res, abs(PnL) > filterPnL)
##   }
##   # summary(bux$PnL)

##   res
## }

