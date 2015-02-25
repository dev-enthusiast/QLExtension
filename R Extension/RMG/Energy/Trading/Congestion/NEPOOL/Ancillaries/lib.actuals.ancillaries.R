# Functions to do the monthly actuals for ancillaries
#
# .actuals_fwdres
# .actuals_lossrmb
# .actuals_opres
# .actuals_rmr
# .actuals_volt
# .calculate_actuals
# .monthly_actuals_load  - get the load actuals by zone     
# .report_actuals        - make the xlsx file with results 
#


################################################################
#
.actuals_fwdres <- function( month )
{
  rLog("------- FwdRes actuals for ", format(month) )
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.reserves.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.fwd.reserves.R")

  rLog("get fwdres positions")
  asOfDate  <- as.Date(secdb.dateRuleApply(month, "-1b"))
  if (asOfDate == as.Date("2013-07-31"))
    asOfDate  <- asOfDate - 1
  qqA <- get_positions_fwdres_ccg(asOfDate)
  qqB <- get_positions_fwdres_cne(asOfDate)
  qq1 <- rbind(qqA, qqB)
  qq1 <- qq1[which(qq1$month == month),]
  colnames(qq1)[1:2] <- c("company", "serviceType")
  qq1$serviceType <- toupper(qq1$serviceType)
  qq1 <- subset(qq1, SettlementType == "Physical")    # take the physical for settlements
  qq1$SettlementType <- NULL
  
  # get the fwdres marks
  PP1 <- pull_marks_fwdres(asOfDate,  startDate=month, endDate=month, service="FWDRES")
  PP1$asOfDate <- NULL

  # get the actual fwdres rate
  RR <- .monthly_actuals_rtreserves( month )
  fwdres <- RR[,c("rop.fwdres.rate", "ct.fwdres.rate", "nema.fwdres.rate")]
  PP2 <- data.frame(month=index(fwdres), bucket=rep("PEAK", 3),
    serviceType="FWDRES", location=c("CT", "NEMA", "ROP") )  
  PP2$value <- c(as.numeric(fwdres$ct.fwdres.rate),
    as.numeric(fwdres$nema.fwdres.rate), as.numeric(fwdres$rop.fwdres.rate) )

  if (month == currentMonth() ) {
    rLog("Adjust intra-month quantity for actual number of days.")
    scale <- as.numeric(Sys.Date()-month-3)/as.numeric(nextMonth()-month+1)
    qq1$value <- qq1$value * scale
  }
  # get the actual 5x16 loads
  aux <- .monthly_actuals_load(month, bucket="5X16")
  qq2.rop <- data.frame(location="ROP", cast(aux, company + month ~ ., sum,
    subset=location %in% c("ME", "NH", "VT", "RI", "SEMA", "WMA") ))
  colnames(qq2.rop)[ncol(qq2.rop)] <- "value"
  qq2 <- rbind(qq2.rop, subset(aux, location %in% c("NEMA", "CT")))
  qq2$serviceType <- "FWDRES"
  
  
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  aux[[2]] <- cbind(serviceType="FWDRES", aux[[2]])
  
  aux
}


################################################################
#
.actuals_lossrmb <- function( month )
{
  rLog("------- LossRmb actuals for ", format(month) )
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.lossrmb.R")

  rLog("get lossrmb positions")
  asOfDate  <- as.Date(secdb.dateRuleApply(month, "-1b"))
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL LOAD PORTFOLIO" )
  qqA <- cbind(company="ccg.load",
         get_positions_lossrmb(asOfDate, books, month=month))
  
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NEWENGLAND PORTFOLIO" )
  qqB <- cbind(company="cne.load",
         get_positions_lossrmb(asOfDate, books, month=month))
  qq1 <- rbind(qqA, qqB)
  qq1 <- qq1[which(qq1$month == month),]
  
  # get the lossrmb marks
  PP1 <- .pull_marks_lossrmb(asOfDate,  startDate=month, endDate=month)
  PP1$asOfDate <- NULL

  # get the actual fwdres rate
  LR <- .monthly_actuals_lossrmb( month ) 
  PP2 <- data.frame(month=month, bucket="FLAT",
    serviceType="LOSSRMB")  
  PP2$value <- as.numeric(LR$ccg.lossrev.rate + LR$cne.lossrev.rate)/2
  PP2$location <- "HUB"

  if (month == currentMonth() ) {
    rLog("Adjust intra-month quantity for actual number of days.")
    scale <- as.numeric(Sys.Date()-month)/as.numeric(nextMonth()-month+1)
    qq1$value <- qq1$value * scale
    qq2 <- qq1
    qq2[which(qq2$company=="ccg.load"),"value"] <- as.numeric(LR$ccg.rt.adjload)
    qq2[which(qq2$company=="cne.load"),"value"] <- as.numeric(LR$cne.rt.adjload)
  } else {
    # get the actual loads
    qq2 <- data.frame(company = c("ccg.load", "cne.load"),
      serviceType = "LOSSRMB",
       month = month,
       value = c(as.numeric(LR$ccg.rt.adjload), as.numeric(LR$cne.rt.adjload)))
  }

  
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year"))  # from lib.pnl.R
  aux[[2]] <- cbind(serviceType="LOSSRMB", aux[[2]])
  
  aux
}


################################################################
#
.actuals_opres <- function( month )
{
  rLog("------- OpRes actuals for ", format(month) )
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.reserves.R")
  
  # get positions
  asOfDate  <- as.Date(secdb.dateRuleApply(month, "-1b"))
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg.load", get_positions_opres(asOfDate, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne.load", get_positions_opres(asOfDate, books))
  qq1   <- rbind(qqA, qqB)
  qq1   <- qq1[which(qq1$month == month),]

  # get the opres marks
  PP1 <- .pull_marks_opres(asOfDate,  startDt=month, endDt=month, adjustOilRFP=TRUE)
  PP1$source <- NULL

  # get the actual opres rate
  OpRes <- .monthly_actuals_opres(month)
  PP2 <- data.frame(month=index(OpRes), bucket=rep("FLAT", 4*nrow(OpRes)),
    serviceType="OPRES")
  PP2$location <- rep(c("RT", "DA", "CTRT", "NEMART"), each=nrow(OpRes))
  PP2$value <- c(as.numeric(OpRes$opres.rt.rop.rate), as.numeric(OpRes$opres.da.rate),
    as.numeric(OpRes$opres.rt.ct.rate), as.numeric(OpRes$opres.rt.nema.rate))

  if (month == currentMonth() ) {
    rLog("Adjust intra-month quantity for actual number of days.")
    scale <- as.numeric(Sys.Date()-month)/as.numeric(nextMonth()-month+1)
    qq1$value <- qq1$value * scale
    qq2 <- qq1
  } else {
    # get the actual loads
    aux <- .monthly_actuals_load(month, bucket="7X24")
    qq2.da <- ddply(aux, .(company, month), function(x){
      data.frame(location="DA", value=sum(x$value)) } )
    qq2.ctrt <- ddply(subset(aux, location=="CT"), .(company, month),
      function(x){data.frame(location="CTRT", value=sum(x$value))})
    qq2.nemart <- ddply(subset(aux, location=="NEMA"), .(company, month),
      function(x){data.frame(location="NEMART", value=sum(x$value))})
    qq2.rt <- ddply(subset(aux, location %in% c("ME", "NH", "VT", "RI",
      "SEMA", "WMA")), .(company, month),
      function(x){data.frame(location="RT", value=sum(x$value))})
    
    qq2 <- rbind(qq2.da, qq2.ctrt, qq2.nemart, qq2.rt) 
  }

  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  aux[[2]] <- cbind(serviceType="OPRES", aux[[2]])
  
  aux
}


################################################################
#
.actuals_rmr <- function( month )
{
  rLog("------- RMR actuals for ", format(month) )
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.rmr.R")

  # get positions
  asOfDate  <- as.Date(secdb.dateRuleApply(month, "-1b"))
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg.load", get_positions_rmr(asOfDate, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne.load", get_positions_rmr(asOfDate, books))
  qq1   <- rbind(qqA, qqB)
  qq1   <- qq1[which(qq1$month == month),]

  # get the rmr marks
  PP1 <- .pull_marks_rmr(asOfDate,  startDate=month, endDate=month)
  PP1$asOfDate <- NULL
  PP1$value[!is.finite(PP1$value)] <- 0

  # get the rmr actuals
  rmr <- .monthly_actuals_rmr( month )
  if ( is.null(rmr) )
    return(NULL)
  
  PP2 <- data.frame(month=index(rmr), bucket="FLAT", serviceType="RMR",
    location=c(
      "NEMA RT", "SEMA RT", "WCMA RT", "MAINE RT", "NH RT", "RI RT", "CT RT", "VT RT",
      "NEMA DA", "SEMA DA", "WCMA DA", "MAINE DA", "NH DA", "RI DA", "CT DA", "VT DA") )
  PP2$value <- c(
    as.numeric(rmr$nema.rt.lscp.rate), as.numeric(rmr$sema.rt.lscp.rate),
    as.numeric(rmr$wma.rt.lscp.rate),  as.numeric(rmr$me.rt.lscp.rate),
    as.numeric(rmr$nh.rt.lscp.rate),   as.numeric(rmr$nh.rt.lscp.rate),
    as.numeric(rmr$ct.rt.lscp.rate),   as.numeric(rmr$vt.rt.lscp.rate),
                 
    as.numeric(rmr$nema.da.lscp.rate), as.numeric(rmr$sema.da.lscp.rate),
    as.numeric(rmr$wma.da.lscp.rate),  as.numeric(rmr$me.da.lscp.rate),
    as.numeric(rmr$nh.da.lscp.rate),   as.numeric(rmr$ri.da.lscp.rate),
    as.numeric(rmr$ct.da.lscp.rate),   as.numeric(rmr$vt.da.lscp.rate))

  if (month == currentMonth() ) {
    rLog("Adjust intra-month quantity for actual number of days.")
    scale <- as.numeric(Sys.Date()-month)/as.numeric(nextMonth()-month+1)
    qq1$value <- qq1$value * scale
    qq2 <- qq1
  } else {
    # get the actual loads
    aux <- .monthly_actuals_load(month, bucket="7X24")
    aux$loc <- aux$location
    aux$loc[which(aux$loc=="ME")] <- "MAINE"
    aux$loc[which(aux$loc=="WMA")] <- "WCMA"
    
    qq2 <- rbind(
      cbind(aux[,c("company", "month", "value")], location=paste(aux$loc, "DA")),
      cbind(aux[,c("company", "month", "value")], location=paste(aux$loc, "RT")))
  }
  
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  aux[[2]] <- cbind(serviceType="RMR", aux[[2]])

  
  aux
}


################################################################
#
.actuals_volt <- function( month )
{
  rLog("------- VOLT actuals for ", format(month) )
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.volt.R")

  # get positions
  asOfDate  <- as.Date(secdb.dateRuleApply(month, "-1b"))
  qq <- get_positions_volt(asOfDate)

  # get the volt marks
  PP1 <- .pull_marks_volt(asOfDate,  startDate=month, endDate=month)
  PP1$asOfDate <- NULL
  PP1$value[!is.finite(PP1$value)] <- 0

  # get the rmr actuals
  rmr <- .monthly_actuals_rmr( month )
  PP2 <- data.frame(month=index(rmr), bucket="FLAT", serviceType="RMR",
    location=c(
      "NEMA RT", "SEMA RT", "WCMA RT", "MAINE RT", "NH RT", "RI RT", "CT RT", "VT RT",
      "NEMA DA", "SEMA DA", "WCMA DA", "MAINE DA", "NH DA", "RI DA", "CT DA", "VT DA") )
  PP2$value <- c(
    as.numeric(rmr$nema.rt.lscp.rate), as.numeric(rmr$sema.rt.lscp.rate),
    as.numeric(rmr$wma.rt.lscp.rate),  as.numeric(rmr$me.rt.lscp.rate),
    as.numeric(rmr$nh.rt.lscp.rate),   as.numeric(rmr$nh.rt.lscp.rate),
    as.numeric(rmr$ct.rt.lscp.rate),   as.numeric(rmr$vt.rt.lscp.rate),
                 
    as.numeric(rmr$nema.da.lscp.rate), as.numeric(rmr$sema.da.lscp.rate),
    as.numeric(rmr$wma.da.lscp.rate),  as.numeric(rmr$me.da.lscp.rate),
    as.numeric(rmr$nh.da.lscp.rate),   as.numeric(rmr$ri.da.lscp.rate),
    as.numeric(rmr$ct.da.lscp.rate),   as.numeric(rmr$vt.da.lscp.rate))

  if (month == currentMonth() ) {
    rLog("Adjust intra-month quantity for actual number of days.")
    scale <- as.numeric(Sys.Date()-month)/as.numeric(nextMonth()-month+1)
    qq1$value <- qq1$value * scale
    qq2 <- qq1
  } else {
    # get the actual loads
    aux <- .monthly_actuals_load(month, bucket="7X24")
    aux$loc <- aux$location
    aux$loc[which(aux$loc=="ME")] <- "MAINE"
    aux$loc[which(aux$loc=="WMA")] <- "WCMA"
    
    qq2 <- rbind(
      cbind(aux[,c("company", "month", "value")], location=paste(aux$loc, "DA")),
      cbind(aux[,c("company", "month", "value")], location=paste(aux$loc, "RT")))
  }
  
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  aux[[2]] <- cbind(serviceType="RMR", aux[[2]])

  
  aux
}




################################################################
# 
.calculate_actuals <- function( month )
{
  fwdres  <- .actuals_fwdres( month )
  opres   <- .actuals_opres( month )
  rmr     <- .actuals_rmr( month )
  lossrmb <- .actuals_lossrmb( month )
  
  res <- rbind(fwdres$PnL,
               opres$PnL,
               rmr$PnL,
               lossrmb$PnL)
  res <- cbind(asOfDate=Sys.Date(), month=month, res)
  res$year <- NULL

  res
}


################################################################
# get the actuals for a month, sum over all the hours
#
.monthly_actuals_load <- function( month, bucket="7X24" )
{  
  startDt <- as.POSIXct(paste(format(month), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(nextMonth(month)), "00:00:00"))

  ccg <- get_hist_ccg_loads_RTLOCSUM(startDt, endDt)
  cne <- get_hist_cne_loads_RTLOCSUM(startDt, endDt)
  
  # for the fwd reserves 
  if (bucket == "5X16") {
    ind <- secdb.getHoursInBucket("NEPOOL", "5x16", startDt, endDt)
    ccg <- window(zoo(ccg, as.POSIXct(rownames(ccg))), index=ind$time)
    cne <- window(zoo(cne, as.POSIXct(rownames(cne))), index=ind$time)
  }
  
  res <- rbind(data.frame(location=toupper(colnames(ccg)),
    company="ccg.load", month=month, value=apply(ccg, 2, sum)),
        data.frame(location=toupper(colnames(cne)),
    company="cne.load", month=month, value=apply(cne, 2, sum)))

  rownames(res) <- NULL

  res
}


################################################################
# for the weekly email we send to FPA
#
.report_actuals <- function( res, do.email=TRUE )
{

  DIR <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/", 
    "Monthly Ancillary Updates/Actuals/Procmon/", sep="")
  filename <- paste(DIR, "ne_ancillary_actuals_", format(res$month[1]),
    "_", format(Sys.Date()), ".xlsx", sep="")
  
  write.xlsx2(res, filename, row.names=FALSE)
  rLog("Wrote", filename)

  # make the summary 
  out <- cast(res, serviceType ~ company,
    function(x){round(sum(x))}, fill=0, value="PnL")
  tot <- cast(res, . ~ company, function(x){round(sum(x))}, fill=0, value="PnL")
  colnames(tot)[1] <- "serviceType"
  out2 <- rbind(out, tot)
  
  note <- ""
  if (res$month[1] == currentMonth())
    note <- "\n*Current month quantities capture partial intra-month flowed MWh only."

  
  TT <- c(
    paste("Nepool ancillaries PnL for month:", format(res$month[1])),
    paste("calculated as of:", format(Sys.Date())), "",
#          "* Oil RFP cost has been taken out of OpRes RT marks for this report only.", "",
    capture.output(print(out2, row.names=FALSE)),
    note,
    paste("\n\nSee details at", filename)  
  )

  if (do.email) {
    rLog("Send email")
    to <- paste("david.riker@constellation.com", 
      "william.ewing@constellation.com", "NEcash@constellation.com",
      "abizar.shahpurwala@constellation.com",
      sep=",")
    #to <- "adrian.dragulescu@constellation.com"
    subject <- "Nepool ancillaries actuals updated"
    TT2 <- c(TT,
      "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/nepool_ancillaries_actuals",
      "Contact: Adrian Dragulescu\n")
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to, 
      subject, TT2)   
  }

}

################################################################
# Update the spreadsheets we use to keep track of realized ancillaries
#
.update_spreadsheets <- function( months )
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.reserves.R")
  aux <- .update_xlsx_reserves( months ) 
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  aux <- .update_xlsx_ncpc( months )
   
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.rmr.R")
  aux <- .update_xlsx_rmr( months )
  
  # in NCPC Tracker extend columns AJ:BD, E:N
  # go to Nepool/Ancillary Services/Monthly Ancillary Updates/Ancillary Mark Updates.xls and copy the columns F:I, J:O, P:R  
}


################################################################
################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(PM)
  require(xlsx)
  
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.actuals.ancillaries.R")


  month <- as.Date("2014-09-01")
  #month <- currentMonth()
  res <- .calculate_actuals( month )
  .report_actuals( res )
  

  # update the actuals spreadsheet!
  # always redo the last calculated month
  months <- seq(as.Date("2014-04-01"), as.Date("2014-08-01"), by="1 month")
  .update_spreadsheets( months )


  .actuals_rmr( month )
  .actuals_lossrmb( month )
  .actuals_fwdres( month )
  
}
