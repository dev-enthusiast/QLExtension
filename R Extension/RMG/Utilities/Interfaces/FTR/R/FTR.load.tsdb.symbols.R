## #  name, vars, function            
## newvars <- list(
##   list("ri/hub.cong",
##        c("NEPOOL_SMD_DA_4005_congcomp", "NEPOOL_SMD_DA_4000_congcomp"),
##        function(x){x[,1]-x[,2]}),
##   list("ct/hub.cong",
##        c("NEPOOL_SMD_DA_4004_congcomp", "NEPOOL_SMD_DA_4000_congcomp"),
##        function(x){x[,1]-x[,2]}),
##   list("neew.binds",
##        c("NEPL_DA_INTER_NEEW_MW", "NEPL_DA_INTER_NEEW_LIMIT"),
##        function(x){x[,1]-x[,2]})
## )


# Allow the calculation of another derived variable from the 
# tsdb symbols in one pull.
#   newvars is a list of list(varname, function)
#
.calculate_new_vars <- function(hPP, newvars)
{
  hPPext <- hPP
  for (i in seq_along(newvars)){
    rLog("Working on newvariable", newvars[[i]][[1]])
    aux <- hPPext[, newvars[[i]][[2]]]
    bux <- newvars[[i]][[3]](aux)       # apply the function!
    hPPext <- cbind(hPPext, bux)
    colnames(hPPext)[ncol(hPPext)] <- newvars[[i]][[1]]
  }

  return(hPPext)
}

# not used anymore.  Use the aggHourly flag. 
.isIntrahour <- function(symbol){
  res <- FALSE
  res <- res | grepl("^NYPP_RT_ZONE_LBM_", toupper(symbol)) # ny rt market
  res <- res | grepl("^GENRT_", toupper(symbol))    # nepool genscape data
  res <- res | grepl("NEPOOL_RT_SYS_DEMAND", toupper(symbol))
  return(res)
}

#########################################################################
#

FTR.load.tsdb.symbols <- function(tsdb.symbols, start.dt, end.dt,
  newvars=NULL, aggHourly=rep(FALSE, length(tsdb.symbols)))
{
  hPP <- NULL
  for (ind in seq_along(tsdb.symbols)){
    rLog(paste("reading curve", tsdb.symbols[ind]))
    res <- try(HH <- tsdb.readCurve(tsdb.symbols[ind], start.dt, end.dt))
    if (class(res)[1]=="try-error" || nrow(HH)==0){
       next
    }
    indDups <- duplicated(HH$time)
    if (length(indDups) > 0) 
      HH <- HH[!indDups,]  # some symbols have duplicate timestamps, IEMO load!
    
    HH <- zoo(matrix(HH$value), order.by=c(HH[,1]))
    if (aggHourly[ind]) {
      # for intraday data 
      HH <- aggregate(HH, as.POSIXct(format(index(HH)-1,    # hour ending
         "%Y-%m-%d %H:00:00"))+3600, mean, na.rm=TRUE)      # takes a while
    }
    colnames(HH) <- tsdb.symbols[ind]
    hPP[[ind]] <- HH
  }
#browser()
  if (is.null(hPP)) {
    rLog("Check symbol names. No data found for:\n",
         paste(tsdb.symbols, collapse="\n"), sep="")
    return(NULL)
  }
  
  hPP <- hPP[!sapply(hPP, is.null)]  # keep only the ones that are not NULL
  hPP <- do.call(cbind, hPP)  # mangles column names?! 6/5/2013

  if (!is.null(newvars)){
    try(hPP <- .calculate_new_vars(hPP, newvars))
  }
  
  return(hPP)     # return a zoo matrix
}

