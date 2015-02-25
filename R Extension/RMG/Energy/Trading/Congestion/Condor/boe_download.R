# Get the gbpusd exchange rate from the Bank of England 
# as agreed for the Distrigas contract
#


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(CEGbase)
require(SecDb)
require(Tsdb)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.BOE.download.R")  

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

# get the GBP/USD rate
fx <- .read_from_boe(startDt=Sys.Date()-30,
                     endDt=Sys.Date(),
                     series=c("XUDLUSS"))
if (nrow(fx) == 0) {
  returnCode <- 1  
} else {
  res <- .write_to_tsdb( fx )
}


rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}





