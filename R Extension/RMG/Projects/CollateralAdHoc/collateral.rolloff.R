# Some report for collateral rolloff
#
# Last modified by: Adrian Dragulescu
# 
# Tested with 2.7.1 

require(RODBC)

###########################################################################
# get the MegaReport data
#
getMegaData <- function( asOfDate )
{
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
    "DSN/RAFT.dsn;UID=ExecSpOnly;PWD=EnergyCredit_ExecSPOnly;", sep="")  
  con <- odbcDriverConnect(con.string)

  query = paste(
    "select * from ExposureByNA_Collateral_CSA_NC ",
    "where ", "DataAsOfDate='", format( asOfDate, "%d%b%y" ), "' ", sep="") 
    
  data = sqlQuery( con, query )
  odbcClose( con )
    
  return( data )
}


###########################################################################
# MAIN
###########################################################################
asOfDate <- as.Date("2008-11-05")
MR <- getMegaData( asOfDate )
names(MR) <- gsub(" ", ".", tolower(names(MR)))

#subset(MR, shortname=="CORAL ENE")

#write.csv(MR, file="C:/Temp/junk.csv", row.names=FALSE)
#MR <- read.csv(file="C:/Temp/junk.csv")

MR <- MR[,c("shortname", "netting.agreement", "mtm", "mtmplusaccrualplusarap",
  "cash.held", "cash.posted", "lc.held", "lc.posted",
  "ceg.threshold.amt.used.(usd)", "cpty.threshold.amt.used.(usd)")]
MR <- MR[!is.na(MR$mtm),]

MR[,1:2] <- sapply(MR[,1:2], as.character)
MR[,3:8] <- sapply(MR[,3:8],
   function(x){x[is.na(x)] <- 0; return(x)})

MR$collateral.posted <- MR$cash.posted + MR$lc.posted




###########################################################################
# load the A' data for this asOfDate ... 
rootdir  <- paste("S:/Risk/Temporary/CollateralAllocation/",
  format(asOfDate, "%Y%m%d"), "/SourceData/", sep="")
filename <- paste("AllPos_AGMTHScale_preibt_", format(asOfDate, "%d%b%y"),
  ".csv", sep="")

AA <- read.csv(paste(rootdir, filename, sep=""), nrows=-1)
names(AA) <- gsub("_",".", tolower(names(AA)))

# remove some columns that are not necessary or redundant
ind.rm <- which(names(AA) %in% c("contractyear", "contractquarter",
  "price", "tenor", "policy.region", "policy.subregion", "sttype",
  "money.pos", "net", "net1", "calc.th", "calc.coll",
  "commodity.grp", "reporting.unit", "conversion.divisor",
  "pos.adj", "bookmap", "business.line", "business.line2",
  "parentportfolio1", "parentportfolio2", "parentportfolio3",
  "parentportfolio4", "parentportfolio5", "parentportfolio6",
  "parentportfolio7", "parentportfolio8", "organization",
  "office", "economic.owner", "portfolio.mgr", "executive.owner",
  "pm.region", "upstream.region", "cer.group", "pm.gen.load"))
AA <- AA[,-ind.rm]
save(AA, file="c:/temp/junk.RData")


AA <- AA[, c("contractmonth", "counterparty",
  "credit.nettingagreement", "exposure")]
AA$contractmonth <- as.Date(AA$contractmonth, "%d%B%y")
AA <- subset(AA, contractmonth >= asOfDate)


# Aggregate exposure by contractmonth, counterparty, credit.nettingagreement
names(AA)[4] <- "value"
AA.agg <- cast(AA, ... ~ ., sum, na.rm=TRUE)
names(AA.agg)[4] <- "value"

# bucket yearly from 2010 onwards ... 
AA.agg$timeBucket <- AA.agg$contractmonth
ind <- which(AA.agg$timeBucket >= as.Date("2010-01-01"))
AA.agg$timeBucket[ind] <- as.Date(format(AA.agg$timeBucket[ind], "%Y-12-01"))

AA.agg <- cast(AA.agg,
  counterparty + credit.nettingagreement + timeBucket ~ ., sum, na.rm=TRUE)
names(AA.agg)[4] <- "value"
AA.agg <- subset(AA.agg, value != 0)

AA.agg <- data.frame(AA.agg)
names(AA.agg)[c(1:2,4)] <- c("shortname", "netting.agreement", "exposure")

# get only the counterparties in the MegaReport
AA.agg <- subset(AA.agg, shortname %in% MR$shortname)
AA.agg <- AA.agg[order(AA.agg$shortname, AA.agg$netting.agreement,
                       AA.agg$timeBucket), ]
rownames(AA.agg) <- NULL

grp <- data.frame(unique(AA.agg[,1:2]))
grp$grp.no <- 1:nrow(grp)
AA.agg <- merge(AA.agg, grp)

# calculate rolloff exposure
AA.agg$roll.exposure <- unlist(sapply(split(AA.agg$exposure, AA.agg$grp.no),  
  function(x){
    z <- rev(cumsum(rev(x)))
    return(z)
  }))

# put the MR and Aprime together
rolloff <- merge(MR[,c("shortname", "netting.agreement", "collateral.posted",
  "ceg.threshold.amt.used.(usd)")], AA.agg[, c("shortname", "netting.agreement",
  "timeBucket", "exposure", "roll.exposure")], all.x=TRUE)

rolloff <- rolloff[!is.na(rolloff$roll.exposure), ]

# not sure if I should do this ... 
rolloff[is.na(rolloff[, "ceg.threshold.amt.used.(usd)"]),
  "ceg.threshold.amt.used.(usd)"] <- 0  

# calculation logic:
# 
rolloff$postedPlusThresholdLessExposure <-
  ifelse(rolloff$roll.exposure < 0, rolloff$collateral.posted +
    rolloff$"ceg.threshold.amt.used.(usd)" - (-rolloff$roll.exposure),
    0)     

# aggregate over all counterparties/agreements
rolloff.agg <- aggregate(rolloff$postedPlusThresholdLessExposure,
  list(date=rolloff$timeBucket), sum, na.rm=TRUE)
names(rolloff.agg)[1:2] <- c("date", "postedCollateral")

rolloff.agg <- subset(rolloff.agg, date <= as.Date("2015-01-01"))
plot(rolloff.agg[,1], rolloff.agg[,2]/1000000, col="blue", type="o",
     lwd=2, ylab="Posted collateral less exposure, M$")


write.csv(rolloff, file="H:/user/R/RMG/Projects/CashManagement/rolloff.csv",
  row.names=FALSE)












## sort(unique(AA.agg$netting.agreement))
## sort(unique(MR$netting.agreement))


## subset(aux, shortname=="CORAL ENE")


## table(AA$ibt.flag)

## unique(AA$ibt.flag)

## #aux <- subset(AA, book.name=="GASVOL" & counterparty=="GVS")
## #aux <- subset(AA, book.name=="GVS" & counterparty=="GASVOL")

## aux <- subset(AA, counterparty=="ENECSFB")




## AA <- aggreagate(AA$exposure, list(AA$))

## AA <- AA[,c("counterparty", "credit.nettingaggreement",
##   "")]

## # from the mega report ... 
## filename <- paste("S:/All/Risk/Projects/PortfolioMargin/2008-10-15/",
##   "Exposure by Agreement Mega Rpt COB 2008-10-14 Dev Downstream Out.csv",
##   sep="")
## MR <- read.csv(filename)
