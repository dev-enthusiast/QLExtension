
#########################################################
#
test.CRR_Auction <- function()
{
  library(RUnit)
  auctionNames <- c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2",
                    "M13-12M", "M13-6M-R1")

  auctions <- parse_auctionName( auctionNames, region="NEPOOL" )
  checkEquals(auctions[[1]]$startDt, as.Date("2010-11-01"))
  checkEquals(auctions[[1]]$noMonths, 1)
  checkEquals(auctions[[1]]$round, 0)
  

  auctions <- parse_auctionName( "K14-6M-R5", region="NYPP" )
  checkEquals(auctions$round, 5)
  
  res <- count_hours_auction("X10", region="NEPOOL")
  checkEquals(res$hours, c(721, 336, 385))
  

  auctions <- archived_auctions(region="NYPP", startDt=as.Date("2014-05-01"))
  aux <- do.call(rbind, lapply(auctions, as.data.frame))
  checkEquals(sort(aux$auctionName),
    c("K14", "K14-1Y-R2", "K14-1Y-R3", "K14-1Y-R4", "K14-2Y-R1",
      "K14-6M-R5", "K14-6M-R6", "K14-6M-R7", "K14-6M-R8"))

  auction <- parse_auctionName("K13-2Y-R2", region="NYPP")
  checkEquals(auction$startDt, as.Date("2013-05-01"))
  checkEquals(auction$endDt, as.Date("2015-04-30"))
  


  
  
}
