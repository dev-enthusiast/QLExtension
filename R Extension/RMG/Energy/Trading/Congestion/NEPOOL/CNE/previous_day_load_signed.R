# Show details on previous day load signed by CNE
#
# Query is from Abizar
# 1. This query will work for ISONE only after the pdf report comes out! 
# 2. It will give you 3 Record Types: New, Expired and Modified (as
#    opposed to 2 in the PDF File)
# 3. It is monthly
# 4. Ignores current Month
# 5. Will give you data at the Contract and Account Level for all hedge types
# 6. Should take between 30secs to 1min to run.
# 7. Current day only!
#
# Author: Adrian Dragulescu on 2010-11-17
#


#############################################################################
#
.pull_data <- function(asOfDate)
{
  query <- "
select a.*, b.company_name, b.account_name
from
(select 
valuation_date,
record_type,
controlarea,
zone,
hedge_type,
mastercontract_our_num,
servacct_our_num,
year,
month,
sum(nvl(total_kwh_day1,0))/1000 QTY_TOTAL, 
SUM(NVL(onpeak_kwh_day1,0))/1000 QTY_ONPEAK,
SUM(NVL(oFFpeak_kwh_day1,0))/1000 QTY_OFFPEAK
from cer_rpt.mvw_delta_new_deals
WHERE CONTROLAREA = 'ISONE'
and to_date(extract (month from valuation_date)||'/01/'||extract (year from valuation_date), 'mm/dd/yyyy')
<> to_date(month||'/01/'||year, 'mm/dd/yyyy')
GROUP BY
valuation_date,
record_type,
controlarea,
zone,
hedge_type,
mastercontract_our_num,
servacct_our_num,
year,
month

UNION ALL

select 
valuation_date,
record_type,
controlarea,
zone,
hedge_type,
mastercontract_our_num,
servacct_our_num,
year,
month,
sum(nvl(total_kwh_day2,0))/1000 *-1 QTY_TOTAL, 
SUM(NVL(onpeak_kwh_day2,0))/1000*-1 QTY_ONPEAK,
SUM(NVL(oFFpeak_kwh_day2,0))/1000*-1 QTY_OFFPEAK
from cer_rpt.mvw_delta_expired_deals
WHERE CONTROLAREA = 'ISONE'
and to_date(extract (month from valuation_date)||'/01/'||extract (year from valuation_date), 'mm/dd/yyyy')
<> to_date(month||'/01/'||year, 'mm/dd/yyyy')
GROUP BY
valuation_date,
record_type,
controlarea,
zone,
hedge_type,
mastercontract_our_num,
servacct_our_num,
year,
month


UNION ALL

select 
valuation_date,
record_type,
controlarea,
zone,
hedge_type,
mastercontract_our_num,
servacct_our_num,
year,
month,
sum(nvl(total_kwh,0))/1000  QTY_TOTAL, 
SUM(NVL(onpeak_kwh,0))/1000 QTY_ONPEAK,
SUM(NVL(oFFpeak_kwh,0))/1000 QTY_OFFPEAK
from cer_rpt.mvw_delta_modified_deals
WHERE CONTROLAREA = 'ISONE'
and to_date(extract (month from valuation_date)||'/01/'||extract (year from valuation_date), 'mm/dd/yyyy')
<> to_date(month||'/01/'||year, 'mm/dd/yyyy')
GROUP BY
valuation_date,
record_type,
controlarea,
zone,
hedge_type,
mastercontract_our_num,
servacct_our_num,
year,
month) A,

(select mastercontract_our_num, servacct_our_num, company_name, account_name 
from cer.mv_crm
where TRUNC(est_power_flow_date) <> TRUNC(est_power_END_date)
group by mastercontract_our_num, servacct_our_num, company_name, account_name) B

where a.mastercontract_our_num = b.mastercontract_our_num
and a.servacct_our_num = b.servacct_our_num
"
  query <- gsub("\n", " ", query)
  
  rLog("Pulling CNE load data from CERDW as of", format(asOfDate))

  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/CERINP.dsn"),
    ";UID=cer_pmo_ro;PWD=cerpmoro;", sep="")  


  con <- odbcDriverConnect(con.string)
  data <- sqlQuery(con, query)
  #data   <- sqlFetch(con, "CER.capacity_load_obligation", max=4)
  odbcCloseAll()

  data
}


#############################################################################
# 
save_data <- function(data)
{
  res <- cast(data, VALUATION_DATE + HEDGE_TYPE + RECORD_TYPE + YEAR + MONTH ~ ., sum,
       value="QTY_TOTAL") #, subset=RECORD_TYPE %in% c("NEW DEALS", "VARIANCE"))
  colnames(res)[ncol(res)] <- "MWH"

  DIR <- "S:/All/Structured Risk/NEPOOL/ConstellationNewEnergy/Daily_Load_Signed/"
  fname <- paste(DIR, "cne_new_load_", format(max(res$VALUATION_DATE[1])), ".RData", sep="")
  save(res, file=fname)

  0
}


#############################################################################
# by zone, by year
view_new_accounts <- function(data)
{
  res <- cast(data, ZONE + COMPANY_NAME + HEDGE_TYPE + RECORD_TYPE ~ YEAR,
    sum, fill=0, value="QTY_TOTAL",
    subset=RECORD_TYPE %in% c("NEW DEALS", "VARIANCE"))
  # filter small deals ...
  # NEW load shows up as a positive number in this report!
  ind <- which(apply(res[,-(1:4)], 1, sum) > 1000)  
  if (length(ind)>0)
    res <- res[ind,]
  
  # round to nearest integer
  res[,5:ncol(res)] <- sapply(res[,5:ncol(res)], round)
  
  print(res, row.names=FALSE)
}

#############################################################################
#############################################################################
.main <- function()
{
  
  require(CEGbase)
  require(RODBC) 
  require(reshape)
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/previous_day_load_signed.R")
  
  asOfDate <- Sys.Date()-1
  data <- .pull_data(asOfDate)

  view_new_accounts(data)

}


