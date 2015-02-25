# A wrapper for the ntplot function in SecDb
#
#

ntplot.rtrhq <- function(portfolio, filter="Fcst", datasource="Archive")
{
  df <- expand.grid(portfolio, filter)

  res <- vector("list", nrow(df))

  for (i in 1:nrow(df)) {
    res[[i]] <- cbind(portfolio=df[i,1], filter=df[i,2],
      as.data.frame(secdb.invoke("_lib ntplot functions", "NTPLOT::RTRHQ",
      as.character(df[i,1]), as.character(df[i,2]), as.character(datasource))))
  }

  if (nrow(df)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)
  
  res
}

.test_rtrhq <- function()
{
  portfolio  <- "nepool load portfolio"
  filter     <- c("Region=NEPOOL;Zone=CT", "Region=NEPOOL;Zone=NEMA")
  datasource <- "Archive"

  res <- ntplot.rtrhq(portfolio, filter)
  
  
}

## secdb.evaluateSlang('Link("_lib NTPLOT Functions");')
##     slangCode <- paste('@NTPLOT::BucketizeByMonth("', df[r,1],
##                        '", "', df[r,2], '");', sep="") 
##     res[[r]]  <- cbind(symbol=df[r,1], bucket=df[r,2],
##                        data.frame(secdb.evaluateSlang(slangCode)))
