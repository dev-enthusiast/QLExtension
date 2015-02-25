# Bunch of functions to deal with reports.
#
#
#

##############################################################################
# File Namespace
#
RPT <- new.env(hash=TRUE)
RPT$save$root.dir <- "S:/All/Risk/Reports/FTRs/"


##############################################################################
# Calculate path payoffs.  Requires a data.frame with at least columnames 
# c("source.location.name", "sink.location.name", "class.type")
# imply that you buy the path.
#
RPT$path.payoffs <- function(Paths)
{
  Paths <- Paths[,c("source.name", "sink.name", "class.type")]
  Paths$path.no <- 1:nrow(Paths)
  aux <- melt(Paths, id=c("class.type", "path.no"))
  names(aux)[4] <- "name"

  Paths <- split(Paths, Paths$class.type)

  
  aux <- merge(aux, NEPOOL$MAP[,c("name", "tsdb.symbol")], all.x=T)

  aux <- merge(aux, NEPOOL$CP[,c("class.type", "name",
                                 "clearing.price.per.hour", "auction")])
  ind <- grep("^source", aux$variable)  
  aux$clearing.price.per.hour[ind] <- -aux$clearing.price.per.hour[ind]
  names(aux)[which(names(aux)=="clearing.price.per.hour")] <- "value"
  aux <- cast(aux, path.no + auction ~ ., sum)
  names(aux)[3] <- "CP"

  ind <- grep("-", aux$auction)
  aux <- aux[-ind,]

  hSP <- vector("list", length=length(Paths)) # by bucket
  names(hSP) <- names(Paths)
  for (ind.b in 1:length(Paths)){
    bucket <- names(Paths)[ind.b]
    hSP[[ind.b]] <- matrix(NA, ncol=nrow(Paths[[bucket]]),
                           nrow=nrow(NEPOOL$hP[[bucket]]))
    hSP <- zoo(NA, index=index(NEPOOL$hP[[bucket]]))

    
  }
  
  lvls <- ut.FTR$.AuctionTerm(NEPOOL$SP)

  
  p <- xyplot(CP ~ auction | path.no, data=aux, layout=c(10,2,61))
  
  p <- xyplot(CP ~ auction | path.no, data=aux, layout=c(10,3,4))
  print(p[1])
  
}

##############################################################################
# Calculate path payoffs.  Requires a data.frame with columnames 
# c("source.location.name", "sink.location.name", "buy.sell", "class.type")
#
RPT$payoffs.summary <- function(PP)
{
  
}
