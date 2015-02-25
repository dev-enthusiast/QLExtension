# Customization run for PJM
# Please check that your list options$quantity.table is consistent with 
# s:/Risk/Projects/Database/ListOfLists.xls 
# Where the numbers in the quantity.table are the subregion ID. 
#
# 

PJM <- function(save, options){

  options$hdata.startDate <- as.Date("2004-04-01")
  options$hdata.endDate   <- as.Date("2005-04-30")
  options$trellis.layout.zone <- c(4,2) 
  options$short.names <- c("West", "East", "NI",   
                           "AEP.DAY", "NJ", "Chicago") # short column names         
  options$ref.name    <- "West"
  options$hdata.symbols <- c("PJMDAM_WESTERN_HUB",    "PJMDAM_EASTERN_HUB",
                             "PJMDAM_N_ILLINOIS_HUB", "PJMDAM_AEPDAYTO_HUB",
                             "PJMDAM_NEW_JERSEY_HUB", "PJMDAM_CHICAGO_HUB")
  options$hdata.filename <-  "hist_pjm_DA_LMP.csv"

  #----------------------------------------Frank convention------------------
  options$quantity.table <- list()        
  options$quantity.table$West    <- 176   # you can concatenate several subregion ID's
  options$quantity.table$East    <- 24
  options$quantity.table$NI      <- 506
  options$quantity.table$AEP.DAY <- c(630, 16)
  options$quantity.table$NJ      <- 33
  options$quantity.table$Chicago <- c(22,651,32)
  options$quantity.table <- stack(options$quantity.table)
    
  return(options)
 }
