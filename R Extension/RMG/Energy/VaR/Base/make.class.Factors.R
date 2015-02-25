# Make classification variables for one run.  
# For example: run$classification <- c("gasseason")
#
# Written by Adrian Dragulescu on 12-Dec-2006

make.class.Factors <- function(QQ, run, curve.info=NULL, options){
  
  class.Factors <- curve.split <- NULL
  uCF <- unique(unlist(strsplit(run$classification, "\\|")))   # class factors
  if (("market" %in% uCF) |
      ("location" %in% uCF)){
    curve.split <- strsplit(QQ$curve.name, " ")
    mkt         <- sapply(curve.split, function(x){x[2]})
    class.Factors$market <- mkt
  }
  if ("location" %in% uCF){
    if (length(curve.split)==0){curve.split <- strsplit(QQ$curve.name, " ")}
    location    <- sapply(curve.split, function(x){x[3]})
    class.Factors$location <- location    
  }
  if ("type" %in% uCF){
    type <- rep("Vega", nrow(QQ))
    type[which(QQ$vol.type=="NA")] <- "Delta"
    class.Factors$type <- type
  }
  if ("year" %in% uCF){
    class.Factors$year <- format(QQ$contract.dt, "%Y") 
  }
  if ("curve.name" %in% uCF){
    class.Factors$curve.name <- QQ$curve.name
    class.Factors$curve.name <- as.character(class.Factors$curve.name)
  }
  if ("commodity" %in% uCF){
      class.Factors$commodity = as.character(mkt)
      energy = grep("(PW)", mkt)
      class.Factors$commodity[energy] = "Energy"
  }
  if ("in.20072008" %in% uCF){
    ind <- class.Factors$year %in% c("2007","2008")
    class.Factors$in.20072008 <- "not.in.2007-2008"
    class.Factors$in.20072008[ind] <- "in.2007-2008"
  }
  if ("gasseason" %in% uCF){
    gasseason    <- gas.season(QQ$contract.dt)
    cash.month   <- format(options$asOfDate, "%Y-%m")
    prompt.month <- format(seq(as.Date(paste(cash.month,"-01",sep="")),
                    by="month", length.out=2)[2], "%Y-%m")
    month        <- format(QQ$contract.dt, "%Y-%m")
    class.Factors$gasseason <- ifelse(month==cash.month, "cash",
      ifelse(month==prompt.month, "prompt", gasseason))
  }
  if ("region" %in% uCF){
    class.Factors <- merge(class.Factors, curve.info[, c("curve.name",
      "region")], all.x=T, sort=FALSE)
  }
  
  # Pass-through the extra classification factors that you have passed
  # with QQ, e.g. counterparty, accounting.type, etc.
  ind <- which(names(QQ) %in% uCF)
  if (length(ind)>0){
    class.Factors <- data.frame(class.Factors, QQ[, ind])
    aux <- (ncol(class.Factors)-length(ind)+1) : ncol(class.Factors)
    names(class.Factors)[aux] <- colnames(QQ)[ind]
  }
  
  return(data.frame(class.Factors))
}

##   if ("gasseason" %in% run$classification){
##     class.Factors$gasseason <- gas.season(QQ$contract.dt)
##   }
