################################################################
# no source/sink more than options$bids$max.mw.node
# max bid should have mw > options$bids$min.mw.path
FTR.enforcePathConstraints <- function( bids, options )
{
  if (length(options$bids$max.mw.node)==0){
    options$bids$max.mw.node <- 200}
  if (length(options$bids$min.mw.path)==0){
    options$bids$min.mw.path <- 2.5}

  if (!("source.name" %in% names(bids)))
    stop("Please add source.name, sink.name to bids input variable.")
  
  aux <- aggregate(bids$mw, as.list(bids[,c("class.type", "source.ptid")]), sum)
  aux <- aux[order(aux$x),]
  names(aux)[3] <- "mw"
  aux <- subset(aux, mw > options$bids$max.mw.node)
  aux <- subset(aux, !(source.ptid %in% options$zones.ptid))
  if (nrow(aux)>0){
    rLog("---------------------------------------------")
    rLog("Scaling down these sources:")
    print(aux)
    aux <- merge(aux, unique(bids[,c("path.no", "source.ptid", "class.type")]))
    aux$scale.factor <- aux$mw/options$bids$max.mw.node
    bids <- merge(bids, aux[,c("path.no", "scale.factor")], all.x=T)
    ind <- which(!is.na(bids$scale.factor))
    bids$mw[ind] <- bids$mw[ind] / bids$scale.factor[ind]
  }

  aux <- aggregate(bids$mw, as.list(bids[,c("class.type", "sink.ptid")]), sum)
  aux <- aux[order(aux$x),]
  names(aux)[3] <- "mw"
  aux <- subset(aux, mw > options$bids$max.mw.node)
  aux <- subset(aux, !(sink.ptid %in% options$zones.ptid))
  if (nrow(aux)>0){
    rLog("---------------------------------------------")
    rLog("Scaling down these sinks:")
    print(aux)
    aux <- merge(aux, unique(bids[,c("path.no", "sink.ptid", "class.type")]))
    aux$scale.factor <- aux$mw/options$bids$max.mw.node
    bids$scale.factor <- NULL
    bids <- merge(bids, aux[,c("path.no", "scale.factor")], all.x=T)
    ind <- which(!is.na(bids$scale.factor))
    bids$mw[ind] <- bids$mw[ind] / bids$scale.factor[ind]
  }

  bids$scale.factor <- NULL
  bids <- bids[order(bids$path.no, -bids$bid.price),]

  if (!is.null(options$bids$max.mw.zone))
    bids <- .enforce.zones(bids)

  
  Qmax <- aggregate(bids$mw, list(path.no=bids[,"path.no"]), sum)
  names(Qmax)[2] <- "Qmax"
  bids <- merge(bids, Qmax)
  bids <- bids[order(bids$path.no, -bids$bid.price),]
  
  bids$cum.mw <- as.numeric(sapply(split(bids$mw, bids$path.no),
                                   cumsum))
  return(bids)
}


.enforce.zones <- function(bids){
  


  return(bids)
}

##   fix.min <- function(x, options){
##     if (x[1] < options$bids$min.mw.path){
##       x[1] <- options$bids$min.mw.path}
##     return(x)
##   }
##   aux <- sapply(split(bids$mw, bids$path.no), fix.min, options) 
##   bids$mw <- as.numeric(aux)


