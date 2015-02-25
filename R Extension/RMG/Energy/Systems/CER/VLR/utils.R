#
#
#
#
source("H:/user/R/RMG/Utilities/RLogger.R")

#==================================================================
# Get the accounts under a profilecode and siccode
#
get.all.sic.and.profiles <- function(busunit_id, options){
    
  query <- paste("SELECT rc.PROFILECODE_ABBR, c.SIC_CODE, ", 
    "a.servacct_our_num FROM BN_LOAD.RMA_ACCT_UDC_RCLASS_T rc, ",
    "CER.ACCOUNT a, CER.COMPANY c WHERE ",  
    "c.busunit_id = 7 and ",
    "a.SERVACCT_OUR_NUM = rc.SERVACCT_OUR_NUM and ",
    "a.COMPANY_ID = c.COMPANY_ID and ",
    "rc.PROFILECODE_ABBR like 'BUSIDRRQ%'",  sep="") 
  res <- sqlQuery(options$con, query)
  names(res) <- c("weather", "sic.code", "ne.acctno")

  res <- na.omit(res)
  res$sic.code <- format(res$sic.code, width=4)
  res$sic.code <- gsub(" ", "0", res$sic.code)
        
  res[,1] <- as.character(res[,1])
  res[,1] <- sapply(res[,1], function(x){strsplit(x, "_")[[1]][2]})
  res$sic.code <- substr(res$sic.code, 1, 2)
   
  aux <- unique(res[,1:2])
  aux <- aux[order(aux[,1], aux[,2]), ]
  rownames(aux) <- NULL
    
  return(aux)  
}

#==================================================================
# By region ...
#
get.all.edc.and.rateclasses <- function(busunit_id, options){
    
    query <- paste("select distinct edc_name, edc_ratecls ", 
            "from bn_load.smis_account_t ",
            "where busunit_id  = '", busunit_id, "' ",                
            "and hourlyacct_yn = 'True'", sep="")
    res <- sqlQuery(options$con, query)
    
    names(res) <- gsub("_", ".", tolower(names(res)))
    res <- res[order(res$edc.name, res$edc.ratecls), ]
    rownames(res) <- NULL

    RR$edc.name <- as.character(RR$edc.name)
    RR$edc.ratecls <- as.character(RR$edc.ratecls)
    
    return(res)  
}

#==================================================================
# Get all the load for a given edc and ratecls.  Should fit in
# memory.
#
get.all.load <- function(busunit.id, edc.name, rateclass, options){
    
    rLog(paste("Grabbing hist pricing data for ", edc.name,
                    " rateclass ", rateclass, sep=""))
    
    if( edc.name=="NA" || is.na(edc.name) )
    {
        edcClause = " and edc_name IS NULL"
    
    }else
    {
        edcClause = paste(" and upper(edc_name) = '", toupper(edc.name), "'", sep="")        
    }
    
    if( rateclass=="NA" || is.na(rateclass) )
    {
        rateClause = " and edc_ratecls IS NULL"
    }else
    {
        rateClause = paste(" and upper(edc_ratecls) = '", toupper(rateclass), "'", sep="")        
    }
    
    query <- paste(
            "select ne_acctno, data_source, read_dt, hour, kw ", 
            "from bn_load.smis_hourly_usage_t where busunit_id =", busunit.id,
            "  and ne_acctno in ( ", 
            "    select ne_acctno",
            "    from bn_load.smis_account_t", 
            "    where upper(busunit_id) = ", toupper(busunit.id),   
            edcClause,  
            rateClause,   
            "  )", sep="")
    
    MM <- sqlQuery(options$con, query)
    
    names(MM) <- gsub("_", ".", tolower(names(MM)))
    rLog(paste("There are ", length(unique(MM$ne.acctno)),
                    " unique accounts.", sep=""))
    
    #MM$data.source <- NULL    # may still want to keep it
    return(MM)
}
#==================================================================
# Get all the load for a given weather profile and sic.code.  Should fit in
# memory.
#
get.all.load.weather.sic <- function(busunit.id=7, weather, sic, options){
    
    rLog(paste("Grabbing hist pricing data for ", weather, sep=""))
    
    query <- paste(
            "select ne_acctno, data_source, read_dt, hour, kw ", 
            "from bn_load.smis_hourly_usage_t where busunit_id =", busunit.id,
            "  and ne_acctno in ( ", 
            "    select sa.ne_acctno",
            "    from bn_load.smis_account_t sa, ",
            "         cer.company c, ",
            "         cer.account a ", 
            "    where upper(sa.busunit_id) = ", toupper(busunit.id),   
            "    and c.company_id = a.company_id ", 
            "    and sa.ne_acctno = a.servacct_our_num ", 
            "    and substr(trim(c.sic_code), 1, 2) = '", sic,"' ", 
            "    and sa.edc_loadprofile like 'BUSIDRRQ_", weather, "%'",
            "  )", sep="")
    MM <- sqlQuery(options$con, query)
    
    names(MM) <- gsub("_", ".", tolower(names(MM)))
    rLog(paste("There are ", length(unique(MM$ne.acctno)),
                    " unique accounts.", sep=""))
    
    #MM$data.source <- NULL    # may still want to keep it
    return(MM)
}

#==================================================================
# Find the maximum common period. 
#
find.max.period <- function(MM){
    
}

#==================================================================
# Remove outliers.  Check if the min/max of the load is 3 interquartile
# range below/above the median
remove.outliers<- function(LL, options){
#   fivenum(LL$kw)
    return(LL)
}



#==================================================================
# Regress load to temperature.
#
regress.LT <- function(HH, wth.station, options){
    
    max.dt <- as.Date(max(HH$read.dt))
    min.dt <- as.Date(min(HH$read.dt))
    #--------------------------------- read historical temperatures
    TT.max <- tsdb.readCurve(paste("Temp_Clean_", wth.station, "_Dly_Max",
                    sep=""), min.dt, max.dt)
    names(TT.max)[2] <- "max"
    TT.min <- tsdb.readCurve(paste("Temp_Clean_", wth.station, "_Dly_Max",
                    sep=""), min.dt, max.dt)
    names(TT.min)[2] <- "min"
    TT <- merge(TT.max, TT.min)
    
    TT$avg.temperature <- (TT$max + TT$min)/2
    TT$max <- TT$min <- NULL
    
    # index of business days
    bdays <- data.frame(date=seq(min.dt, max.dt, by="day"),
            is.bus=!secdb.isHoliday(seq(min.dt, max.dt, by="day"))) 
    
    uAccounts <- sort(unique(HH$ne.acctno))
    # aggregate to daily for use ...
    names(HH)[which(names(HH)=="kw")] <- "value" 
    DD <- cast(HH, ne.acctno + read.dt ~ ., sum)
    names(DD)[2:3] <- c("date", "kw")
    DD$date <- as.Date(DD$date)
    
    DD <- merge(DD, TT, all.x=TRUE)  # add the temperature
    
    # store the regression results
    reg <- vector("list", length=length(uAccounts))
    names(reg) <- uAccounts
    # loop over accounts 
    for (a in 1:length(uAccounts)){
        rLog("--> Regressing account", uAccounts[a])
        LL <- subset(DD, as.character(DD$ne.acctno)==uAccounts[a])
        ddate <- which(diff(LL$date) > 365)
        if (length(ddate)>0){
            rLog(paste("WARNING: Found ", length(ddate), " gap(s) bigger than ",
                            "365 days in historical data.", sep=""))
            LL <- LL[(max(ddate)+1):nrow(LL),]
        }
        if (nrow(LL)< 365){
            rLog("WARNING: Less then 365 days of data!  Take a look!")
        }
        # cleaning historical data for obvious outliers
        LL <- remove.outliers(LL, options)
        
        LL <- merge(LL, data.frame(date=options$aggregate$days, to.agg=TRUE),
                all=T)
        if (!any(is.na(LL$kw[LL$to.agg]))){
            rLog("Lucky you!  You don't need to forecast this account. Next!")
        } else {
            rLog("We need to forecast", length(is.na(LL$kw[LL$to.agg])),
                    "days.")
        }
        
        
        LL <- merge(LL, bdays, all.x=T)
        LL$month <- as.numeric(format(LL$date, "%m"))
        months.to.agg <- sort(unique(LL$month[is.na(LL$kw)]))
        
        plot(LL$date, LL$kw)
        points(LL$date[LL$is.bus], LL$kw[LL$is.bus], col="blue")
        
        reg <- NULL
        rLog("Regress load vs. temperature ...")
        for (is.bday in c(TRUE, FALSE)){
            rLog("  For business days =", is.bday)
            for (m in months.to.agg){
                rLog("    For month =", m)
                ll <- subset(LL, is.bus==is.bday & month==m & !is.na(kw))
                if (nrow(ll) < options$alarm.no.hdays){
                    rLog("      Not much data, only", nrow(ll), "days.") 
                }
                res  <- rlm(kw ~ avg.temperature, data=ll)
                sres <- summary(res)
                if (abs(sres$coefficients["avg.temperature", "t value"]) < 1.65){
                    rLog("      WARNING: the regression to temperature is not great!")
                }
                reg <- rbind(reg, c(is.bday = is.bday, 
                                month = m,
                                intercept = sres$coefficients[1,"Value"],
                                slope = sres$coefficients[2,"Value"],
                                sigma = sres$sigma))
            }
        }
        rLog("Done!")
        
        
        
    }  
    
    
}
