# Collect some curve info, maybe useful later.  
#
#

get.curve.info <- function(options){
  
  cat("Reading in curve denomination ... ")  
  query <- paste("select commod_curve, denominated ",
    "from foitsox.sdb_commod_market_location where pricing_date=to_date('",
    options$asOfDate, "','yyyy-mm-dd') and denominated <> 'USD'", sep="")
  den <- sqlQuery(options$connections$CPSPROD, query)
  colnames(den) <- c("curve.name", "denominated")
  den$curve.name <- as.character(den$curve.name)
  den$denominated <- as.character(den$denominated)
  cat("Done!\n")

  cat("Reading in curve region mapping ... ")  
  query <- paste("select upper(t.commod_curve), v.region, upper(t.linked_location), ",
    "T.quantity_bucket_type ",             
    "FROM FOITSOX.SDB_COMMOD_MARKET_LOCATION T, FOITSOX.RMG_CPSCURVES s, ",
    "FOITSOX.RMG_COMMODITIES r, FOITSOX.RMG_SUBREGIONS u, FOITSOX.RMG_REGIONS v ", 
    "where pricing_date=to_date('", options$asOfDate, "', 'yyyy-mm-dd') ", 
    "and t.COMMOD_CURVE=s.CURVENAME and s.COMMID=r.COMM_ID and s.SRID=u.SR_ID ",
    "and u.REGION_ID=v.REGION_ID", sep="")
  curve.info <- sqlQuery(options$connections$CPSPROD, query)
  colnames(curve.info)  <- c("curve.name", "region", "linked.location", "bucket")
  curve.info <- as.data.frame(sapply(curve.info, as.character), stringsAsFactors=FALSE)
  
  aux <- strsplit(curve.info$curve.name, " ")
  aux <- unlist(lapply(aux, function(x){paste(x[1:2], collapse=" ")}))
  curve.info$linked.location <- ifelse(curve.info$linked.location != "NA",
    paste(aux, curve.info$linked.location), "NA")
  cat("Done!\n")

  curve.info <- .map.buckets(curve.info)   # map buckets to peak/offpeak/flat
   
  cat("Reading in buddy curve info ... ")  
  query <- paste("select commod_curve, buddy_curve from ",
    "foitsox.sdb_buddy_curve where pricing_date=to_date('",
    options$asOfDate, "', 'yyyy-mm-dd') ", sep="")
  res <- sqlQuery(options$connections$CPSPROD, query)
  names(res) <- c("curve.name", "buddy.curve")
  res$buddy.curve <- as.character(res$buddy.curve)
  curve.info <- merge(curve.info, res, all=TRUE)
  cat("Done!\n")

  filename <- paste(options$save$datastore.dir, "curve.info.",
                    options$asOfDate, ".RData",sep="")
  save(den, curve.info, file=filename)
}

#=================================================================================
# Map original SecDB buckets to peak/offpeak/flat. 
# Where curve.info is a data.frame with two columns (curve.name, bucket)
#
.map.buckets <- function(curve.info){

  curve.info$peak.offpeak <- NA              
  curve.info$bucket <- toupper(curve.info$bucket)
  uBuckets <- unique(curve.info$bucket)

  ind <- c(grep("X8", uBuckets),
           grep("2X16", uBuckets), grep("OFFPEAK", uBuckets),
           grep("1X16", uBuckets),             # SAT1X16, SUN1X16
           grep("[12]X24", uBuckets), 
           grep("-WE", uBuckets),              # the UK WeekEnd bloks
           grep("B[126]-WD", uBuckets)         # the UK WeekDay blocks
           )
  if (length(ind)>0){
    ind.c <- which(curve.info$bucket %in% uBuckets[ind])
    curve.info$peak.offpeak[ind.c] <- "Offpeak"
    uBuckets <- uBuckets[-ind]
  }

  ind <- c(grep("[56]X16", uBuckets),                          
           grep("PEAK", uBuckets),
           grep("PK", uBuckets),
           grep("B[345]-WD", uBuckets)
           )
  if (length(ind)>0){
    ind.c <- which(curve.info$bucket %in% uBuckets[ind])
    curve.info$peak.offpeak[ind.c] <- "Peak"
    uBuckets <- uBuckets[-ind]
  }
  
  ind <- c(grep("7X24", uBuckets),                          
           grep("FLAT", uBuckets)
           )
  if (length(ind)>0){
    ind.c <- which(curve.info$bucket %in% uBuckets[ind])
    curve.info$peak.offpeak[ind.c] <- "Flat"
    uBuckets <- uBuckets[-ind]
  }

  uBuckets <- na.omit(uBuckets)  # there are some VLR curves...
  if (length(uBuckets)>0){
    cat("Inside function VaR/Overnight/get.curve.info.R\n")
    cat("Could not map buckets", uBuckets, "\n")
  }
  
  return(curve.info)
}






# Not sure why I was using this before ... AAD
#  curve.info$hub.name   <- "Nymex"
#  curve.info$hub.curve  <- "COMMOD NG EXCHANGE"
#  curve.info$vol.type   <- NA 
