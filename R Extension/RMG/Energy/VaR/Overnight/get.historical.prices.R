# Functions to extract historical prices and vols from CPSPROD, foitsox.
# With RODBC it takes about two min to get all prices for one day. 
#
# Written by Adrian Dragulescu on 23-Nov-2006

get.one.day.prices <- function(day, options){  # about 2 min
    cat(paste("Get all prices for", day, "... "))
  query <- paste("SELECT commod_curve, start_date, end_date, price ",
    "from foitsox.sdb_prices WHERE PRICING_DATE=TO_DATE('",
    as.character(day), "','YYYY-MM-DD') order by commod_curve, end_date", sep="")
    hP <- sqlQuery(options$connections$CPSPROD, query)  # all the prices 
    names(hP) <- c("curve.name", "start.dt", "end.dt", as.character(day))
    hP$start.dt <- as.Date(hP$start.dt); hP$end.dt <- as.Date(hP$end.dt)
    hP$contract.dt <- as.Date(format(hP$start.dt, "%Y-%m-01")) 
    eom <- as.Date(format(hP$contract.dt + 31, "%Y-%m-01"))-1  # end of the month 
    hP  <- hP[(hP$end.dt == eom),]  # keep only the balance of the month prices
    hP$start.dt <- hP$end.dt <- NULL
    hP$curve.name <- toupper(as.character(hP$curve.name)) 
    hP <- hP[!duplicated(hP),]                   # there are some weird duplicates
    
    filename <- paste(options$save$datastore.dir,"Prices/hPrices.",day,".RData",sep="")
    if (nrow(hP)>0){save(hP, file=filename)}
    cat("Done!\n")
}

get.one.day.vols <- function(day, options){   # about 10 min
    cat(paste("Get all vols for", day, "... "))
    query <- paste("SELECT commod_curve, contract_date, type, ",
            "volatility from foitsox.sdb_bs_volatility where ",
            "pricing_date=to_date('", day, "','YYYY-MM-DD') ",
            "and strike_pct = 0.5", sep="")
    hV   <- sqlQuery(options$connections$CPSPROD, query)  # all the vols 
    colnames(hV)   <- c("curve.name", "contract.dt", "vol.type", as.character(day))
    hV$contract.dt <- as.Date(hV$contract.dt)
    hV$curve.name  <- toupper(as.character(hV$curve.name))
    hV$vol.type    <- as.character(hV$vol.type)
    hV <- hV[!duplicated(hV),]                   # there are some weird duplicates
    
    filename <- paste(options$save$datastore.dir,"Vols/hVols.",day,".RData", sep="")
    if (nrow(hV)>0){save(hV, file=filename)}
    cat("Done!\n")  
}
get.range.days.prices <- function(start.dt, end.dt, options){
    days <- seq(start.dt, end.dt, by=1)
    days <- days[days %in% options$calc$hdays]
    for (day in as.character(days)){get.one.day.prices(day, options)}
}
get.range.days.vols <- function(start.dt, end.dt, options){
    days <- seq(start.dt, end.dt, by=1)
    days <- days[days %in% options$calc$hdays]
    for (day in as.character(days)){get.one.day.vols(day, options)}
}

##   hV$pricing.dt  <- as.Date(day)
##   hV <- reshape(hV, idvar=c("curve.name", "contract.dt", "vol.type"),
##     timevar="pricing.dt", direction="wide")   # transpose the vols
##   colnames(hV)  <- gsub("value.", "", colnames(hV))
