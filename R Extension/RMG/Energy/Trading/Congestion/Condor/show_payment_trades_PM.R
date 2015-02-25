# Show the payment trades that got booked in the PM world.
#
#
# Written by Adrian Dragulescu on 24-Mar-2010


#################################################################
# X are the trades
email_one <- function(to, XX)
{
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  subject <- paste("Payment trades in your books (last ", 
    DAYS_TO_KEEP, " days)", sep="")

  out <- capture.output(print(XX, row.names=FALSE))
  total <- paste("Total: ", sum(XX$value))
  total <- paste(paste(rep(" ", nchar(out[1])-nchar(total)-1),
                       sep="", collapse=""), total)
  
  out <- c(out, "", total)
  
  sendEmail(from, to, subject, out)
}


#################################################################
#
email_trades_to_people <- function(TT)
{
  for (p in people){
    if (p == "steve"){
      to <- "steven.webster@constellation.com"
      PP <- .filter_steve()
    } else if (p == "adrian"){
      to <- "adrian.dragulescu@constellation.com"
      PP <- .filter_adrian()
    } else if (p == "default"){
      to <- "adrian.dragulescu@constellation.com"
      PP <- NULL
    }
    
    XX <- merge(PP, TT)
    if (nrow(XX)==0)
      next
    
    XX <- cast(XX, portfolio + trading.book + eti + trade.type + trade.date +
      employee.id + quantity.uom ~ ., sum, value="notional")
    names(XX)[ncol(XX)] <- "value"
    XX$value <- round(XX$value)
    
    if (any(abs(XX$value) > PNL_THRESHOLD))
      email_one(to, XX)
  }
}


#################################################################
#
.filter_adrian <- function()
{
  books <- .getPortfolioBooksFromSecDb("NEPOOL BASIS PORTFOLIO")
  PP <- data.frame(portfolio="NEPOOL BASIS PORTFOLIO", trading.book=books)
  
  return(PP)
}    


#################################################################
#
.filter_steve <- function()
{
  books <- .getPortfolioBooksFromSecDb( "NEPOOL FP PORTFOLIO" )
  PP <- data.frame(portfolio="NEPOOL FP PORTFOLIO", trading.book=books)
  
  books <- .getPortfolioBooksFromSecDb( "CNE NEWENGLAND PORTFOLIO" )
  PP <- rbind(PP,
   data.frame(portfolio="CNE NEWENGLAND PORTFOLIO", trading.book=books))

  return(PP)
}    
 



#################################################################
#################################################################
options(width=200)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase); require(CEGterm); require(reshape)
require(SecDb)

source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.payment.trades.from.blackbird.R")

## # Initialize SecDb for use on Procmon
## DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
## Sys.setenv(SECDB_SOURCE_DATABASE=DB)
## Sys.setenv(SECDB_DATABASE="Prod")

rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate      <<- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
DAYS_TO_KEEP  <<- 5
PNL_THRESHOLD <<- 2000   # send email if one trade > threshold
people        <<- c("adrian", "steve") #, "default")


rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")

# Pulling the Payment trades from prism (for all CCG!)
TT <- get.payment.trades.from.blackbird(asOfDate=asOfDate)
TT <- TT[,c("eti", "trade.type", "trading.book", "trade.date", "notional",
  "employee.id", "quantity.uom")]
TT <- subset(TT, trade.date >= asOfDate-DAYS_TO_KEEP)


email_trades_to_people(TT)

rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

