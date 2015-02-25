##############################################################################
test.termGetTerm <- function()
{
    library(RUnit)
    checkEquals( termFromDatePair("2008-01-01", "2008-01-01"), "JAN 08" )
    checkEquals( termFromDatePair("2010-02-01", "2010-02-28"), "FEB 10" )
    checkEquals( termFromDatePair("2008-01-01", "2008-12-31"), "CAL 08" )
    checkEquals( termFromDatePair("2008-01-01", "2008-02-29"), "WIN 08" )
    checkEquals( termFromDatePair("2008-11-01", "2009-03-31"), "WIN 08/09" )
    checkEquals( termFromDatePair("2008-03-01", "2008-04-30"), "SPR 08" )
    checkEquals( termFromDatePair("2008-07-01", "2008-08-31"), "SUM 08" )
    checkEquals( termFromDatePair("2008-04-01", "2008-10-31"), "SUM 08" )
    checkEquals( termFromDatePair("2008-05-01", "2008-09-30"), "SUM 08" )    
    checkEquals( termFromDatePair("2008-01-01", "2008-03-31"), "Q1 08" )
    checkEquals( termFromDatePair("2008-04-01", "2008-06-30"), "Q2 08" )
    checkEquals( termFromDatePair("2008-07-01", "2008-09-30"), "Q3 08" )
    checkEquals( termFromDatePair("2008-10-01", "2008-12-31"), "Q4 08" )
    checkEquals( termFromDatePair("2008-01-01", "2009-01-01"), "JAN 08/09" )
    
    starts = c("2009-01-01", "2010-04-01")
    ends = c("2009-12-31", "2010-06-30")
    checkEquals( termFromDatePair(starts, ends), c("CAL 09", "Q2 10") )
}
