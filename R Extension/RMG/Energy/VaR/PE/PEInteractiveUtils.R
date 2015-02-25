source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
source("H:/user/R/RMG/Utilities/write.xls.R")
require(reshape)
library(lattice)

#----------------------------------------------------------------------------------------------------
PEInteractiveUtils <- new.env(hash=TRUE)
#----------------------------------------------------------------------------------------------------

######################################################################################################
# Function used to obtain PE values, given a dataframe QQ (col 2-5 must be curve.name, contract.dt, position
PEInteractiveUtils$getPE <- function(cp.name, FP.options, QQ) {

    loptions            <- NULL
    loptions$asOfDate   <- FP.options$asOfDate
    loptions$calc$hdays <- seq( FP.options$asOfDate-FP.options$N.hist, FP.options$asOfDate, by="day")
    PE$options$asOfDate <- as.Date(FP.options$asOfDate)
    current.mo.1stdt    <- as.Date(format(FP.options$asOfDate, "%Y-%m-01"))
    next.mo.1stdt       <- as.Date(format(as.Date(secdb.dateRuleApply(PE$options$asOfDate, "+1m")), "%Y-%m-01"))
    PE$BASE_SIM_DIR     <- "//nas-msw-07/rmgapp/SimPrices/Network/"  #or put in "Potential Exposure"?
    PE$options$fp       <- FP.options
    PE$CURVE_DATA_DIR   <- FP.options$targetdata.dir

    FP.options$hPtmp.dir = paste(FP.options$report.dir,"HistPriceTemp/", sep="")
    if (file.access(FP.options$hPtmp.dir)!=0){mkdir(FP.options$hPtmp.dir)}
    curvenames <- QQ$curve.name
    for(curvename in curvenames)
    {
      from.dir  <-paste("//nas-msw-07/rmgapp/Prices/Historical","/",tolower(curvename), sep ="")
      to.dir    <- paste(FP.options$hPtmp.dir,"/", tolower(curvename), sep="")
      filecopy  <- file.copy(from.dir, to.dir, overwrite=TRUE)
      if(!filecopy)
      {
        rLog("Historical price file cannot be copied for curve:", curvename)
        QQ <- subset(QQ, curve.name != curvename)
        next()
      }
    }
    hPtemp.dir <- substr(FP.options$hPtmp.dir, 1, nchar(FP.options$hPtmp.dir)-1)
    hPP.env.PF <- .getFileHashEnv(hPtemp.dir)
    if(nrow(QQ)==0) {
      rLog("No historical prices found for this CP:", cp.name)
      return()
    }
    PP         <- get.prices.from.R(QQ, loptions, hPP.env.PF)
    PP.F0      <- subset(PP, select = c("curve.name","contract.dt", as.character(FP.options$asOfDate)))
    rownames(PP.F0) <- c(1:nrow(PP.F0))
    positionList    <- merge(QQ[,-1], PP.F0)
    colnames(positionList)[4] <- "F0"

    #re-adjust positionList to remove expired contract while accounting for contracts that starts in the middle of a month
    Idx1 <- which( positionList$contract.dt < PE$options$asOfDate | is.na(positionList$F0) )
    if( length(Idx1)>0 ){
        positionList <- positionList[-Idx1,]
    }
    #In case there are contract dates that are in the middle of the month 
    #but starts after AOD, will move such positions to the next contract month.
    Idx2 <- which(positionList$contract.dt >= PE$option$asOfDate & positionList$contract.dt< next.mo.1stdt)
    for(ind in Idx2){
        ind.next.mo <- which(positionList$curve.name == positionList[ind,]$curve.name & positionList$contract.dt == next.mo.1stdt)
        if(is.null(ind.next.mo)|length(ind.next.mo)==0) {
          positionList[ind,]$contract.dt <- next.mo.1stdt
        }else{
          positionList[ind.next.mo,"position"] <- positionList[ind.next.mo,"position"] + positionLit[ind,"position"]
          rLog("The positions on", positionList[ind,]$contract.dt,"for curve ", positionList[ind,]$curve.name,
                " is added to next months.")
        }
    }
    #browser()
    res.sPE <- NULL
    if(!is.null(positionList)) res.sPE <- PECalculator$.aggregateSimulations(positionList)
    return(res.sPE)
}
############################################################################################
# PE by quantiles
PEInteractiveUtils$qPE <-function(res.sPE){
     qPE <- NULL
     qPE$direct    <- PECalculator$.summarizeData(res.sPE, mode="direct")
     qPE$indirect  <- PECalculator$.summarizeData(res.sPE, mode="indirect")#note the negative sign is already considered in PECalculator.
     return(qPE)
}
     
############################################################################################
## Function used to plot direct PE quantiles and indirect PE quantiles together
## If the direct and indirect PEs are to be plotted seperately, refer to:
## PECalculator$.outputData() and PECalculator$.plotAggregatedData()
PEInteractiveUtils$plotPE <- function(cpname, FP.options, qPE) {
    if (file.access(FP.options$report.dir)!=0){mkdir(FP.options$report.dir)}
    filename <- paste(FP.options$report.dir, cpname, ".MPE.pdf", sep = "")
    pdf(filename, width=5.5, height =8, pointsize =8)

    # plot the PE results ...
    rangePlot   <- range(as.numeric(qPE$direct),  as.numeric(qPE$indirect))
    pricing.dts <- as.Date(colnames(qPE$direct))

    xlabel <- paste("Pricing date is ", as.character(FP.options$asOfDate),sep="")
    plot( pricing.dts, y=NULL, xlim=range(pricing.dts),
          ylim=rangePlot, type="l", main=cpname,
          ylab="Potential Exposure, M$", xlab=xlabel )
    colorList <- c("black", "blue", "red", "black", "blue", "red")
    for( index in 1:nrow(qPE$direct) )
    {
        lines(pricing.dts, qPE$direct[index,], col=colorList[index])
        points(pricing.dts, qPE$direct[index,], pch=index, col=colorList[index])
        lines(pricing.dts, qPE$indirect[index,], col=colorList[index])
        points(pricing.dts, qPE$indirect[index,], pch=index, col=colorList[index])
    }
    legend( "topright", legend=c(rownames(qPE$direct)),
            col=c(colorList[1:nrow(qPE$direct)]),
            text.col=c(colorList[1:nrow(qPE$direct)]),
            pch=c(1:nrow(qPE$direct)) )
    graphics.off()
}

##############################################################################################
## Funtion to write results to an Excel file
## Note: QQ can be a dataframe that list the details of the positions in the CP. 
## There is no restriction what QQ should contain but it should have curvenames, contract dates,
## and position values.
##
PEInteractiveUtils$write.xls <- function( dMPE, iMPE, QQ, outputDir, cpName, totalPE=FALSE )
{
  file <- paste(outputDir, "results.", cpName, ".xls", sep="")
  if(!totalPE)
  {
    DD <- data.frame(round(t(dMPE), 2))
    names(DD) <- gsub("\\.", "", gsub("X", "p", names(DD)))
    DD <- cbind(contract.dt=colnames(dMPE), DD) 
    write.xls(DD, file, sheetname="PE_direct")
    DD <- data.frame(round(t(iMPE), 2))
    names(DD) <- gsub("\\.", "", gsub("X", "p", names(DD)))
    DD <- cbind(contract.dt=colnames(dMPE), DD)
    write.xls(DD, file, sheetname="PE_indirect")
  }else{
    DD <- data.frame(round(t(dMPE), 2))
    names(DD) <- gsub("\\.", "", gsub("X", "p", names(DD)))
    DD <- cbind(contract.dt=colnames(dMPE), DD) 
    write.xls(DD, file, sheetname="totalPE_direct")
    DD <- data.frame(round(t(iMPE), 2))
    names(DD) <- gsub("\\.", "", gsub("X", "p", names(DD)))
    DD <- cbind(contract.dt=colnames(dMPE), DD)
    write.xls(DD, file, sheetname="totalPE_indirect")
  }
    
  write.xls(QQ, file, sheetname="positions")  
  rLog("Wrote the results.xls file.")

}