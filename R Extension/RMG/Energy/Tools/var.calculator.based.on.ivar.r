# Calculate VaR on adjusted IVaR delta positions
#
#
# Written by Michael Zhang 12/26/2006

var.calculator <- function(file.name)  {
    require(RODBC)
    require(reshape)
    require(xtable)
    options <- NULL
    options$source.dir <- "S:/All/Risk/Software/R/prod/Energy/"
    options$spreadsheet.dir <- "C:/Risk/R/London/"
    #options$source.dir <- "C:/Risk/Projects/R/RMG/Energy/"
    options$save$reports.dir.run <- "c:/temp/"
    source(paste(options$source.dir,"VaR/rmsys/core.calc.VaR.R",sep=""))
    source(paste(options$source.dir,"VaR/rmsys/make.VaR.report.R",sep=""))

    # Import price differnces, volatility differences, delta, and vega
    #fileName <- paste(options$spreadsheet.dir,"VaRCalculator.xls", sep="")
    fileName <- file.name
    con <- odbcConnectExcel(fileName)
    data.delta <- sqlFetch(con, "Prc_Chgs")
    data.vega <- sqlFetch(con, "Vol_Chgs")
    odbcCloseAll()

    colnames(data.delta)[which(colnames(data.delta) == "delta")] <- "value"
    #DD <- cbind(format(as.Date(substr(data.delta$rd_ref, 11, 18), "%d%b%y"), "%Y"),
    #            format(as.Date(substr(data.delta$rd_ref, 11, 18), "%d%b%y"), "%b"),
    #            data.delta)
    colnames(data.delta)[1] <- c("curve_name")
    colnames(data.vega)[which(colnames(data.vega) == "vega")] <- "value"
    #VV <- cbind(format(as.Date(substr(data.vega$rd_ref, 12, 19), "%d%b%y"), "%Y"),
    #            format(as.Date(substr(data.vega$rd_ref, 12, 19), "%d%b%y"), "%b"),
    #            data.vega)
    colnames(data.vega)[1] <- c("curve_name")
    #VaR$Changes <- rbind(DD, VV)
    IVaR <- NULL
    indDates <- grep("_00_",colnames(data.delta))
    IVaR$PriceChange <- data.delta[,indDates]
    IVaR$Delta <- data.delta[,c("value")]
    IVaR$DeltaFactor <- data.delta[, c("curve_name", "contract_month", "year")]
    IVaR$VolatilityChange <- data.vega[,indDates] 
    IVaR$Vega <- data.vega[,c("value")]
    IVaR$VegaFactor <- data.vega[, c("curve_name", "contract_month", "year")]
    IVaR$Change <- rbind(  IVaR$PriceChange,   IVaR$VolatilityChange )
    #IVaR$Change <- IVaR$VolatilityChange
    IVaR$Position <- c(  IVaR$Delta,  IVaR$Vega )
    #IVaR$Position <- IVaR$Vega
    IVaR$PnL <- IVaR$Change * IVaR$Position
    IVaR$ClassFactor <- rbind(  IVaR$DeltaFactor,  IVaR$VegaFactor )
    #IVaR$ClassFactor <- IVaR$VegaFactor
    #dP  <- IVaR$Changes[,indDates]
    #pos <- IVaR$Changes[,c("value")]
    #PnL <- dP * pos                                                    
    VaR <- list(Total=4*sd(colSums(IVaR$PnL, na.rm=T)))

    class.Factors <- IVaR$ClassFactor[, c("curve_name", "year")]

    for (i in 1:ncol(class.Factors)){
      cFactor <- data.frame(class.Factors[,i])
      colnames(cFactor) <- colnames(class.Factors)[i]
      VaR[[colnames(cFactor)]] <- list(Total=core.calc.VaR(IVaR$PnL, cFactor))
    }
  
    res  <- core.calc.VaR(IVaR$PnL, IVaR$ClassFactor[, c("curve_name", "year")])
    VaR[["curve_name"]] <- c(VaR[["curve_name"]], list(year=list(Total=res)))
    OutT <- cast(res, curve_name ~ year)
    OutT$curve_name <- as.character(OutT$curve_name)
    ind <- which(sapply(OutT, is.numeric))
    OutT[,ind] <- apply(OutT[,ind], 2, function(x){gsub("NA", "  ",x)})
    
    if (!is.data.frame(OutT)){OutT <- data.frame(OutT)}
    colnames(OutT) <- gsub("X","",colnames(OutT)) # if cast was used
    var.names <-  c("curve_name","year")
    OutT <- cbind(OutT, Total=VaR[[var.names[1]]]$Total$value)
    OutT <- rbind(OutT, c(NA, VaR[[var.names[2]]]$Total$value,VaR$Total))
    OutT[nrow(OutT),1] <- "Total"
    d <- data.frame(OutT)
    return(d)
}
