# Calculate the settle price for a set of paths for a given auction
# Ideally you don't need to calculate them, they are already in a 
# database/cache.
#



#############################################################
# Calculate the settle price for a given auction for a
# number of paths.  Ignore missing data.
# @param paths is a list of Path objects, or a data.frame
# @param auction is an FTR Auction object
# @param region the name of the region
# @result is a list with two components settle price and hours.
#   It can happen that you want to see the partial settle for a 
#   CRR path, so you need to return the actual number of hours
#   available in tsdb. 
#
get_settle_price <- function( paths )
{
  if (inherits(paths, "CRR_Path")) 
    paths <- list(paths) 
    
  region <- unique(lapply(paths, function(p) {p$auction$region}))
  if (length(region) > 1)
    stop("Only one region is allowed for your paths.")

  # get the hourly settle price
  sph <- get_settle_price_hourly( paths )

  # average over the hours
  list(sp    = sapply(sph, function(x){ mean(x, na.rm=TRUE) }),
       hours = sapply(sph, length))
}


#############################################################
#############################################################
#
.test_get_settle_price <- function()
{
  
  paths <- FTR:::.get_favorite_paths()
  
  auction <- parse_auctionName( "U13" )
  
  sp <- get_settle_price(paths)
  paths$SP <- sp; print(paths)
  
  
}




