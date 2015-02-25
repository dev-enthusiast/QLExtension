# Deal with SPP inc/decs
#
# min.max.yest
# bucketized.decs
#
# Rob Cirincione, 10/9/2014



###############################################################################
#takes data frame "prices" as input and returns min/max nodes for any hour
#
min.max.yest <- function(prices)
{ 
  aux <- data.frame(day=as.Date(format(index(prices)-1, "%Y-%m-%d")),
                    datetime=index(prices), prices)
  bux  <- melt(aux, id=1:2)
  cux  <- subset(bux, day==as.Date(Sys.Date()-1))
  
  res <- ddply(cux, c("day"), function(x) {
    
          x <- x[order(x$value, decreasing=TRUE),]
    
          rbind(head(x, 10), tail(x,10))
           })
  res$value <- round(res$value, 2)
  
  res
}


###############################################################################
#Bucketizes a data frame "prices" of dec's or inc's by peak and offpeak hours. 
#Returns top 10 locations by bucket
#
bucketized.decs <- function(prices)
{  
  #creates a character identifier for each hour 
  hour   <- as.POSIXlt(index(prices))$hour 
  bucket <- sapply(hour, function(x) {
     ifelse(x %in% 8:23, bucket <- "Peak", bucket <- "Offpeak")
    })
  
  #Add bucket ID to data frame. Melt data frame and average on peak/offpeak
  aux <- data.frame(day=as.Date(format(index(prices)-1, "%Y-%m-%d")), bucket, prices)
  
  bux <- melt(aux, id=c("day","bucket"))
  
  cux <- ddply(bux, c( "bucket", "variable"), function(x) {
    bucketMean <- mean(x$value) 
    })
  
  colnames(cux)[3] <- "avg" #Question: why does the ddply above set this colname to v1?
  
  #Now order the data frame by each bucket, then keep only top and bottom 10
  res <- ddply(cux, c( "bucket"), function(x) {

    x <- x[order(x$avg, decreasing=TRUE),]
    
    rbind(head(x, 10), tail(x,10))
  })
  res$avg <- round(res$avg, 2)

  res
}

####################################################################################
#calculates pnl of two simple strategies: 1. Trend following: if DART is positive, 
#buy it for the next day, if not, sell. 2. Contrarian: if DART is positive, sell it
#for next day, if not, buy. Top 10 by bucket for past 10 days. 
#

do.opposite <- function(prices) {
  
  #creates a character identifier for each hour 
  hour   <- as.POSIXlt(index(prices))$hour 
  bucket <- sapply(hour, function(x) {
    ifelse(x %in% 8:23, bucket <- "Peak", bucket <- "Offpeak")
  })
  
  #Add bucket ID to data frame. Melt data frame and average on peak/offpeak
  aux <- data.frame(day=as.Date(format(index(prices)-1, "%Y-%m-%d")), bucket, prices)
  bux <- melt(aux, id=c("day","bucket"))
  cux <- ddply(bux, c("day", "bucket", "variable"), function(x) {
    bucketMean <- mean(x$value) 
  })
  
  colnames(cux)[4] <- "avg"
  
  peak    <- subset(cux, bucket=="Peak")
  offpeak <- subset(cux, bucket=="Offpeak") 
  
  #reshape the data as a (day x location) array
  peakC    <- cast(peak, day + bucket ~ variable)
  offpeakC <- cast(offpeak, day + bucket ~ variable)
  
  #creates a dummy data frame to store the pnl of the strategy
  peakOpp    <- as.data.frame(matrix(NA, nrow(peakC)-1, ncol(peakC)-2))
  offpeakOpp <- as.data.frame(matrix(NA, nrow(offpeakC)-1, ncol(offpeakC)-2))
  
  peakTrend    <- as.data.frame(matrix(NA, nrow(peakC)-1, ncol(peakC)-2))
  offpeakTrend <- as.data.frame(matrix(NA, nrow(offpeakC)-1, ncol(offpeakC)-2))
  
  colnames(peakOpp)      <- colnames(prices)
  colnames(offpeakOpp)   <- colnames(prices)
  colnames(peakTrend)    <- colnames(prices)
  colnames(offpeakTrend) <- colnames(prices)
  
  #pnl of opposite strategy. start on column 3 as 1&2 are day, bucket
  for(i in 1:nrow(peakOpp) ) { 
    for(j in 1:ncol(peakOpp)) {
      
      ifelse(peakC[i,j+2] > 0, flag <- "sell", flag <- "buy")
      
      if(flag=="buy")    peakOpp[i,j] <-  peakC[i+1,j+2]  
      if(flag=="sell")   peakOpp[i,j] <- -peakC[i+1,j+2] 
    } 
  }
  
  for(i in 1:nrow(offpeakOpp) ) { 
    for(j in 1:ncol(offpeakOpp)) {
      
      ifelse(offpeakC[i,j+2] > 0, flag <- "sell", flag <- "buy")
      
      if(flag=="buy")    offpeakOpp[i,j] <-  offpeakC[i+1,j+2]  
      if(flag=="sell")   offpeakOpp[i,j] <- -offpeakC[i+1,j+2]
    } 
  }
  
  #pnl of trend following strategy. start on column 3 as 1&2 are day, bucket
  for(i in 1:nrow(peakTrend) ) { 
    for(j in 1:ncol(peakTrend)) {
      
      ifelse(peakC[i,j+2] > 0, flag <- "buy", flag <- "sell")
      
      if(flag=="buy")   peakTrend[i,j] <-  peakC[i+1,j+2]  
      if(flag=="sell")  peakTrend[i,j] <- -peakC[i+1,j+2]
    } 
  }
  
  for(i in 1:nrow(offpeakTrend) ) { 
    for(j in 1:ncol(offpeakTrend)) {
      
      ifelse(offpeakC[i,j+2] > 0, flag <- "buy", flag <- "sell")
      
      if(flag=="buy")   offpeakTrend[i,j] <-  offpeakC[i+1,j+2]  
      if(flag=="sell")  offpeakTrend[i,j] <- -offpeakC[i+1,j+2]
    } 
  }
  
  peakOppm      <- melt(peakOpp)
  offpeakOppm   <- melt(offpeakOpp)
  peakTrendm    <- melt(peakTrend)
  offpeakTrendm <- melt(offpeakTrend)
  
  resP <- ddply(peakOppm, c("variable"), function(x) {
    x <- round(sum(x$value),2)
  })
  
  resPt <- ddply(peakTrendm, c("variable"), function(x) {
    x <- round(sum(x$value),2)
  })
  
  resO <- ddply(offpeakOppm, c("variable"), function(x) {
    x <- round(sum(x$value),2)
  })
  
  resOt <- ddply(offpeakTrendm, c("variable"), function(x) {
    x <- round(sum(x$value),2)
  })
  
  colnames(resP)[2]  <- "value"
  colnames(resO)[2]  <- "value"
  colnames(resPt)[2] <- "value"
  colnames(resOt)[2] <- "value"
  
  resP  <- resP[order(resP$value, decreasing="true"),]
  resO  <- resO[order(resO$value, decreasing="true"),]
  resPt <- resPt[order(resPt$value, decreasing="true"),]
  resOt <- resOt[order(resOt$value, decreasing="true"),]
  
  output <- cbind(bucket=rep("peak", 10), rbind(head(resP, 10)))
  
  output <- rbind(output, cbind(bucket=rep("Offpeak", 10), rbind(head(resO, 10))))
  
  output <- cbind(strategy=rep("Contrarian", 20), output)
  
  output2 <- cbind(bucket=rep("peak", 10), rbind(head(resPt, 10)))
  
  output2 <- rbind(output2, cbind(bucket=rep("Offpeak", 10), rbind(head(resOt, 10))))
  
  output2 <- cbind(strategy=rep("Trend Following", 20), output2)
  
  output3 <- rbind(output2, output)
  
}

#################################################################
#
.main <- function()
{ 
  library(CEGbase)
  library(SecDb)
  library(CRR)
  library(FTR)
  library(zoo)
  library(reshape)

  rLog(paste("Started process at ", Sys.time()))

  
  # load your SPP environment, for now it only contains a MAP
  load_ISO_environment(region="SPP")
  #head(SPP$MAP)

  pnodes        <- SPP$MAP$Pnode #actual ISO Pnode names
  ptid          <- SPP$MAP$ptid #associated ptid settlement locations
  tsdbDAM       <- SPP$MAP$DAM.Symbol #corresponding TSDB DAM symbols
  tsdbRT        <- SPP$MAP$LMP.Symbol #corresponding TSDB RT symbols
  pnodeLength   <- length(pnodes)


  #sets the date to pull settlement data (currently yesterday)
  start <- as.POSIXct(paste(Sys.Date()-10, "00:00:05", sep=" "))
  end   <- as.POSIXct(paste(Sys.Date(), "00:00:00", sep=" "))

  #Creates a zoo matrix of DAM and RT by reading TSDB symbols. Avg by hour.
  sppDAM <- FTR.load.tsdb.symbols(tsdbDAM, start, end,
                                   aggHourly=rep(TRUE, length(tsdbDAM)))

  sppRT  <- FTR.load.tsdb.symbols(tsdbRT, start, end,
                                   aggHourly=rep(TRUE, length(tsdbRT)))

  #Sets column names to ptid's. Some tsdb's are empty hence the logical check
  colnames(sppDAM) <- ptid[tsdbDAM %in% names(sppDAM)] 
  colnames(sppRT)  <- ptid[tsdbRT %in% names(sppRT)] 

  dec <- sppRT - sppDAM
  rLog("Finished calculating dec")

  minmax <- min.max.yest(dec)
  out <- c(paste("Top 10 Incs and Decs from yesterday", as.Date(Sys.Date()-1)),
    "\n", capture.output(print(minmax, row.names=FALSE)))
  
  dartByBucket <- bucketized.decs(dec)
  out <- c(out, "\n\n",
           paste("Top 10 Incs and Decs by bucket from", as.Date(start), "to", as.Date(end)-1),
            "\n", capture.output(print(dartByBucket, row.names=FALSE)))
  
  oppositePNL <- do.opposite(dec)
  out <- c(out, "\n\n", paste("Top 10 performing nodes under a contrarian strategy from", 
                         as.Date(start), "to", as.Date(end)-1),
           "\n", capture.output(print(oppositePNL, row.names=FALSE)))

  rLog("Email results")
  out <- c(out, "\n\n", 
               "procmon job: RMG/R/Reports/Energy/PM/SPP/spp_inc-dec",
               "contact: Rob Cirincione")
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  to   <- paste(c("adrian.dragulescu@constellation.com",
                  "robert.cirincione@constellation.com",
                  "robert.monachello@constellation.com",
                  "james.lacer@constellation.com"), sep="", collapse=",")
  subject <- "SPP inc/dec report"
  sendEmail(from, to, subject, out)

  # uhh new stuff ...
  rLog(paste("Done at ", Sys.time()))

  if( !interactive() )
    q( status = 0 )

  
}


#################################################################
#################################################################
.main()
