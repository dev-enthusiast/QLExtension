# Read the bal report from archive, not really run it.
#
#

###########################################################
#
.pull_bal_oneday <- function(day, bookName)
{

  if (day >= Sys.Date())
    stop("Day must be in the future!")
  
  code <- paste('Link( "_LIB Elec GAAP P&L Fns" );
Link( "_LIB Elec Bal Rpt For Trade Fns" );
Date = Date( "', format(day, "%d%b%y"), '" );
Books = Structure( "', bookName, '", 1 );

BalReportMap = GetThese = Results = Structure();

If( !@BAL TRADE RPT::GetBalReportMap( &BalReportMap ))
    debug;

ForEach( Item, BalReportMap.Order )
    GetThese[ Item ] = BalReportMap.Map[ Item ].Mnemonic;

If( !@GAAP PNL Fns::GetBalReportValues( Date, &Books, &GetThese, False, &Results ))
    debug;

Results;
', sep="")

  res <- secdb.evaluateSlang(code)

  if (length(res) == 0){
    NULL                   # for weekends, holidays
  } else {
    cbind(Period=as.Date(day), as.data.frame(res[[1]]))
  }
}

###########################################################
#
secdb.runBalReport <- function(days, bookName)
{
  res <- vector("list", length=length(days))
  
  for (d in seq_along(days)){
    rLog("Working on day", as.character(days[d]))
    res[[d]] <- .pull_bal_oneday(days[d], bookName)
  }
  res <- do.call(rbind, res)

  # format as the tap report
  rownames(res) <- NULL  
  res <- res[,c("Period", "Yesterday", "Prev.Mark", "Time", "Commod", "Locations",
    "FX.Spot", "Rates", "Vols", "Corrs", "Data.Fixs", "Qty.Fcst", "SubTotals",
    "Rolls", "New.Trades", "Expires", "Mismatch", "PNL", "Growth", "Today")]
  
  res
}


.test_runBalReport <- function()
{
  require(CEGbase)
  require(SecDb)
  
  days <- seq(as.Date("2011-07-28"), as.Date("2011-07-31"), by="1 day")
  bookName <- "CONGIBT"

  out <- secdb.runBalReport(days, bookName)
  
  
  
}
