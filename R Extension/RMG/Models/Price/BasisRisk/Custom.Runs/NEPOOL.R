# Customization run for NEPOOL
#
#
#


NEPOOL <- function(save, options){

  options$hdata.startDate <- as.Date("2003-04-01")
  options$hdata.endDate   <- options$asOfDate
  options$trellis.layout.zone <- c(4,2) 

  options$short.names <- c("Hub","ME","NH","VT","CT","RI","SEMA",
                           "WMASS","NEMA") # short column names
  options$ref.name    <- "Hub" 
  options$hdata.symbols <- c("NEPOOL_SMD_DA_4000_LMP", "NEPOOL_SMD_DA_4001_LMP",
                             "NEPOOL_SMD_DA_4002_LMP", "NEPOOL_SMD_DA_4003_LMP",
                             "NEPOOL_SMD_DA_4004_LMP", "NEPOOL_SMD_DA_4005_LMP",
                             "NEPOOL_SMD_DA_4006_LMP", "NEPOOL_SMD_DA_4007_LMP",
                             "NEPOOL_SMD_DA_4008_LMP")
  options$hdata.filename <-  "hist_nepool_DA_LMP.csv"

  #----------------------------------------Frank convention------------------
  options$quantity.table <- list()        
  options$quantity.table$Hub    <- c(387, 237)  # hub and seller's choice   
  options$quantity.table$ME     <- 160
  options$quantity.table$NH     <- 178
  options$quantity.table$VT     <- 285
  options$quantity.table$CT     <- c(386, 419) # CT and CT-SW
  options$quantity.table$RI     <- 162
  options$quantity.table$SEMA   <- 238
  options$quantity.table$WMASS  <- 356
  options$quantity.table$NEMA   <- 385
  
  options$quantity.table <- stack(options$quantity.table)

  return(options)
 }
