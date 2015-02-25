# Look at load resettlemements for NEPOOL
# 
# archive_sd_rtload
# compare_resettlements
#
#

##########################################################################
# Make and update an archive with loads/resettlement loads for cne and
# exgen.  
# @param months a vector of bom Dates
# 
archive_sd_rtload <- function( months )
{
  reportName <- "SD_RTLOAD"; tab <- 1
  
  res <- lapply(months, function(m) {
    rLog("Working on month ", format(m))
    fnames <- .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName)
    company <- paste("EXGEN_", format(m, "%Y%m"), "_", sep="")
    AA <- .archive_report(fnames, tab=tab, save=TRUE, company=company)    
  })
  
  res <- lapply(months, function(m) {
    rLog("Working on month ", format(m))
    fnames <- .get_report_csvfilenames(org="CNE", month=m, reportName=reportName)
    company <- paste("CNE_", format(m, "%Y%m"), "_", sep="")
    AA <- .archive_report(fnames, tab=tab, save=TRUE, company=company)    
  })
  
}

##########################################################################
# Compare resettlements data for one month
# @param month a bom Dates
# 
compare_resettlements <- function( month=as.Date("2013-11-01"), company="EXGEN" )
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/MIS/"

  yyyymm <- format(month, "%Y%m")
  fname <- paste(DIR, company, "_", yyyymm, "__SD_RTLOAD_tab1.RData", sep="")
  load(fname)

  #unique(AA[,c("Asset.Name", "Asset.ID")])
  # ddply(AA, "Asset.ID", function(x){max(-x$Share.of.Load.Reading)})
  namesDT <- unique(AA[,c("Asset.Name", "Asset.ID")])
  
  # add labels to the resetlement date
  availableLabels <- c("1st", "2nd", "3rd", "4th", "5th")
  aux <- unique(AA[,c("reportDay", "reportTimestamp")])
  aux <- aux[order(aux$reportDay, aux$reportTimestamp),]
  label <- ddply(aux, "reportDay", function(x) {
    x$label <- availableLabels[1:nrow(x)]
    x
  })
  AA <- merge(AA, label)

  # construct the datetime hour
  aux <- unique(AA[,c("reportDay", "Trading.interval")])
  colnames(aux) <- c("Date", "Hour.Ending")
  aux <- PM:::isotimeToPOSIXct(aux, extraHourToken="02X")
  aux <- aux[order(aux$datetime),]
  colnames(aux)[1:2] <- c("reportDay", "Trading.interval")
  
  AA <- merge(AA, aux)
  AA <- AA[order(AA$Asset.ID, AA$datetime), ]

  ## asset.id <- 37763
  ## xyplot(-Share.of.Load.Reading ~ datetime, data=AA, groups=AA$label,
  ##        type=c("g", "l"),
  ##        subset=AA$Asset.ID==asset.id,
  ##        xlab="",
  ##        ylab=paste(subset(namesDT, Asset.ID==asset.id)$Asset.Name),
  ##        auto.key=list(space="top", points=FALSE, lines=TRUE,
  ##          columns=length(unique(AA$label))))

  
  # make a time series with different resettlement values
  # because ISO will usually publish a 3rd settlement only for a few days only
  res <- ddply(AA, c("Asset.Name"), function(x) {
    #browser()
    aux <- cast(x, reportDay + Trading.interval ~ label, I, fill=NA,
                value="Share.of.Load.Reading")
    
    # fill up with the previous resettlement
    if ("2nd" %in% colnames(aux)) {
      ind <- is.na(aux[,"2nd"])
      aux[ind, "2nd"] <- aux[ind, "1st"]
    }
    if ("3rd" %in% colnames(aux)) {
      ind <- is.na(aux[,"3rd"])
      aux[ind, "3rd"] <- aux[ind, "2nd"]
    }
    if ("4th" %in% colnames(aux)) {
      ind <- is.na(aux[,"4th"])
      aux[ind, "4th"] <- aux[ind, "3rd"]
    }
    if ("5th" %in% colnames(aux)) {
      ind <- is.na(aux[,"5th"])
      aux[ind, "5th"] <- aux[ind, "4th"]
    }

    
    melt(aux, id=1:2)
  })



  res
##                Asset.Name  reportDay Trading.interval   value label
## 1 CCG_CMP_SOS_33%_3_YEARS 2014-07-01                1 -99.943   1st
## 2 CCG_CMP_SOS_33%_3_YEARS 2014-07-01                2 -93.000   1st
## 3 CCG_CMP_SOS_33%_3_YEARS 2014-07-01                3 -88.503   1st
## 4 CCG_CMP_SOS_33%_3_YEARS 2014-07-01                4 -85.785   1st
## 5 CCG_CMP_SOS_33%_3_YEARS 2014-07-01                5 -87.286   1st
## 6 CCG_CMP_SOS_33%_3_YEARS 2014-07-01                6 -92.805   1st  
}



##########################################################################
#
report_month_totals <- function( month )
{
  rLog("For month", format(month))
  company <- c("EXGEN", "CNE")
  res <- lapply(company, function(cmp) {
    rLog("Working on", cmp)
    aux <- compare_resettlements(month=month, company=cmp)

    # total MWh for the month
    totals <- cast(aux, Asset.Name + label ~ ., sum)
    colnames(totals)[ncol(totals)] <- "MWh"
    totals$company <- cmp
    totals$month <- month

    totals
  })
  
  totals <- do.call(rbind, res)

  DIR <- "S:/All/Structured Risk/NEPOOL/Load Related/ISO_Resettlement/"
  fname <- paste(DIR, "resettlements_", month, ".csv", sep="")
  write.csv(totals, file=fname, row.names=FALSE)
}


##########################################################################
#
.summary_12months <- function()
{
  DIR <- "S:/All/Structured Risk/NEPOOL/Load Related/ISO_Resettlement/"
  month <- currentMonth()

  files <- list.files(DIR, full.names=TRUE)
  aux <- lapply(files, function(fname){
    read.csv(fname)
  })
  aux <- do.call(rbind, aux)

  res <- cast(aux, company + month + label ~ ., sum, value="MWh",
              fill=NA)
  colnames(res)[ncol(res)] <- "total"

  xyplot(-total ~ as.Date(month)|company, data=res,
         type=c("g", "p"),
         groups=label,
         xlab="",
         ylab="MWh",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=length(unique(res$label)))
)

  
  
  
  
}


##########################################################################
##########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(plyr)
  require(reshape)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.load.resettlement.R")

  
  months <- seq(as.Date("2014-06-01"), as.Date("2014-12-01"), by="1 month")
  archive_sd_rtload( months )


  months <- seq(as.Date("2014-01-01"), as.Date("2014-12-01"), by="1 month")
  lapply(months, function(month){
    report_month_totals( month )
  })


  ## month <- as.Date("2014-07-01")
  ## company <- "EXGEN"

  
}
