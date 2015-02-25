`FTR.AuctionTermLevels` <-
function( auction )
{
  ind.notMonthly <- grep("-", auction)
  auction.multiMonths <- auction[ind.notMonthly]
  if (length(ind.notMonthly)>0){
    auction <- auction[-ind.notMonthly]            # just monthlies
  }
  mths <- c("F","G","H","J","K","M","N","Q","U","V","X","Z")
  years <- unique(substr(auction, 2, 3))
  all.months <- c(outer(mths, years, paste, sep=""))

  auction <- all.months[which(all.months %in% auction)]
  auction <- c(auction, sort(auction.multiMonths))
  return(auction)
}

