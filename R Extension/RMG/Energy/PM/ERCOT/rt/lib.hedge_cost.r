# Code used for calculating hedge cost for pricing
#
#
# Written by Chelsea Yeager on 16-Apr-2012



#------------------------------------------------------------
# PULL ELEC PRICES 
# 
.pull_prices <- function(asOfDates, startDate, endDate)
{
  ## STATE VARIABLES ##
    commodity   <- "COMMOD PWR ERCOT PHYSICAL"
    location    <- c("HOUSTON HUB RT", "NORTH HUB RT", "SOUTH HUB RT",
      "WEST HUB RT", "HOUSTON LZ RT", "NORTH LZ RT", "SOUTH LZ RT", "WEST LZ RT")
    bucket      <- c("PEAK", "OFFPEAK")
    serviceType <- "Energy"
    useFutDates <- FALSE

    # read existing file
    archive_file <- "S:/All/Portfolio Management/Ercot/Fees/Hedge Costs/R Data/elec_price_history.RData"
    valDate <- if (file.exists(archive_file)) {
      rLog("Read archive file.")
      load(archive_file)
      haveDays <- as.Date(colnames(resArchive)[4:ncol(resArchive)])
      as.Date(setdiff(asOfDates, haveDays), origin="1970-01-01")
    } else {
      resArchive <- NULL
      asOfDates
    }

    
    if ( length(valDate) == 0 ) {
      
      rLog("All prices in archive.  Exiting.")
      
    } else {
      
      rLog("Pulling prices for ", as.character(valDate))
      aux <- try(secdb.getElecPrices( valDate, startDate, endDate,
        commodity, location, bucket, serviceType, useFutDates))
      res <- cast(aux, month + location + bucket ~ asOfDate, I, value = "value", fill = NA)

      rLog("Overwrite/append the new days")
      if (is.null(resArchive)) {
        resArchive <- res                     # archive was empty
      } else {
        resArchive <- merge(resArchive, res)  # archive had some dates in 
      }
       
      allDays <- format(colnames(resArchive)[4:ncol(resArchive)])
      resArchive <- cbind(resArchive[,1:3], resArchive[, allDays[rev(order(allDays))]])

      save(resArchive, file=archive_file)
      rLog("Saved prices to ", archive_file)
      
    }     
            
    
    resArchive
}


 

#------------------------------------------------------------
# FUNCTIONS
#   
# 

Calculate.HedgeCost = function(strDays)
  {
 
        
        ## CHOOSE DATES ##
          
          arrDate <- data.frame( read.csv(
            file="//ceg/cershare/All/Portfolio Management/ERCOT/Yeager/R/cvs/dates.csv",
            header = TRUE, sep = ","))
        
          arrDate$Date = as.Date(arrDate$Date)
          strVal   <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
          strRow = match(strVal, arrDate$Date)
        
          arrDate <- arrDate[1:strRow,]
          arrBus <- subset(arrDate, Holiday == 0)
          strRow <- match(max(arrBus$Date), arrBus$Date)
          strRowBeg <- strRow - strDays
          strRow <- as.numeric(strRow)
          arrBus <- arrBus[strRowBeg:strRow,]

          startDate <-  as.Date(secdb.dateRuleApply(Sys.Date(), "1m"))
          endDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "+6y"))
          asOfDates <- arrBus$Date

          resComp <- .pull_prices(asOfDates, startDate, endDate)
          resComp <- resComp[,c(1:(strDays+3))]  
          
          rLog("Calculate the standard deviations")
          MM <- resComp[ ,4:ncol(resComp)]
          #ncol = # of columns
          DD <- MM [ , 1:ncol(MM)-1] - MM[ , 2:(ncol(MM))]
          ##DD[DD<=0] <- NA
          
          HedgeCost = cbind(resComp [ , 1:3], HedgeCost = apply(DD, 1, sd, na.rm = TRUE))
          Final <- cast(HedgeCost, month ~ location + bucket, I, value = "HedgeCost", fill = NA)
          


          asDate <- max(arrBus$Date)
         ## WRITE FILES ##
          
          filename <- paste("//ceg/cershare/All/Portfolio Management/ERCOT/Fees/Hedge Costs/R Data/ERCOT Hedge Costs ", asDate,".csv", sep="")
          write.csv(Final, file=filename)
          rLog("Wrote hedge costs to", filename)
          
          
  }
