# Estimate, mark loss reimbursment in NEPOOL
#
# Do a monthly regression of LossRevenues to DA power prices
#
# Location of legacy spreadsheet:
# S:\All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates\LossRmb\Loss Rmb.xlsx
#
# Marks are pushed into:
#   Marks/Ancillaries/NEPOOL Zonal Ancillaries 04mar11LossRmb.xls
#
# 201311_estimate_lossrmb_enhancement
# calc_pnl_impact_lossrmb
# .get_fwd_data
# .get_hist_data
# .get_positions_lossrmb
# .monthly_actuals_lossrmb
# .pull_hist_lossrmb
# .pull_marks_lossrmb  
# .regress_daily 
# .regress_monthly
# .write_marks_xls
#


#######################################################################
# Estimate effect of Loss Rmb of ISO announced enhancements
# 
estimate_lossrmb_enhancement_201311 <- function()
{
  require(xlsx)

  # load the map
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

  
  DIR <- "S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/LossRmb/2013-11 ISO enhancement/"
  fname <- paste(DIR, "da_case_loss_enhancements.xlsx", sep="")
  data <- read.xlsx2(fname, sheetName="Sheet1", 
    colClasses=c(rep("character", 2), rep("numeric", 6)), startRow=2)
  data$hourEnding <- as.numeric(sapply(strsplit(data$MKTHOUR, " "), "[[", 2))
  colnames(data)[c(2,5,8)] <- c("name", "MCL.after", "MCL.before")

  # get the load
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  LL <- get_historical_zonal_load(start.dt=as.POSIXct("2013-10-19 01:00:00"),
                                  end.dt=as.POSIXct("2013-10-20 00:00:00"))
  LL <- merge(LL, MAP[,c("ptid", "name")])
  LL <- subset(LL, dart == "RT")
  LL$hourEnding <- as.numeric(format(LL$time, "%H")) + 1
  colnames(LL)[4] <- "load"


  # get the gen
  frujun <- paste(DIR, "10-19-13 Hourly Generation.xlsx", sep="")
  GG <-  read.xlsx2(frujun, sheetName="Hourly report", colIndex=12:14,
    colClasses=c(rep("numeric", 3)))
  colnames(GG) <- c("hourEnding", "mw", "ptid")
  GG2 <- merge(GG,  MAP[,c("ptid", "name")])
  GG2 <- merge(GG2, data[,c("name", "hourEnding", "MCL.after", "MCL.before")])
  GG2$dollars.before <- GG2$MCL.before * GG2$mw
  GG2$dollars.after  <- GG2$MCL.after  * GG2$mw 
  sum(GG2$dollars.before)  # paste this in the spreadsheet
  sum(GG2$dollars.after)


  res <- merge(data[,c("name", "hourEnding", "MCL.after", "MCL.before")],
               LL[,c("name", "hourEnding", "load")])
  res <- res[order(res$name, res$hourEnding),]
  res$dollars.before <- res$MCL.before * res$load
  res$dollars.after  <- res$MCL.after  * res$load
  print(res, row.names=FALSE)

  (sum(res$dollars.after) - sum(res$dollars.before))/sum(res$dollars.before)
  

  

  
  
}


#######################################################################
# calculate PnL
# 
calc_pnl_impact_lossrmb <- function(day1, day2)
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")

  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg", get_positions_lossrmb(day1, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_lossrmb(day1, books))
  qq1   <- rbind(qqA, qqB)
  qq1   <- subset(qq1, month >= nextMonth())

  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg", get_positions_lossrmb(day2, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_lossrmb(day2, books))
  qq2   <- rbind(qqA, qqB)
  qq2   <- subset(qq2, month >= nextMonth())

  PP1 <- .pull_marks_lossrmb(day1)
  PP1$asOfDate <- NULL
  
  PP2 <- .pull_marks_lossrmb(day2)
  PP2$asOfDate <- NULL

  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2, groups=c("company", "year"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  
  rLog("Total PnL change is", sum(gPnL$PnL))
   
  aux
}





#######################################################################
#
# @param asOfDate
# @param pfolio the name of the SecDb portfolio
#
#
get_positions_lossrmb <- function(asOfDate, books, month=NULL)
{
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    contract.dt=month, fix=FALSE, service="LOSSRMB")
  aux <- subset(aux, contract.dt >= nextMonth(asOfDate))
  aux <- subset(aux, mtm.usd != 0)  # attrition trades are booked like this, see PZSEEAZ
  
  qq <- aggregate(aux[,"notional"], list(month=aux[,"contract.dt"]),  sum)
  colnames(qq)[2] <- "value"

  qq <- cbind(serviceType="LOSSRMB", qq)
  
  qq
}



#######################################################################
# create the table you'll write to xlsx
#
.make_xlsx_output <- function(MM, PnL, reg)
{
  TT <- MM[,c("hub.da", "energy", "loss.rmb")]
  TT <- cbind(month=as.Date(index(TT)), data.frame(TT))
  rownames(TT) <- NULL
  TT$actual.lossRmb <- TT$loss.rmb/TT$energy

  # add the backcast 
  aux <- data.frame(month=as.Date(names(reg$fitted.value)), fitted.values=reg$fitted.values)
  aux <- merge(aux, TT[, c("month", "energy")])
  aux$forecasted.lossRmb <- aux$fitted.values/aux$energy 
  TT <- merge(TT, aux[,c("month", "forecasted.lossRmb")], all.x=TRUE)
  TT$`position$` <- NA
  TT$PnL <- NA

  # add the forecast
  aux <- PnL[, c("month",  "hub.da", "energy", "loss.revenue", "$/MWh", "position$", "PnL")]
  colnames(aux)[4:5] <- c("loss.rmb", "forecasted.lossRmb")
  aux$actual.lossRmb <- NA
  aux <- aux[, colnames(TT)]
  TT <- rbind(TT, aux)

  TT
}



#######################################################################
# Calculate monthly actuals for loss rmb
# @return a monthly zoo object
#
.monthly_actuals_lossrmb <- function( month )
{
  startTime <- as.POSIXct(format(month, "%Y-%m-01 01:00:00"))
  endTime   <- nextMonth(month) - 1
  rLog("  working on month", format(month))

  HH <- .pull_hist_lossrmb(startTime, endTime)
  
  MM <- aggregate(HH, as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)
  MM$`ccg.rt.adjload`   <- -MM$`ccg.rt.adjload`
  MM$`cne.rt.adjload`   <- -MM$`cne.rt.adjload`
  MM$ccg.lossrev.rate <- MM$`ccg.lossrev$`/MM$ccg.rt.adjload
  MM$cne.lossrev.rate <- MM$`cne.lossrev$`/MM$cne.rt.adjload
  
  MM
}



#######################################################################
# Pull hist loss reimbursment data.
# from SR_RTCUSTSUM 
# ccg.lossrev$ = (rt.ccg.load/rt.pool.load)*(pool.lossrev.rt + pool.lossrev.da)
#
.pull_hist_lossrmb <- function(startTime=NULL, endTime=Sys.time())
{
  if (is.null(startTime))
    startTime <- as.POSIXct(paste(as.numeric(format(Sys.Date(), "%Y"))-3, 
       "-01-01 01:00:00", sep=""))
  
  symbols <- c(
   "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
   "NEPOOL_SMD_RT_4000_lmp",         "hub.rt",            
   "NeRpt_rtCus_PoolLoadObl",        "load",               # pool load
   "NeRpt_rtCus_PoolMargLossRev",    "pool.lossrev.rt",    # pool RT marg loss revenue
   "NeRpt_rtCus_DAPoolMargLossRev",  "pool.lossrev.da",    # pool DA marg loss revenue
   "NERpt_RTCuS_MargLossRevAlloc",   "ccg.lossrev$",       # ccg marg loss revenue
   "NCRpt_RTCuS_MargLossRevAlloc",   "cne.lossrev$",       # cne marg loss revenue
   "NERpt_RTCuS_AdjLoadObl",         "ccg.rt.adjload",     # ccg adj load oblg
   "NCRpt_RTCuS_AdjLoadObl",         "cne.rt.adjload"      # ccg adj load oblg
               
 #  "NeRpt_rtCus_PoolEnergyStl",      "pool.engy.stl.rt",
 #  "NeRpt_rtCus_PoolLossRev",        "pool.loss.rev.rt",
 #  "NeRpt_rtCus_PoolExtInadCost",    "pool.ext.inad.cost",
 #  "NeRpt_rtCus_PoolEmerCost",       "pool.emer.cost.rt",               
 #  "NeRpt_rtCus_PoolEnergyStl",      "pool.ener.stl.da"
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startTime, endTime)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)

  ## HH <- cbind(time=index(HH), as.data.frame(HH))
  ## rownames(HH) <- NULL

  # discard NA obs
  HH <- HH[!is.na(HH$`cne.lossrev$`),]
  
  HH
}



#######################################################################
# read the forward price from SecDb, forward energy from xlsx
# 
.pull_marks_lossrmb <- function(asOfDate=Sys.Date(),
  startDate=nextMonth(), endDate=NULL)
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+7, 
       "-12-01", sep=""))

  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- "HUB"
  bucket      <- "FLAT"
  serviceType <- "ENERGY"
  useFutDates <- FALSE
  ## curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
  ##   commodity, location, bucket, serviceType, useFutDates)
  ## names(curveData)[ncol(curveData)] <- "hub.da"

  serviceType <- "LOSSRMB"
  curveData2 <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  names(curveData2)[ncol(curveData2)] <- "value"
  
  curveData2
}





#######################################################################
# HH - a zoo object
#
.regress_monthly <- function()
{
  HH <- .pull_hist_lossrmb()
  
  # calculate monthly average da, rt prices
  prices <- aggregate(HH[,c("hub.da", "hub.rt")], as.Date(format(index(HH)-1,
    "%Y-%m-01")), mean, na.rm=TRUE)
  aux <- aggregate(HH[,c("pool.lossrev.rt", "pool.lossrev.da", "load")],
    as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)
  names(aux)[3] <- "energy"
  aux$energy <- -aux$energy   
  MM <- merge(prices, aux)
  
  MM$loss.rmb <- MM$pool.lossrev.rt + MM$pool.lossrev.da
  MM$loss.rmb.rate <- MM$loss.rmb/MM$energy

  MM <- MM[-nrow(MM),]   # remove current month as data is not complete!

  reg <- lm(loss.rmb.rate ~ hub.da, data=MM)  #,  data=window(MM, start=fromDate))
  summary(reg)

  # plot actuals vs. predicted as a function of time
  y <- fitted(reg)
  windows(8,5)
  plot(index(MM), MM$loss.rmb.rate, col="blue", xlab="",
       ylab="Loss reimbursment rate, $/MWh")
  lines(as.Date(names(y)), as.numeric(y), type="l",
        col="red", lwd=2)


  plot(MM$hub.da, MM$loss.rmb.rate, col="blue",
       xlab="Hub DA 7x24, $/MWh",
       ylab="Loss reimbursment rate, $/MWh")
  xFit <- c(20,150)
  yFit <- predict(reg, data.frame(hub.da=xFit))
  lines(xFit, yFit, col="gray")
 

  list(reg, MM)
}



#######################################################################
# Recreate the sheet AAD in the spreadsheet
#
.update_base_xlsx <- function(TT)
{
  outdir <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
                  "Monthly Ancillary Updates/LossRmb/", sep="")
  fname <- paste(outdir, "Loss Rmb.xlsx", sep="")

  wb <- loadWorkbook(fname)
  sheets <- getSheets(wb)
  if ("AAD" %in% names(sheets)) {
    removeSheet(wb, sheetName="AAD")
  }
  sheet <- createSheet(wb, sheetName="AAD")
  rows  <- createRow(sheet, rowIndex=1:(nrow(TT)+1))
  cells <- createCell(rows, colIndex=1:ncol(TT))
  mapply(setCellValue, cells[1,(1:ncol(cells))], colnames(TT))

  dateFormat <- createCellStyle(wb, dataFormat="m/d/yyyy")
  ccyFormat2  <- createCellStyle(wb, dataFormat="$* #,##0.00")
  ccyFormat0  <- createCellStyle(wb, dataFormat="$* #,##0")
    
  for (i in 1:ncol(cells)){
    mapply(setCellValue, cells[(2:nrow(cells)), i], TT[,i])
    if (i==1){
      lapply(cells[(2:nrow(cells)), 1], setCellStyle, dateFormat)
    } else if (i %in% c(3,4,7,8)){
      lapply(cells[(2:nrow(cells)), i], setCellStyle, ccyFormat0)
    } else {
      lapply(cells[(2:nrow(cells)), i], setCellStyle, ccyFormat2)
    }
  }
  
  saveWorkbook(wb, fname)
  rLog("Wrote file", fname)

  
}


#######################################################################
# not used
#
.write_marks_xls <- function(PnL, outdir="C:/Temp/")
{
  fname <- paste(outdir, "NEPOOL Zonal Ancillaries ", format(Sys.Date(), "%d%b%y"),
                 "LossRmb.xls", sep="")

  wb <- createWorkbook(type="xls")
  sheet1 <- createSheet(wb, "Description")
  rows <- createRow(sheet1, rowIndex=1:3)
  cells <- createCell(rows, colIndex=1)
  setCellValue(cells[[1]], "NEPOOL Ancillary Marks for Updating to Secdb")
  setCellValue(cells[[2]], Sys.time()-5*3600)   # Excel is GMT
  dtFormat <- createCellStyle(wb, dataFormat="m/d/yyyy h:mm:ss;@")
  setCellStyle(cells[[2]], dtFormat)
  setCellValue(cells[[3]], "NEPOOL")
    
  sheet2 <- createSheet(wb, "LossRmb")
  rows   <- createRow(sheet2, rowIndex=1:(nrow(PnL)+1))
  cells  <- createCell(rows, colIndex=1:2)
  setCellValue(cells[[1,1]], "7x24")
  setCellValue(cells[[1,2]], "LossRmb")
  mapply(setCellValue, cells[(2:nrow(cells)), 1], PnL$month)
  dateFormat <- createCellStyle(wb, dataFormat="m/d/yyyy")
  lapply(cells[(2:nrow(cells)), 1], setCellStyle, dateFormat)
  mapply(setCellValue, cells[(2:nrow(cells)), 2], PnL$secdb.mark.new)
  ccyFormat <- createCellStyle(wb, dataFormat="$* #,##0.00")  
  lapply(cells[(2:nrow(cells)), 2], setCellStyle, ccyFormat)
  
  
  saveWorkbook(wb, fname)
  rLog("Wrote file", fname)

  TRUE
}


#######################################################################
#######################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.lossrmb.R")


  FF <- .pull_marks_lossrmb()
  print(FF[,c("month", "value")], row.names=FALSE)


  day1 <- as.Date("2013-11-04")
  day2 <- as.Date("2013-11-05")
  pnl  <- calc_pnl_impact_lossrmb(day1, day2)
  print(cast(pnl$gPnL, company ~ year, I, value="PnL", fill=0),
        row.names=FALSE)
  

  
  month <- currentMonth()
  LR <- .monthly_actuals_lossrmb( month )
  

  

  ####################################################################
  aux <- .regress_monthly()
  reg <- aux[[1]]  # the regression object
  MM  <- aux[[2]]  # monthly data
  summary(reg)

  
  # get the positions
  asOfDate  <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  qqA <- cbind(company="ccg.load",
         .get_positions_lossrmb_pfolio(asOfDate, NULL, "NEPOOL LOAD PORTFOLIO"))
  qqB <- cbind(company="cne.load",
         .get_positions_lossrmb_pfolio(asOfDate, NULL, "CNE NEWENGLAND PORTFOLIO"))
  qq  <- rbind(qqA, qqB)
  qq  <- cast(qq, month ~ company, I, fill=0)
  qq$total.mwh <- qq$ccg.load + qq$cne.load
  qq$implied.lossrmb.mwh <- qq$total.mwh * coef(reg)["hub.da"]
  print(qq, row.names=FALSE)
  
  
  fwd <- .get_fwd_data(asOfDate) 
  y <- predict(reg, fwd)            # forecast
  fwd <- cbind(fwd, loss.revenue=as.numeric(y))
  fwd$`$/MWh` <- fwd$loss.revenue/fwd$energy
  fwd$secdb.mark.new <- -fwd$phys.hedge.percent*fwd$`$/MWh`
  
  PnL <- .calculate_PnL(asOfDate, fwd, qq)

  TT <- .make_xlsx_output(MM, PnL, reg)
  update_base_xlsx(TT)

  #outdir <- "S:/All/Structured Risk/NEPOOL/Marks/Ancillaries/"
  outdir <- "C:/Temp/"
  success <- write_marks_xls(PnL, outdir=outdir)
  
  if (success)
    email_results(PnL)




  
}



















## #######################################################################
## # HH - a zoo object
## #
## # Does not make a big difference to use daily regression vs. monthly
## #
## .regress_daily <- function(HH, fromDate=as.Date("2009-01-01"))
## {
##   # calculate daily average da, rt prices
##   prices <- aggregate(HH[,c("hub.da", "hub.rt")], as.Date(format(index(HH)-1,
##     "%Y-%m-%d")), mean, na.rm=TRUE)
##   aux <- aggregate(HH[,c("lossrev.rt", "lossrev.da", "load")],
##     as.Date(format(index(HH)-1, "%Y-%m-%d")), sum, na.rm=TRUE)
##   DD <- merge(prices, aux)
  
##   DD$loss.rmb <- DD$lossrev.rt #+ DD$lossrev.da
  
##   DD <- window(DD, start=fromDate)
##   # pairs(DD)

##   plot(DD$hub.rt, DD$lossrev.rt)
  
##   plot(DD[,c("hub.rt", "lossrev.rt")], main="", col="blue")
  
##   reg <- lm(loss.rmb ~ hub.rt + load,  data=DD)
##   summary(reg)
##   # plot(reg)

##   # plot actuals vs. predicted as a function of time
##   y <- fitted(reg)
##   windows(8,5)
##   plot(index(DD), DD$loss.rmb/1000000, col="blue", xlab="",
##        ylab="Loss reimbursment, M$")
##   lines(as.Date(names(y)), as.numeric(y)/1000000, type="l",
##         col="gray", lwd=2)

##   # there is some autocorrelation in the residuals ...
##   res <- residuals(reg)
##   plot(index(DD), res, xlab="", ylab="Regression residual")
##   ablines(h=0, col="gray", lty=2)
  

##   reg
## }

  ## y.DD <- fitted(reg.daily)
  ## y.DD <- zoo(y.DD, as.Date(names(y.DD)))
  ## y.DD.agg <- aggregate(y.DD, as.Date(format(index(y.DD), "%Y-%m-01")), sum, na.rm=TRUE)
  ## lines(index(y.DD.agg), as.numeric(y.DD.agg)/1000000, type="l",
  ##       col="red", lwd=2)
 
  ## aux <- data.frame(year=format(index(MM), "%Y"),
  ##                   mon =format(index(MM), "%m"),
  ##                   loss.rmb=as.numeric(MM$loss.rmb))
  ## print(cast(aux, mon ~ year, I, value="loss.rmb", fill=NA), row.names=FALSE)


  ## # read the fwd energy.  Should read it from TSDB!
  ## outdir <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
  ##                 "Monthly Ancillary Updates/LossRmb/", sep="")
  ## fname <- paste(outdir, "Loss Rmb.xlsx", sep="")
  ## wb <- loadWorkbook(fname)
  ## sheet <- getSheets(wb)[["Forward"]]
  ## energy <- getMatrixValues(sheet, 3:100, 3)
  ## month  <- getMatrixValues(sheet, 3:100, 1)
  ## month  <- as.Date(month - 25569, origin="1970-01-01")
  ## phys.hedge <- getMatrixValues(sheet, 3:100, 8)
    
  ## aux <- data.frame(month=month[,1], energy=as.numeric(energy),
  ##                   phys.hedge.percent=as.numeric(phys.hedge))
  ## aux <- data.frame(na.exclude(aux))

  #fwd <- merge(aux, curveData[,c("month", "hub.da")], by="month", all.y=TRUE)
  #fwd <- merge(fwd, curveData2[,c("month", "secdb.mark")], by="month", all.y=TRUE)
 
