# Calculate the nextMonth
#
#


currentMonth <- function( fromDate=Sys.Date() )
{
  as.Date(format(fromDate, "%Y-%m-01"))
}

nextMonth <- function( fromDate=Sys.Date() )
{
  bom <- as.Date(format(fromDate, "%Y-%m-01"))
  seq(bom, by="1 month", length.out=2)[2]
}

prevMonth <- function( fromDate=Sys.Date() )
{
  bom <- as.Date(format(fromDate, "%Y-%m-01"))
  seq(bom, by="-1 month", length.out=2)[2]
}

isWeekend <- function( x )
{
  wd <- weekdays(x, abbreviate=TRUE)
  ifelse(wd == "Sat" | wd == "Sun", TRUE, FALSE)
}
