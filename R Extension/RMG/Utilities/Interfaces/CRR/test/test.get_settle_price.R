#########################################################
#
test.get_settle_price <- function()
{  
  rLog("A NEPOOL path")
  load_ISO_environment(region="NEPOOL")
  auction <- parse_auctionName("U13", region="NEPOOL")

  path <- Path(source.ptid = 4000,
               sink.ptid   = 4007,
               bucket      = "PEAK",
               auction     = auction)

  sph <- get_settle_price_hourly( path )
  checkEquals(as.numeric(sph[[1]][5]), 0.04, tolerance=10^-5)

  rLog("A NEPOOL path with buddy")
  path <- Path(source.ptid = 1691,
               sink.ptid   = 4006,
               bucket      = "PEAK",
               auction     = parse_auctionName("F12-1Y", region="NEPOOL"),
               source.ptid.buddy = 40327,
               source.ptid.buddy.startDt = as.POSIXct("2012-06-01 01:00:00"))
  sph <- get_settle_price_hourly( path )
  checkEquals(as.numeric(sph[[1]][1710:1720]),
    c(0.04, 0.03, 0.00, 0.01, 0.10, 0.05, 0.04, 0.04, 0.07, 0.07, 0.13), tolerance=10^-5)

 

  
  path <- Path(source.ptid = 4000,
               sink.ptid   = 4007,
               bucket      = "PEAK",
               auction     = auction)
  sp <- get_settle_price( path )
  checkEquals(sp$sp, -4.3238, tolerance=10^-5)

  rLog("Two NEPOOL paths")
  paths <- list(Path(source.ptid = 4000,
                     sink.ptid   = 4001,
                     bucket      = "PEAK",
                     auction     = auction),
                Path(source.ptid = 4000,
                     sink.ptid   = 4001,
                     bucket      = "OFFPEAK",
                     auction     = auction))
  sp <- get_settle_price( paths )
  checkEquals(sp$sp, c(-6.10618, -1.23140), tolerance=10^-5)
 
  rLog("NEPOOL paths, different auctions")
  paths <- list(Path(source.ptid = 4000,
                     sink.ptid   = 4001,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("U13", region="NEPOOL")),
                Path(source.ptid = 4000,
                     sink.ptid   = 4001,
                     bucket      = "OFFPEAK",
                     auction     = parse_auctionName("Q13", region="NEPOOL")))
  sp <- get_settle_price( paths ) 
  checkEquals(sp$sp, c(-6.1061875, -0.006913265), tolerance=10^-5)
  
  rLog("Two NYPP paths")
  load_ISO_environment(region="NYPP")
  auction <- parse_auctionName("K13-6M", region="NYPP")
  paths <- list(Path(source.ptid = 61752,
                     sink.ptid   = 61757,
                     bucket      = "FLAT",
                     auction     = auction),
                Path(source.ptid = 61752,
                     sink.ptid   = 61761,
                     bucket      = "FLAT",
                     auction     = auction),
                Path(source.ptid = 61757,
                     sink.ptid   = 61761,
                     bucket      = "FLAT",
                     auction     = auction))
  sp <- get_settle_price( paths )
  checkEquals(sp$sp, c(-0.31401, 3.21668, 3.53070), tolerance=10^-5)
  
  rLog("An SPP path")
  load_ISO_environment(region="SPP")
  path <- Path(source.ptid = "AEC",
               sink.ptid   = "BCA",
               bucket      = "PEAK",
               auction     = parse_auctionName("N14", region="SPP"),
               strategy    = "spec")       # add a strategy if you want
  as.data.frame(path)
  sp <- get_settle_price( path )
  checkEquals(sp$sp, c(-0.3336648), tolerance=10^-5)
  
   
  
}
