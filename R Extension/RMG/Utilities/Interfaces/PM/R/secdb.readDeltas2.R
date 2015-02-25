# An improvement over secdb.readDeltas
# Uses the delta archives not the dgc archives. 
#
#
# Written by AAD on 3/26/2013

###############################################################
# 
# @param asOfDate, an R Date
# @param portfolioBook, the name of a SecDb portfolio or book.  Can be 
#    a vector of books and portfolios. 
# @param discounted, to return discounted deltas or not
# @param aggregate, if TRUE combine the portfolioBooks, otherwise return a list
# @param energyOnly, if TRUE try to remove some non-energy curves.  Not
#   guaranteed to be complete.  
# @return a data.frame with columnames c("curve", "month", "value")
# 
#
secdb.readDeltas2 <- function(asOfDate, portfolioBook, discounted=TRUE,
  aggregate=TRUE, energyOnly=FALSE)
{
  ..read.one <- function(security, asOfDate, discounted, energyOnly)
  {
    #if (security == "NECNEBAL")  browser()
    
    res <- secdb.invoke("_lib elec delta pnl report", "deltapnl::fastdeltas",
      security, asOfDate, discounted)
    if (length(res) == 0)
      return(NULL)
    ctr <- names(res)
    aux <- strsplit(ctr, "\\s", perl=TRUE)
    mkt <- sapply(aux, function(x){substring(x[1], 1, nchar(x[1])-3)})
    myy <- sapply(aux, function(x){substring(x[1], nchar(x[1])-2, nchar(x[1]))})
    rest <- toupper(sapply(aux, function(x) {paste(x[-1], collapse=" ")}))

    qq <- data.frame(portfolioBook=toupper(security),
      curve=paste("COMMOD", mkt, rest),
      month=format.dateMYY(myy), value=as.numeric(unlist(res)))

    # remove non energy items
    if (energyOnly) {
      qq <- subset(qq, !curve %in% c("COMMOD PWX 7X24 ISOFEE PHYSICAL",
                                     "COMMOD PWX 7X24 UCAP PHYSICAL",
                                     "COMMOD ENV BASE PHYSICAL"))
      qq <- qq[!grepl("^COMMOD PWX UCAP ",qq$curve),]
      qq <- qq[!grepl(" 7X24 VLRCST ",qq$curve),]
    }

    qq <- qq[order(qq$portfolioBook, qq$curve, qq$month), ]
    
    qq
  }

  QQ <- lapply(portfolioBook,
    function(security, asOfDate, discounted, energyOnly) {
      ..read.one(security, asOfDate, discounted, energyOnly)
    },
    asOfDate, discounted, energyOnly)


  if (aggregate) {
    QQ <- do.call(rbind, QQ)
    if (is.null(QQ)) {
      rLog("No positions found for asOfDate", format(asOfDate))
    } else {
      QQ <- cast(QQ, curve + month ~ ., sum)
      colnames(QQ)[ncol(QQ)] <- "value"
    }
  }
  
  QQ
}


###############################################################
###############################################################
# 
.test.secdb.readDeltas2 <- function()
{
  require(CEGbase)
  require(SecDb)
  require(reshape)
  
  asOfDate <- as.Date("2012-12-31")
  book <- "NEPPWRBS"
  res  <- secdb.readDeltas2(asOfDate, book)
  head(res)

  asOfDate <- as.Date("2013-03-25")
  book <- "NEPOOL MTM Term Hedges"
  res  <- secdb.readDeltas2(asOfDate, book)
  head(res)

  asOfDate <- as.Date("2013-03-25")
  sec <- c("NEPPWRBS", "NESTAT", "NPFTRMT1", "CONGIBT")
  res  <- secdb.readDeltas2(asOfDate, sec, aggregate=TRUE, energyOnly=TRUE)
  #res  <- secdb.readDeltas2(asOfDate, sec, aggregate=FALSE)
  head(res)

  
  

  
}
