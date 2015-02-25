#
# .get_buddy_node - how to extend history for certain nodes
# .check_bids  
#


#################################################################
# check bids for fat-finger errors or other ...
# bids should have at least the path info
#   c("path.no", "source.ptid", "sink.ptid", "class.type", "mw", "bid.price")
#
.check_bids <- function(bids)
{
  if (!all(c("path.no", "source.ptid", "sink.ptid", "class.type", "mw",
        "bid.price") %in% names(bids)))
    stop(paste("You're missing one of path.no, source.ptid, sink.ptid,",
               "class.type, mw, bid.price"))

  # check quantities make sense
  qq <- cast(bids, path.no + source.ptid + sink.ptid + class.type ~ ., sum,
              value="mw")
  names(qq)[ncol(qq)] <- "mw"
  qq <- qq[order(-qq$mw), ]
  aux <- subset(qq, mw > 1000)
  if (nrow(aux)>1){
    rLog("\nSTOP: A few paths have total quantity > 1000 MW!!")
    print(aux, row.names=FALSE)
    cat("\n")
  }

  aux <- subset(qq, mw > 200)
  if (nrow(aux)>1){
    rLog("\nWARNING: A few paths have total quantity > 200 MW!")
    print(aux, row.names=FALSE)
    cat("\n")
  }

  
  # check total expenditure makes sense
  bids$expenditure <- bids$mw * bids$bid.price * bids$hours
  xx <- cast(bids, path.no + source.ptid + sink.ptid + class.type ~ ., sum,
              value="expenditure")
  names(xx)[ncol(xx)] <- "expenditure"
  xx <- xx[order(-xx$expenditure), ]
  aux <- subset(xx, abs(expenditure) > 100000)
  if (nrow(aux)>1){
    rLog("\nSTOP: you intend to spend more than $100,000 on a path!!")
    print(aux, row.names=FALSE)
    cat("\n")
  }

  invisible()
}



#################################################################
# Maintain this list by hand
#
.get_buddy_node <- function()
{
  filein <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/CSV/buddy_ptid.csv"
  res <- read.csv(filein)
  structure(res$buddyPtid, names=res$ptid)
}
