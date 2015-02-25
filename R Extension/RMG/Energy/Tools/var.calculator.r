# Calculate VaR on adjusted IVaR delta positions
#
#
# Written by Michael Zhang 12/26/2006

var.calculator <- function(file.name)  {
    require(reshape)
    options <- NULL
    options$source.dir <- "H:/user/R/RMG/Energy/"
    options$save$reports.dir.run <- "c:/temp/"
    source(paste(options$source.dir,"VaR/rmsys/core.calc.VaR.R",sep=""))
    source(paste(options$source.dir,"VaR/rmsys/make.VaR.report.R",sep=""))

    # Import price differnces, volatility differences, delta, and vega
    file <- paste(options$source.dir,"Tools/LondonVaRCalc.xls", sep="")
    con <- odbcConnectExcel(file)
    data.delta <- sqlFetch(con, "Prc_Chgs")
    data.vega <- sqlFetch(con, "Vol_Chgs")
    odbcCloseAll()

    colnames(data.delta)[which(colnames(data.delta) == "delta")] <- "value"
    DD <- cbind(format(as.Date(substr(data.delta$rd_ref, 11, 18), "%d%b%y"), "%Y"),
                format(as.Date(substr(data.delta$rd_ref, 11, 18), "%d%b%y"), "%b"),
                data.delta)
    colnames(DD)[1:3] <- c("year", "month", "curve.name")
    colnames(data.vega)[which(colnames(data.vega) == "vega")] <- "value"
    VV <- cbind(format(as.Date(substr(data.vega$rd_ref, 12, 19), "%d%b%y"), "%Y"),
                format(as.Date(substr(data.vega$rd_ref, 12, 19), "%d%b%y"), "%b"),
                data.vega)
    colnames(VV)[1:3] <- c("year", "month", "curve.name")
    IVaR$Changes <- rbind(DD, VV)
    indDates <- grep("_00_",colnames(IVaR$Changes))
    dP  <- IVaR$Changes[,indDates]
    pos <- IVaR$Changes[,c("value")]
    PnL <- dP * pos
    VaR <- list(Total=4*sd(colSums(PnL, na.rm=T)))

    class.Factors <- IVaR$Changes[, c("curve.name", "year", "month" )]

    res  <- core.calc.VaR(PnL, class.Factors[, c("curve.name","year")])
    VaR[["curve.name"]] <- c(VaR[["curve.name"]], list(year=list(Total=res)))
    OutT <- cast(res, curve.name ~ year)
    OutT$curve.name <- as.character(OutT$curve.name)
    return(OutT)
}