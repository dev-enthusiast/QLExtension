#########################################################
#
test.get_payoff <- function()
{
  
  rLog("A NEPOOL path")
  load_ISO_environment(region="NEPOOL")
  auction <- parse_auctionName("M14", region="NEPOOL")

  path <- Path(source.ptid = 4000,
               sink.ptid   = 4008,
               bucket      = "PEAK",
               auction     = auction)
  
  res <- get_payoff( path )
  checkEquals(res$FROM_START, -426.64, tolerance=10^-5)

  rLog("Three NEPOOL paths different auction")
  paths <- list(Path(source.ptid = 4000,
                     sink.ptid   = 4008,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("F14", region="NEPOOL")),
                Path(source.ptid = 4000,
                     sink.ptid   = 4008,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("G14", region="NEPOOL")),
                Path(source.ptid = 4000,
                     sink.ptid   = 4006,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("F14", region="NEPOOL")))
  res <- get_payoff( paths )
  checkEquals(res$FROM_START, c( -46.73, -510.50, 138.07), tolerance=10^-5)
 

  rLog("Some NYPP paths")
  load_ISO_environment(region="NYPP")
  auction <- parse_auctionName("K14-6M-R6", region="NYPP")
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
  res <- get_payoff( paths, asOfDate=as.Date("2014-08-08") )
  checkEquals(res$FROM_START, c(-3096.450, -8556.083, -5459.633), tolerance=10^-5)
  checkEquals(res$CURRENT_MONTH, c(-382.6054, -1033.5180,  -650.9126), tolerance=10^-5)
  checkEquals(res$NEXT_DAY, c(-5.410598, -77.917554, -72.506957), tolerance=10^-5)
  
  
}
