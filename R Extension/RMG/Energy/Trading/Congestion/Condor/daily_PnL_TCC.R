# Report TCC PnL 
#
#
# Written by AAD on 15-Aug-2014


#################################################################
#
.calculate_PnL <- function( asOfDate, filename, sheetIndex, sheetName )
{
  x <- .read_exgen_awards_xlsx(filename, sheetIndex, sheetName=sheetName)
  
  # get only the active deals
  y <- subset(x, startDt <= asOfDate & endDt >= asOfDate)
  y <- cbind(path.no=1:nrow(y), y)

  paths <- dlply(y, "path.no", function(x){
    Path(x$source.ptid,
         x$sink.ptid,
         bucket="FLAT",
         auction=parse_auctionName(x$auctionName, region="NYPP"),
         mw=x$mw)
  })
  
  payoff <- get_payoff( paths )

  res <- y[,c("path.no", "strategy", "auctionName", "bucket", "mw",
              "source.ptid", "sink.ptid")]
  res$source.name <- sapply(paths, function(x) x$source.name)
  res$sink.name   <- sapply(paths, function(x) x$sink.name)
  res$PnL.from_start <- round(payoff$FROM_START)
  res$PnL.this_month <- round(payoff$CURRENT_MONTH)
  res$PnL.next_day   <- round(payoff$NEXT_DAY)
  
  res
}



#################################################################
#################################################################
#
main <- function()
{
  options(width=800)  # make some room for the output
  options(stringsAsFactors=FALSE) 
  library(methods)   # not sure why this does not load!  It's documented in ?Rscript
  library(CEGbase)
  library(SecDb)
  library(plyr)
  library(CRR)
  library(xlsx)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.awards.R")

  rLog(paste("Start proces at ", Sys.time()))
  returnCode <- 0    # succeed
  
  asOfDate <- Sys.Date() + 1        # asOfDate <- as.Date("2009-04-10") #
  rLog(paste("Running report on", as.character(asOfDate)))
  rLog("\n\n")

  load_ISO_environment(region="NYPP")

  #------------------------------------------------------------------
  rLog("Calculate spec PnL")
  #filename <- "S:/Trading/Nodal FTRs/NYISO/Auctions/awards.xlsx"
  filename <- "S:/Trading/NYTCC_Nodal/awards.xlsx"
  sheetName <- "NYISO"
  pnl <- .calculate_PnL( asOfDate, filename, sheetName=sheetName )
  
  out <- c(
    paste("Total TCC PnL next day:  ", dollar(sum(pnl$PnL.next_day))),         
    paste("Total TCC PnL this month:", dollar(sum(pnl$PnL.this_month))),
    paste("Total TCC PnL from start:", dollar(sum(pnl$PnL.from_start))),
     "\n\n",
    capture.output(print(pnl, row.names=FALSE)),
    "\n\nAwards file: S:/Trading/Nodal FTRs/NYISO/Auctions/awards.xlsx",       
    "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/daily_PnL_TCC",
    "\nContact: Adrian Dragulescu")

  rLog("Email results")
  to <- paste(c("adrian.dragulescu@constellation.com","rujun.hu@constellation.com",
                "dibyendu.das@constellation.com", "Vikram.Krishnaswamy@constellation.com",
                "andrew.culham@constellation.com", "andrew.hlasko@constellation.com"
                ),
              sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to, 
    paste("Spec TCC PnL for ", as.character(asOfDate)), out)

  #------------------------------------------------------------------
  rLog("Calculate hedge PnL")
  #filename <- "S:/Trading/Nodal FTRs/NYISO/Auctions/awards.xlsx"
  filename <- "S:/Trading/NYTCC_Nodal/awards.xlsx"
  sheetName <- "Hedge"
  pnl <- .calculate_PnL( asOfDate, filename, sheetName=sheetName )
  
  out <- c(
    paste("Total TCC PnL next day:  ", dollar(sum(pnl$PnL.next_day))),         
    paste("Total TCC PnL this month:", dollar(sum(pnl$PnL.this_month))),
    paste("Total TCC PnL from start:", dollar(sum(pnl$PnL.from_start))),
     "\n\n",
    capture.output(print(pnl, row.names=FALSE)),
    "\n\nAwards file: S:/Trading/Nodal FTRs/NYISO/Auctions/awards.xlsx",       
    "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/daily_PnL_TCC",
    "\nContact: Adrian Dragulescu")

  rLog("Email results")
  to <- paste(c("adrian.dragulescu@constellation.com", "rujun.hu@constellation.com",
                "kevin.telford@constellation.com", "andrew.hlasko@constellation.com",
                "michael.flannery@constellation.com"
                ),
              sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to, 
    paste("Hedge TCC PnL for ", as.character(asOfDate)), out)
  

  
  rLog(paste("Done at ", Sys.time()))
  returnCode <- 0        # make it succeed.  I oon't need emails from IT!
  if( interactive() ){
    print( returnCode )
  } else {
    q( status = returnCode )
  }
}


main()


