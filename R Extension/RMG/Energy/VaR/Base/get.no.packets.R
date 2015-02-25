# Figure out in how many packets to split up the historical data. 
# Called by main.run.VaR.R and also by fill.mktData.R 
#
# Last modified by Adrian Dragulescu on 2-Apr-2007

get.no.packets <- function(QQ=NULL, options){
  memory.max <- memory.limit()    # how much this computer can chug
  Rversion <- as.numeric(paste(R.version$major, gsub("\\.", "",
    R.version$minor), sep=""))
  if (Rversion < 260){memory.max <- memory.max/1024}
  
  if (length(QQ)==0){
    filename <- paste(options$save$datastore.dir, "Prices/hPrices.",
      options$asOfDate, ".RData", sep="")
    backupLoad(filename)
    QQ <- hP                # use hP as a proxy for QQ
    QQ$vol.type <- NA
  }
  memory.QQ  <- 2*object.size(QQ)/(1024*1024) # 2 is for extra overhead.
  options$parallel$is.necessary <- FALSE
  options$parallel$no.packets   <- 1
  options$parallel$no.days.in.packet <- trunc(memory.max/memory.QQ)
  cat("Can fit", options$parallel$no.days.in.packet, "days in memory.\n")
  
  if (options$parallel$no.days.in.packet < options$calc$no.hdays){ 
    options$parallel$is.necessary <- TRUE
    options$parallel$no.packets <- ceiling(options$calc$no.hdays/
                                         options$parallel$no.days.in.packet)
    cat("Need to serialize the VaR calculation and use",
       options$parallel$no.packets, "packets.\n")
  }
  return(options)
}


