# Functions to extract historical prices and vols from CPSPROD, foitsox.
# With RODBC it takes about two min to get all prices for one day. 
#
# Written by Adrian Dragulescu on 23-Nov-2006

get.one.day.prices <- function(day, options){
  cat(paste("Get all prices for", day, "... "))
  query <- paste("SELECT commod_curve, start_date, end_date, price ",
    "from foitsox.sdb_prices WHERE PRICING_DATE=TO_DATE('",
    as.character(day), "','YYYY-MM-DD') order by commod_curve, end_date", sep="")
  hP <- sqlQuery(options$connections$CPSPROD, query)  # all the prices 
  names(hP) <- c("curve.name", "start.dt", "end.dt", "value")
  hP$start.dt <- as.Date(hP$start.dt); hP$end.dt <- as.Date(hP$end.dt)
  hP$contract.dt <- as.Date(format(hP$start.dt, "%Y-%m-01")) 
  #------------------------------do the averaging for the cash month----------------
  ind <- which(hP$contract.dt == format(day, "%Y-%m-01"))
  aux <- hP[ind,]; hP <- hP[-ind,]
  aux$eom <- as.Date(format(aux$contract.dt + 31, "%Y-%m-01"))-1  # end of the month
  aux$days.in.month <- as.numeric(ifelse(aux$end.dt-aux$start.dt<aux$eom-aux$contract.dt,
    aux$eom-day+1, aux$eom-aux$contract.dt+1))
  aux$weight <- ifelse(aux$end.dt==day, 1, aux$end.dt-aux$start.dt+1)/aux$days.in.month
  aux$value  <- aux$value*aux$weight
  aux <- cast(aux, curve.name + contract.dt ~ ., sum)

  hP <- rbind(hP[,c("curve.name","contract.dt","value")], aux)
  hP$curve.name <- as.character(hP$curve.name)
  names(hP)[3]  <- as.character(day)
  filename <- paste(options$save$datastore.dir,"/hPrices.",day,".RData",sep="")
  if (nrow(hP)>0){save(hP, file=filename)}
  cat("Done!")
}

get.one.day.vols <- function(day, options){
  cat(paste("Get all vols for", day, "... "))
  query <- paste("SELECT commod_curve, contract_date, type, ",
    "volatility from foitsox.sdb_bs_volatility where ",
    "pricing_date=to_date('", day, "','YYYY-MM-DD') ",
    "and strike_pct = 0.5", sep="")
  hV   <- sqlQuery(options$connections$CPSPROD, query)  # all the prices 
  colnames(hV)   <- c("curve.name", "contract.dt", "vol.type", "value")
  hV$contract.dt <- as.Date(hV$contract.dt)
  hV$pricing.dt  <- as.Date(day)
  hV$curve.name  <- as.character(hV$curve.name)
  hV$vol.type    <- as.character(hV$vol.type)
  hV <- reshape(hV, idvar=c("curve.name", "contract.dt", "vol.type"),
    timevar="pricing.dt", direction="wide")   # transpose the vols
  colnames(hV)  <- gsub("value.", "", colnames(hV))
  
  filename <- paste(options$save$datastore.dir,"/hVols.",day,".RData", sep="")
  if (nrow(hV)>0){save(hV, file=filename)}
  cat("Done!")  
}
get.range.days.prices <- function(start.dt, end.dt, options){
  days <- seq(start.dt, end.dt, by=1)
  for (day in as.character(days)){get.one.day.prices(day, options)}
}
get.range.days.vols <- function(start.dt, end.dt, options){
  days <- seq(start.dt, end.dt, by=1)
  for (day in as.character(days)){get.one.day.vols(day, options)}
}
