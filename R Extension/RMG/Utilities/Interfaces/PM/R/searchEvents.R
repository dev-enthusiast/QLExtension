# Search NEPOOL "events" from the weekly summary report
#
# 
#
# Written by Adrian Dragulescu on 16-Feb-2010


searchEvents <- function(token, startDt=as.Date("2005-01-01"),
  endDt=Sys.Date(), event="Day-Ahead Price Separation",
  days=NULL, read=TRUE, region="NEPOOL")
{
  if (region == "NEPOOL")
    search_events_nepool(token, startDt=startDt, endDt=endDt,
      event=event, days=days, read=TRUE)

}

.read_event_file_nepool <- function()
{
  fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/",
    "ISODatabase/NEPOOL/Reports/WeeklyMarketReport/",
    "weekly_market_summary.csv", sep="")
  EE <- read.csv(fname)
  EE$startDate <- as.Date(EE$startDate, "%Y-%m-%d")
  EE$endDate   <- as.Date(EE$endDate, "%Y-%m-%d")
  EE
}


search_events_nepool <- function(token, startDt=as.Date("2005-01-01"),
  endDt=Sys.Date(), event="Day-Ahead Price Separation", days=NULL, read=TRUE)
{
  if (!exists(".NEPOOL_EVENTS")){
    .NEPOOL_EVENTS <<- .read_event_file_nepool()  # cache it for performance ... 
  } 

  EE <- get(".NEPOOL_EVENTS")

  ind <- rep(TRUE, length=nrow(EE))
  if (!is.null(event))
    ind <- ind & EE$event == event

  if (is.null(days)){
    if (!is.null(startDt))
      ind <- ind & EE$startDate >= startDt
    if (!is.null(endDt))
      ind <- ind & EE$endDate <= endDt
  } else {
    for (day in days){
      #browser()
      ind <- ind & (EE$startDate <= as.Date(day) & EE$endDate >= as.Date(day))
    }
  }

  if (!missing(token))
    ind <- ind & grepl(token, EE$comment)

  # subset here 
  SS <- EE[ind,]

  .fmt_one_row <- function(x){
    y <- paste(x["event"], " from ", x["startDate"], " to ", 
     x["endDate"], " ", x["comment"], sep="")
    text_wrap(y)
  }
  
  if (read){
#    browser()
    out <- apply(SS, 1, .fmt_one_row)
    out <- paste(out, sep="", collapse="\n\n")
    cat(out, "\n\n")
  }
  
  invisible(out)
  
}


