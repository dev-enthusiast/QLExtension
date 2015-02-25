#########################################################
#
test.get_clearing_price <- function()
{
  
  rLog("A NEPOOL path")
  load_ISO_environment(region="NEPOOL")
  auction <- parse_auctionName("N14", region="NEPOOL")

  path <- Path(source.ptid = 4000,
               sink.ptid   = 4008,
               bucket      = "PEAK",
               auction     = auction)
  
  cp <- get_clearing_price( path )
  checkEquals(cp, 1.940852, tolerance=10^-3)

  rLog("Two NEPOOL paths")
  auction <- parse_auctionName("U13", region="NEPOOL")
  paths <- list(Path(source.ptid = 4000,
                     sink.ptid   = 4001,
                     bucket      = "PEAK",
                     auction     = auction),
                Path(source.ptid = 4000,
                     sink.ptid   = 4001,
                     bucket      = "OFFPEAK",
                     auction     = auction))
  cp <- get_clearing_price( paths )
  checkEquals(cp, c(0.030125, -0.0125), tolerance=10^-5)
 

  rLog("Some NYPP paths")
  load_ISO_environment(region="NYPP")
  auction <- parse_auctionName("K14-6M-R5", region="NYPP")
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
  cp <- get_clearing_price( paths )
  checkEquals(cp, c(0.220000, 3.251526, 3.031526), tolerance=10^-5)
  

  
  
}
