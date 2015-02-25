#########################################################
#
test.Path <- function()
{
  library(RUnit)
  rLog("A NEPOOL path")
  load_ISO_environment(region="NEPOOL")
  auction <- parse_auctionName("N14", region="NEPOOL")
 
   
  path <- Path(source.ptid = 4000,
               sink.ptid   = 4008,
               bucket      = "PEAK",
               auction     = auction,
               strategy    = "spec")       # add a strategy 
  as.data.frame(path)


 
  rLog("An SPP path")
  load_ISO_environment(region="SPP")
  auction <- parse_auctionName("N14", region="SPP")

  path <- Path(source.ptid = "AEC",
               sink.ptid   = "BCA",
               bucket      = "PEAK",
               auction     = auction,
               strategy    = "spec")       # add a strategy 
  as.data.frame(path)

  


  
  
}
