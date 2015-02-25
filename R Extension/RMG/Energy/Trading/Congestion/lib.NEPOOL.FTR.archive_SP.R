# Maintain an archive of settled prices by ptid, class.type, month, year
#
# .read_SP_archive
# .update_SP_archive
# 

##########################################################################
# always redo the current year
#
.combine_SP <- function( SP_old, SP_new )
{
  SP <- list(year=NULL, month=NULL, lastUpdated=NULL)
  
  ind <- SP_old$year$year %in% unique(SP_new$year$year)
  SP$year <- SP_old$year[!ind,]               # take them out
  SP$year <- rbind(SP$year, SP_new$year)      # put the new ones in
  SP$year <- SP$year[order(SP$year$ptid, SP$year$bucket, SP$year$year), ]
  rownames(SP$year) <- NULL

  ind <- SP_old$month$month %in% unique(SP_new$month$month)
  SP$month <- SP_old$month[!ind,]                # take them out
  SP$month <- rbind(SP$month, SP_new$month)      # put the new ones in
  SP$month <- SP$month[order(SP$month$ptid, SP$month$bucket, SP$month$month), ]
  rownames(SP$month) <- NULL

  SP$lastUpdated <- SP_new$lastUpdated

  SP
}


##########################################################################
# always redo the current year
#
.get_startDateTime <- function( lastUpdated )
{
  aux <- as.POSIXlt(lastUpdated)
  startDateTime <- ISOdatetime(aux$year+1900, 1, 1, 1, 0, 0)

  startDateTime
}

##########################################################################
# 
.read_SP_archive <- function()
{
  dir.rdata <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
  filename <- paste(dir.rdata, "ftrsp.RData", sep="")

  if (file.exists(filename)) {
    load(filename)
  } else {
    SP <- NULL
  }
  
  SP
}


##########################################################################
# Calculate congestion settle prices by year and month
# and save it to a file
# 
.update_SP_archive <- function(do.save=FALSE)
{
  FTR.load.ISO.env("NEPOOL")

  SP <- .read_SP_archive()

  
  startDt <- .get_startDateTime(SP$lastUpdated)
  endDt <- Sys.time()
  rLog("Reading tsdb from ", format(startDt))
  
  ptids <- NEPOOL$MAP$ptid
  #ptids <- ptids[1:10]

  bucketMask <- secdb.getBucketMask("NEPOOL", "Peak", startDt, endDt) 
  bucketMask$bucket <- ifelse(bucketMask$Peak, "ONPEAK", "OFFPEAK")
  
  
  res <- lapply(ptids, function(ptid)
  {  
    rLog("Working on ptid", ptid)
    symb <- paste("nepool_smd_da_", ptid, "_congcomp", sep="")
    hp <- tsdb.readCurve(symb, startDt, endDt)
    if (nrow(hp) == 0)
      return( NULL )
      
    hp$year  <- as.numeric(format(hp$time-1, "%Y"))
    hp$month <- as.Date(format(hp$time-1, "%Y-%m-01"))
    hpB <- merge(hp, bucketMask[,c("time", "bucket")], by="time", )

    mm <- cast(hpB, bucket + month ~ ., mean, value="value")
    mm <- cbind(ptid=ptid, as.data.frame(mm))

    yy <- cast(hpB, bucket + year ~ ., mean, value="value")
    yy <- cbind(ptid=ptid, as.data.frame(yy))
 
    list(month = mm,
         year  = yy)
  })

  SP.month <- do.call(rbind, lapply(res, "[[", "month"))
  colnames(SP.month)[ncol(SP.month)] <- "SP"
  SP.year <- do.call(rbind, lapply(res, "[[", "year"))
  colnames(SP.year)[ncol(SP.year)] <- "SP"

  if (do.save) {
    SP_new <- list(month=SP.month,
                   year=SP.year,
                   lastUpdated=Sys.time())
    SP_old <- SP
    
    SP <- .combine_SP( SP_old, SP_new )
    
    dir.rdata <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
    filename <- paste(dir.rdata, "ftrsp.RData", sep="")
    save(SP, file=filename)
    rLog("Saved settle price archive to", filename)
  }
  
}



##########################################################################
##########################################################################
# 
.test <- function()
{
  require(CEGbase)
  require(FTR)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.archive_SP.R")

  .update_SP_archive(do.save=TRUE)
}
