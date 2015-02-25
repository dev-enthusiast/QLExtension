# Aggregate all the prices necessary for this run.  Requires a position 
# data.frame with curve.name, contract.dt, vol.type, position variables. 
# Runs that fit in memory use the aggregate.run.mkt branch.
# Runs that are too big will use the aggregate.run.parallel branch.  (slower)
#
# Last modified by Adrian Dragulescu on 27-Mar-2006 

overlay.filled.marks <- function(filename, hPV, options)
{
    # filename = path to filled marks
    backupLoad(filename)
    
    fPV <- if (exists("fPP")){fPP} else {fVV}
    aux <- merge(
            cbind(ind.hPV=1:nrow(hPV), hPV[,c("curve.name", "contract.dt", "vol.type")]),
            cbind(ind.fPV=1:nrow(fPV), fPV[,c("curve.name", "contract.dt", "vol.type")])
            )
    fPV <- fPV[,which(names(fPV) %in% as.character(options$calc$hdays)), drop=F] # only the days you need
    cnames <- c("curve.name", "contract.dt", "vol.type", as.character(options$calc$hdays))
    ind <- cnames %in% names(hPV)
    if (length(ind)>4)
    {                          
        # probably a new market & you're missing history
        bux <- data.frame(hPV[,c("curve.name", "contract.dt", "vol.type")],
                array(NA, dim=c(nrow(hPV), (options$calc$no.hdays+1))))
        names(bux) <- cnames
        bux[,which(cnames %in% names(hPV))] <- hPV
        hPV <- bux
    }
    
    # overlay the fillings
    if ((nrow(aux)>0) && (ncol(fPV)>0))
    {
      hPV[aux$ind.hPV, colnames(fPV)] <- fPV[aux$ind.fPV, ]
    }

    return(hPV)
}

aggregate.run.prices.mkt <- function(QQ, options)
{
    QQ <- unique(QQ[, c("curve.name", "contract.dt", "vol.type")])
    QQ <- subset(QQ, vol.type == "NA")       # pick up the deltas only
    curve.split <- strsplit(QQ$curve.name, " ")
    mkt  <- sapply(curve.split, function(x){x[2]})   # get the market
    uMkt <- unique(mkt)
    hPP  <- NULL
    for (m in 1:length(uMkt)){
        cat("Loading prices market ", uMkt[m], "\n", sep="")
        filename <- paste(options$save$prices.mkt.dir, "all.", uMkt[m],
                ".Prices.RData", sep="")
        backupLoad(filename)
        hP.mkt$vol.type <- "NA"
        mQQ <- subset(QQ, mkt == uMkt[m])
        aux <- merge(mQQ, hP.mkt, all.x=T)
        ind <- c(1:3, which(names(aux) %in% as.character(options$calc$hdays)))
        aux <- aux[,ind]    # pick only the days you need
        filename <- paste(options$save$prices.mkt.dir, "filled.", uMkt[m],
                ".Prices.RData", sep="")
        if (file.exists(filename) & options$calc$fill.marks){
            aux <- overlay.filled.marks(filename, aux, options)
        }
        if (length(hPP)==0)
        {
            hPP <- aux
        } else 
        {
            if( ncol(hPP) != ncol(aux) )
            {
                next
            }

            hPP <- rbind(hPP, aux)
        }
    }
    return(hPP)     # return the positions too
}
aggregate.run.vols.mkt <- function(QQ, options){
    QQ <- unique(QQ[, c("curve.name", "contract.dt", "vol.type")])
    QQ <- QQ[QQ$vol.type != "NA",]        # pick up the vegas only
    if (nrow(QQ)==0){return(NULL)}
    curve.split <- strsplit(QQ$curve.name, " ")
    mkt  <- sapply(curve.split, function(x){x[2]})   # get the market
    uMkt <- unique(mkt)
    hVV  <- NULL
    for (m in 1:length(uMkt)){
        cat("Loading vols market ", uMkt[m], "\n", sep="")
        mQQ <- subset(QQ, mkt == uMkt[m])
        filename <- paste(options$save$vols.mkt.dir, "all.", uMkt[m],
                ".Vols.RData", sep="")
        if (file.exists(filename)){
            backupLoad(filename)
            aux <- merge(mQQ, hV.mkt, all.x=T)
            ind <- c(1:3, which(names(aux) %in% as.character(options$calc$hdays)))
            aux <- aux[,ind]    # pick only the days you need
        } else {              # in a perfect world this else branch is not needed!!!
            cat("No vols for ", uMkt[m], ".  Call Jenny Zhang now!\n", sep="")
            aux <- array(0, dim=c(nrow(mQQ), length(options$calc$hdays)), dimnames=list(
                            1:nrow(mQQ), as.character(options$calc$hdays)))
            aux <- cbind(mQQ, aux)
        }
        filename <- paste(options$save$vols.mkt.dir, "filled.", uMkt[m],
                ".Vols.RData", sep="")
        if (file.exists(filename)){
            aux <- overlay.filled.marks(filename, aux, options)
        }
        if (length(hVV)==0)
        {
            hVV <- aux
        } else 
        {
            if(ncol(hVV) != ncol(aux))
            {
                rWarn("Couldn't bind vols for market", uMkt[m])
            }else
            {
                hVV <- rbind(hVV, aux)
            }
        }
    }
    
    #Remove duplicate combinations
    hVV = hVV[rownames(unique(hVV[,1:3])),]
    
    return(hVV)     # return the positions too
}


aggregate.run.vols.sas <- function( QQ, options )
{
    vegas = QQ[ -(which(QQ$vol.type=="NA")), ]
    vegas <- unique(vegas[, c("curve.name", "contract.dt", "vol.type")])
    
    hVV = vegas
    for( dateToLoad in as.character(options$calc$hdays) )
    {
        cat("Loading SAS vols for", dateToLoad, "\n")
        fileName = paste( options$save$datastore.dir, "Vols/sasVols.", 
                dateToLoad, ".RData", sep="" )
        backupLoad( fileName )
        
        hVV = merge(hVV, hV, all.x=TRUE)
    }
    
    return(hVV)
}


#######################################################################################
# Extra functionality, not used now.
#
aggregate.run.prices.parallel <- function(QQ, options){
    QQ <- QQ[QQ$vol.type == "NA",]       # pick up the deltas only
    p  <- options$parallel$packet
    days <- ((p-1)*options$parallel$no.days.in.packet+1):min(options$calc$no.hdays+1,
            p*options$parallel$no.days.in.packet)
    hPP <- cbind(QQ, array(NA, dim=c(nrow(QQ), length(days))))
    names(hPP)[5:ncol(hPP)] <- as.character(options$calc$hdays[days])
    for (d in days){
        cat("Loading prices for day", as.character(options$calc$hdays[d]))
        filename <- paste(options$save$datastore.dir, "Prices/hPrices.",
                options$calc$hdays[d], ".RData", sep="")
        backupLoad(filename)
        hP$vol.type <- "NA"
        
        hP <- hP[!duplicated(hP[,c("curve.name", "contract.dt")]),]  # this should not be here!
        aux <- merge(QQ, hP, all.x=T)
        hPP[,as.character(options$calc$hdays[d])] <- aux[,ncol(aux)] # last hP colum 
        cat(" Done.\n")                                              # has the prices
    }
    return(hPP)     # return the positions too
}
aggregate.run.vols.parallel <- function(QQ, options){
    QQ <- QQ[QQ$vol.type != "NA",]        # pick up the vegas only
    if (nrow(QQ)==0){return(NULL)}
    p  <- options$parallel$packet
    days <- ((p-1)*options$parallel$no.days.in.packet+1):min(options$calc$no.hdays+1,
            p*options$parallel$no.days.in.packet)
    hVV <- NULL
    for (d in days){
        cat("Loading prices for day", as.character(options$calc$hdays[d]))
        filename <- paste(options$save$datastore.dir, "Prices/hVols.",
                options$calc$hdays[d], ".RData", sep="")
        backupLoad(filename)
        aux <- merge(QQ, hV, all.x=T)
        hVV[,as.character(options$calc$hdays[d])] <- aux[,ncol(aux)] # last hV colum 
        cat(" Done.\n")                                              # has the vol
    }
    return(hVV)     # return the positions too
}
#    if (length(hPP)==0){hVV <- aux} else {hVV <- cbind(hVV, aux)}
