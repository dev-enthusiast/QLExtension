##############################################################################
test.termFromReutersCode <- function()
{
    library(RUnit)
    checkEquals( termFromReutersCode("F08"), as.Date("2008-01-01") )
    checkEquals( termFromReutersCode("G10"), as.Date("2010-02-01") )
    checkEquals( termFromReutersCode("M 09"), as.Date("2009-06-01") )
    checkEquals( termFromReutersCode("Z 12"), as.Date("2012-12-01") )
    checkEquals( termFromReutersCode("Z"), as.Date(format(Sys.Date(), "%Y-12-01")) )
    checkEquals( termFromReutersCode("H"), as.Date(format(Sys.Date(), "%Y-03-01")) )
    
    checkEquals( termFromReutersCode(c("H", "M09", "G 10")), 
            c( as.Date(format(Sys.Date(), "%Y-03-01")),
                    as.Date("2009-06-01"), as.Date("2010-02-01")) )
}
