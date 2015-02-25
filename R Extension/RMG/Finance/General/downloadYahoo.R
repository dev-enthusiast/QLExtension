# Import stock data from Yahoo's Internet Site.  Works well for long time
# historical intervals. 
#
# Written by Adrian Dragulescu on 27-Sep-2004
#
# Tickers    <- c("IWF", "KMP", "IBM", "QQQ", "C")
# startDate <- as.Date("1992-06-30")
# endDate   <- as.Date("2004-08-24")
# what      <- "Adj..Close." #or "Volume","Open", "Close", "Low", or NULL for all
# res <- downloadYahoo(Tickers, startDate, endDate, what)
#
# aux <- downloadYahoo(c("AMD", "AKAM"), Sys.Date()-365, Sys.Date(),
#                       c("Open", "Adj.Close"))

yahooImport <- function (query, file = "tempfile",
  source = "http://chart.yahoo.com/table.csv?", save = FALSE, sep = ";",
  swap = 20, try = TRUE){
    if (try) {
        z = try(yahooImport(file = file, source = source, query = query, 
            save = save, try = FALSE))
        if (class(z) == "try-error" || class(z) == "Error") {
            return("No Internet Access")
        } else {return(z)}
    }
    else {
        if (class(version) != "Sversion") {
            method = NULL
        } else {
            method = "wget"
        }
        download.file(url = paste(source, query, sep = ""), destfile = file, 
            method = method, quiet=TRUE)
        x1 = rev(scan(file, what = "", skip = 1))           
        x2 = strsplit(x1, ",")
        x1 = matrix(unlist(x2), byrow = TRUE, nrow = length(x2))
        z = matrix(as.numeric(x1[, -1]), ncol = dim(x1)[2] - 1)
        rowNames = as.character(as.Date(x1[, 1]))  
        colNames = scan(file = file, n = dim(x1)[2], what = "", 
            sep = ",")[-1]
        dimnames(z) = list(1:nrow(z), colNames)
        unlink(file)
#        browser()
       z = data.frame(DATE = rowNames, z, row.names = NULL)
       return(as.data.frame(z))
    }
    invisible()
}


downloadYahoo <- function(Tickers, startDate, endDate, what="Adj.Close"){

  if (is.null(what)){what <- c("Open", "Volume", "Adj.Close")}
  
  trueStartDate <- startDate
  startDate     <- startDate-365   # just to be sure I get everything

  dwnl.1.Ticker <- function(ticker, startDate, endDate){
#   server <- "http://ichart.finance.yahoo.com/table.csv?"
    yearSD  <- format(startDate, "%Y"); yearED  <- format(endDate, "%Y")
    monthSD <- format(startDate, "%m"); monthED <- format(endDate, "%m")
    daySD   <- format(startDate, "%d"); dayED   <- format(endDate, "%d")  

    query <- paste("s=",ticker,"&a=",monthSD,"&b=",daySD,"&c=",yearSD,
               "&d=",monthED,"&e=",dayED,"&f=",yearED,"&g=d", sep="")
    prices=NULL; trial=0
    while(trial<5){
      prices <- try(yahooImport(query, try = TRUE))
      if (length(prices)>0){break}
      Sys.sleep(10)         # sometimes you lose internet connection!
      trial  <- trial + 1
    }
    if (class(prices) == "data.frame"){
      prices$date <- as.Date(as.character(prices$DATE))
      prices$DATE <- NULL
      prices <- data.frame(date=prices$date, prices[, what])
    } else {prices <- NULL}
    return(prices)
  }

  quotes <- NULL
  for (tk in 1:length(Tickers)){
    q <- dwnl.1.Ticker(Tickers[tk], startDate, endDate)
    if (is.null(q)) next
    if ((is.data.frame(q)) & (length(Tickers)>1) & length(what)==1){      
      colnames(q)[2] <- Tickers[tk]}
    if (length(what)==1){     # for one thing, do a merge
      if (tk==1){quotes <- q} else {quotes <- merge(quotes, q, all=T)}
    } else {
      quotes[[tk]] <- data.frame(ticker=Tickers[tk], q) # make a list
    }
  }
  if (length(what)>1){
    quotes <- do.call(rbind, quotes)   # return a data frame
  }
  if (is.data.frame(quotes)){
    quotes <- subset(quotes, (date >= trueStartDate & date <= endDate))}
  quotes$ticker <- as.character(quotes$ticker)
  return(quotes) 
}		

  
