# Make classification variables for one run.  
#
#
# Written by Adrian Dragulescu on 12-Dec-2006

make.class.Factors <- function(IVaR, run, options){
  IVaR <- IVaR$Changes
  curve.split <- strsplit(IVaR$curve.name, " ")
  mkt         <- sapply(curve.split, function(x){x[2]})
  location    <- sapply(curve.split, function(x){x[3]})
  type        <- rep("Vega", nrow(IVaR))
  type[which(is.na(IVaR$vol.type))] <- "Delta"
  class.Factors <- data.frame(year       = format(IVaR$contract.dt, "%Y"),
                              month      = format(IVaR$contract.dt, "%Y-%m"),
                              market     = mkt,
                              location   = location, 
                              type       = type,
                              curve.name = IVaR$curve.name)
  class.Factors$curve.name <- as.character(class.Factors$curve.name)
  if ("in.20072008" %in% run$classification){
    ind <- class.Factors$year %in% c("2007","2008")
    class.Factors$in.20072008 <- "not.in.2007-2008"
    class.Factors$in.20072008[ind] <- "in.2007-2008"
  }
  if ("gasseason" %in% run$classification){
    gasseason    <- gas.season(IVaR$contract.dt)
    cash.month   <- format(options$asOfDate, "%Y-%m")
    prompt.month <- format(seq(as.Date(paste(cash.month,"-01",sep="")),
                    by="month", length.out=2)[2], "%Y-%m")
    class.Factors$gasseason <- ifelse(as.character(class.Factors$month)==cash.month,
      "cash", ifelse(as.character(class.Factors$month)==prompt.month, "prompt",
      gasseason))
  }  
  return(class.Factors)
}

##   if ("gasseason" %in% run$classification){
##     class.Factors$gasseason <- gas.season(IVaR$contract.dt)
##   }
