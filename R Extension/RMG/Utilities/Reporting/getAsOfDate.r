################################################################################
## Try the procmon date value.
## If that fails, check the R asOfDate.
## If that fails, use yesterday.
##
getAsOfDate <- function()
{
    runDate = Sys.getenv("PROCMON_RUNDATE")
    asOfDate = Sys.getenv("asOfDate")

    if( runDate != "" )
    {
        asOfDate = as.Date( runDate, format="%Y%m%d" )

    } else if( asOfDate != "" )
    {
        asOfDate = as.Date( asOfDate )

    } else
    {
        asOfDate = Sys.Date()-1
    }

    return( asOfDate )
}
