# Load SAS IVaR file
#
#
# Written by Adrian Dragulescu on 21-Nov-2006


load.SAS.IVaR <- function(options){

  if (!file.exists(options$file)){
    cat("Cannot find the input IVaR file.  Please check.")
    flush.console()
  }
  con          <- odbcConnectExcel(options$file)
  IVaR.Delta   <- sqlFetch(con, "Prc_Chgs")
  IVaR.Vega    <- sqlFetch(con, "Vol_Chgs")
  odbcCloseAll()
  IVaR.Delta[1:10,]

#  IVaR.Delta <- clean.IVaR(IVaR.Delta, options)    # clean the inputs a little
  ind <- which(colnames(IVaR.Delta) %in% c("CVaR","MVaR"))
  if (length(ind)>0){IVaR.Delta <- IVaR.Delta[,-ind]}
  IVaR <- cbind(IVaR.Delta, type="DELTA")
  if (ncol(IVaR.Vega)>1){               # make sure you have vega contribution
#    IVaR.Vega  <- clean.IVaR(IVaR.Vega, options)   # clean the inputs a little
    colnames(IVaR.Vega)[which(colnames(IVaR.Vega)=="vega")] <- "delta"
    IVaR <- rbind(IVaR, cbind(IVaR.Vega, type="VEGA"))
  }
  if (length(options$startDate)>0 && !is.na(options$startDate)){
    IVaR <- subset(IVaR, contracts >= options$startDate)}
  if (length(options$startDate)>0 && !is.na(options$endDate)){
    IVaR <- subset(IVaR, contracts <= options$endDate)}
  
  ind <- which(colnames(IVaR) %in% c("rd_ref"))
  IVaR[,ind] <- gsub("[^:alnum:]*_","",IVaR$rd_ref)
  IVaR[,ind] <- as.Date(IVaR[,ind], format="%d%B%y")
  colnames(IVaR)[ind] <- "contract.dt"

  colnames(IVaR) <- gsub("_00_00_00", "", colnames(IVaR))
  colnames(IVaR)[1:2] <- c("commod.name", "position")
  IVaR$commod.name <- as.character(IVaR$commod.name)
#  IVaR$contract.dt <- as.Date(IVaR$contract.dt)
  return(IVaR)
}
