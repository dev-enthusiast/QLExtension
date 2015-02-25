#
#
#

# You should consider uncommenting the line below
#Sys.setenv(TZ="")

test.tsdb.readCurve <- function()
{

  if (!("package:SecDb" %in% search()))
    require(SecDb)
  
  # check daylight savings time; hour 02:00:00 should be missing.
  res <- tsdb.readCurve("nepool_smd_da_4000_lmp",
    as.POSIXct("2008-03-09 00:00:00"), as.POSIXct("2008-03-09 23:00:00"))
  current <- round(res$value[1:5],2)
  target  <- c(69.26, 66.75, 66.74, 58.96, 58.47)
  checkEquals(current, target)

}





