# Functions related to capacity charges for loads
#
# .archive_FCMNRCPSUM
# .capacity_reconcile_actuals
# .get_nrcp
# get_ucap_load_positions_prism
#
#


################################################################
# Archive the FCMNRCPSUM report to an RData file
#
.archive_FCMNRCPSUM <- function( month )
{
  company <- "CCG"
  fnames  <- .get_report_csvfilenames( org=company,
    month=as.Date(month), reportName="SR_FCMNRCPSUM" )
}



################################################################
# 
#
.capacity_reconcile_actuals <- function( month )
{
  rLog("Working on month", format(month))
  asOfDate <- secdb.dateRuleApply(asOfDate, "-1b")

  reportName <- "SD_FCMCLOSTLDTL"; tab <- 3
  months <- month
  fnames <- as.vector(unlist(sapply(months, function(m){
    c(.get_report_csvfilenames(org="CNE", month=m, reportName=reportName),
      .get_report_csvfilenames(org="CCG", month=m, reportName=reportName),
      .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName))
  })))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  
  
  
  
  qq <- get_ucap_load_positions_prism(asOfDate, month)
  PP <- .pull_marks_capacity(asOfDate, service="UCAP")

  

  
}



################################################################
# Get the Net Regional Clearing Price
# (and the Peak Contribution, Peak Requirement) 
#
.get_nrcp <- function( month,
   company=c("CCG", "CNE", "EXELON") )
{
  rLog("Pulling  obligation for", company, format(month))
  fnames <- .get_report_csvfilenames( org=company,
    month=as.Date(month), reportName="SD_FCMCLOSTLDTL" )
  fnames <- fnames[order(fnames)]
  fname <- fnames[ length(fnames) ]   # get the last report
  if (length(fname) == 0){
    NULL
  } else {  
    aux <- .process_report( fname )  
    res <- cbind(company=company, aux[[3]])  # Customer Detail
    res$month <- month

    res[,c("company", "month", "Capacity.Zone.Name", "Net.Regional.Clearing.Price",
           "Customer.Peak.Contributions", "Customer.Capacity.Requirement")]
  }
}


################################################################
# matched with what I see in secdb for a given deal 
#
get_ucap_load_positions_prism <- function( asOfDate, month=NULL )
{
  books <- PM:::secdb.getBooksFromPortfolio("NEPOOL Load Portfolio")
  qq1 <- cbind(company="EXGN", get.positions.from.blackbird( asOfDate=asOfDate, 
    fix=FALSE, service=c("UCAP"), books=books ))
  
  books <- PM:::secdb.getBooksFromPortfolio("CNE NewEngland Portfolio")
  qq2 <- cbind(company="CNE", get.positions.from.blackbird( asOfDate=asOfDate, 
    fix=FALSE, service=c("UCAP"), books=books ))

  qq <- rbind(qq1, qq2)
  
  if (!is.null(month))
    qq <- subset(qq, contract.dt==month)

  qq <- qq[,c("company", "trading.book", "eti", "load.name", "service", "contract.dt",
    "notional")]
  qq <- subset(qq, contract.dt >= nextMonth(asOfDate))
  qq <- cast(qq, company + trading.book + eti + load.name + service +
    contract.dt ~ ., sum, fill=0, value="notional")
  colnames(qq)[ncol(qq)] <- "value"

  qq <- subset(qq, value < 0)
  
  qq
}



################################################################
################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(reshape)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.auctions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.PnL.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.loads.R")


  ####################################################################
  #
  asOfDate <- as.Date("2012-12-31")
  month <- nextMonth(asOfDate)
  .capacity_reconcile_actuals( month )
  
  qq <- get_ucap_load_positions_prism(asOfDate, month)
  PP <- .pull_marks_capacity(asOfDate, service="UCAP")

  # look at historical NRCP 
  reportName <- "SD_FCMCLOSTLDTL"; tab <- 3
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  xyplot(Net.Regional.Clearing.Price ~ reportDay|Capacity.Zone.Name,
    data=AA, type=c("g","p"), 
    layout=c(1,2),
    subset=AA$accountNumber=="000050017")
  

  # look at historical Pool CSO 
  reportName <- "SD_FCMCLOSTLDTL"; tab <- 1
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  subset(AA, accountNumber=="000050017")
  xyplot(Pool.Capacity.Supply.Obligation ~ reportDay,
    data=AA, type=c("g","p"), 
    subset=AA$accountNumber=="000050017")

  # look at FCA payment 
  reportName <- "SR_FCMNRCPSUM"; tab <- 2
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  subset(AA, accountNumber=="000050017")
  xyplot(Pool.Capacity.Supply.Obligation ~ reportDay,
    data=AA, type=c("g","p"), 
    subset=AA$accountNumber=="000050017")


  

  ####################################################################
  # archive FCMNRCPSUM report
  reportName <- "SR_FCMNRCPSUM"
  months <- seq(as.Date("2012-06-01"), by="1 month", length.out=24)
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="CCG", month=m, reportName=reportName)
  }))
  
  .archive_report(fnames, tab=2)

  

  
  capacityAuction <- CapacityAuction("2014-2015", "FCA")


  month <- as.Date("2011-07-01")
  
  

}



  ## ####################################################################
  ## # get NRCP by month
  ## company <- "CNE"  # nrcp is the same for both cne, ccg
  ## months  <- format(seq(as.Date("2012-06-01"), by="1 month", length.out=24))
  ## res <- lapply(months, function(month){
  ##   aux <- try(.get_nrcp(month, company))
  ##   if (class(aux)=="try-error")
  ##     NULL
  ##   else
  ##     aux
  ## })
  ## aux <- do.call(rbind, res)
  ## aux$Reserve.Margin <- aux$Customer.Capacity.Requirement/aux$Customer.Peak.Contribution
  ## print(aux, row.names=FALSE)
  
