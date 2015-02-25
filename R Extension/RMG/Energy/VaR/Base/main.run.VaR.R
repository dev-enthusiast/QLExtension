# Run VaR on a given run parameters data.frame.  The code will figure out if
# it needs to split up the computation.  
#  - run is a list with elements: run.name, and specification
#  - options are set up in set.overnight.run.R
#  - QQ is a data.frame with at least the columns:
#      (curve.name, contract.dt, position, vol.type)
#
# Last modified by Adrian Dragulescu on 30-Mar-2007


main.run.VaR <- function(run, options, QQ=NULL)
{
    if (length(options$make.pdf)==0){options$make.pdf <- TRUE}
    
    cat(paste("Loading curve info ... "))
    backupLoad(options$save$curve.info) # has denomination and region mapping, etc.
    cat("Done!\n")
    cat(paste("Loading fx spot data ... "))
    backupLoad(options$save$fx.file)
    cat("Done!\n")
    
    res <- set.parms.run(run, options)
    run <- res[[1]]; options <- res[[2]]
    if (is.null(QQ)){       # if you pass positions to the calculation...
        QQ <- aggregate.run.positions(run, options)
        if (is.null(QQ)) {
            source("H:/user/R/RMG/Utilities/RLogger.R")
            rLog("Net position is zero. No calculation is needed.")
            return()
        }
    }

    class.Factors <- make.class.Factors(QQ, run, curve.info, options)
    
    hPP <- hVV <- PnL <- NULL
    options <- get.no.packets(QQ, options)     # decide if you need to split
    for (p in 1:options$parallel$no.packets)
    {
        cat("Processing packet", p, "of", options$parallel$no.packets, "\n")
        options$parallel$packet <- p
        if (options$parallel$is.necessary){    
            hPP <- aggregate.run.prices.parallel(QQ, options)
            hVV <- aggregate.run.vols.parallel(QQ, options)
        } else {
            hPP <- aggregate.run.prices.mkt(QQ, options)
            hVV <- aggregate.run.vols.mkt(QQ, options)
        }
        
        if (length(hVV)!=0)
        {
            hPV <- rbind(hPP, hVV)
        } else 
        {
            hPV <- hPP
        }
        # realign positions/class.Factors with the IVaR data
        index <- merge(data.frame(data.frame(QQ, ind.QQ=1:nrow(QQ))),
          data.frame(hPV[,c("curve.name", "contract.dt", "vol.type")],
          ind.hPV=1:nrow(hPV)), all.x=T)

        QQ <- QQ[index$ind.QQ, ]
        class.Factors <- class.Factors[index$ind.QQ, ]
        IVaR <- array(NA, dim=c(nrow(QQ), (ncol(hPV)-3)))       
        IVaR <- hPV[index$ind.hPV, ]
        
        ind.no.mkt <- which(is.na(index$ind.hPV))
        if (length(ind.no.mkt)>0){
          rWarn("Some curves with positions are missing prices/vols!")
          rWarn("These positions below will be dropped:")
          print(QQ[ind.no.mkt, ])      # rLog does not work with data.frames
          index <- index[-ind.no.mkt, ]
          QQ   <- QQ[-ind.no.mkt,]
          IVaR <- IVaR[-ind.no.mkt, ]
          class.Factors <- class.Factors[-ind.no.mkt, ]          
        }

        # add the positions to the IVaR table, and resort
        IVaR <- cbind(position=QQ$position, IVaR)
        IVaR <- IVaR[, c(2:4,1,5:ncol(IVaR))]
        
        hPP <- hVV <- hPV <- NULL; gc()    
        IVaR <- convert.prices.to.USD(IVaR, den, hFX)   
        IVaR <- add.price.vol.fillings(IVaR, options)  # filling of last resort
        if (length(options$run$is.bidweek.day)>0 && options$run$is.bidweek.day){
            IVaR <- adjust.bidweek(IVaR, options)
        }
        
        PnL[[p]] <- calc.PnL.by.factor(IVaR, run, class.Factors, options)
        
        # free up some memory
        if (options$parallel$is.necessary){
            hPP <- hVV <- IVaR <- NULL; gc()
        }   
    } # close the loop over the packets
    
    cat("Start VaR tables ... ")    

    make.VaR.tables(PnL, run, options); cat("Done!\n")

    if (options$calc$do.CVaR){
        IVaR$Changes$CVaR <- calc.CVaR(PnL, IVaR, run, class.Factors, options)}
    if (options$calc$do.incremental.VaR){
        calc.incremental.VaR(PnL, IVaR, run, class.Factors, options)}
    if (options$calc$do.corr.sensitivity){
        calc.correlation.sensitivity(IVaR, options)}
    if (options$make.pdf){
      cat("Start pdf ... ")
      setwd(options$save$reports.dir.run)
      source(paste(options$source.dir,"Base/make.pdf.R",sep=""))
      make.pdf(run, options)
      cat("Done!\n")
    }
    if ((!options$parallel$is.necessary) &&  #-----save IVaR results
            (length(options$save$IVaR.filename)>0)){
        write.xls(IVaR$Prices, options$save$IVaR.filename, sheetname="Prices")
        write.xls(IVaR$Changes, options$save$IVaR.filename, sheetname="Changes")
    }
}

