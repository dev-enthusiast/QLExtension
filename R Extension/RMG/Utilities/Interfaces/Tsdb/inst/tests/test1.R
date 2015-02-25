#
#
#
#

require(CEGbase)
require(SecDb)


name <- "pm_nepool_trashme1"
path <- "T:/Data/pm/nepool/pm_nepool_trashme1.bin"
#path <- "T:/Data/pm/nepool"
description  <- "First try"
dataSource   <- "Imagination Inc." 
realTimeFlag <- TRUE   # 1 for hourly, 0 for daily data

res <- secdb.invoke("_lib elec matlab dispatch fns", "tsdb::create",
  name, path, realTimeFlag, description, dataSource)
res





rLog("Create a tsdb symbol ... ")
source("H:/user/R/RMG/Utilities/Interfaces/Tsdb/R/tsdbCreate.R")
res <- tsdbCreate(name, path, description, dataSource, realTimeFlag)


if (res==1) {
  rLog("Done")
} else {
  rLog("Failed")
}


rLog("Fill a tsdb symbol ... ")
name <- "pm_nepool_trashme"
times  <- seq(as.POSIXct("2012-01-01 01:00:00"), by="1 hour", length.out=10)
values <- 2*c(71.501127, 83.637090, 45.702816, 50.687329, 62.872875, 63.313160,
            61.210874, 69.387397, 63.585272, 40.147513)
x <- data.frame(time=times, value=values)
tsdbUpdate(name, x)


name <- "pm_nepool_second"
secdb.invoke("_lib r functions", "R::TsdbUpdate", name, as.list(as.numeric(times)),
             as.list(values))









name = "pm_nepool_second";
times  = [Time(1293868800), Time(1293872400)];
values = [45.70, 50.69];


curve <- matrix(c(times, values), ncol=2)

#source("H:/user/R/RMG/Utilities/Interfaces/Tsdb/R/tsdbUpdate.R")
#res <- tsdbUpdate(name, curve, fillDST=FALSE)


#secdb.setDatabase("TSDB")
aux <- list(Dates=times[3], Values=values[3])
res <- secdb.invoke("_lib elec matlab dispatch fns", "tsdb::update",
  name, aux)

# You can do it like this
secdb.evaluateSlang('TsdbUpdate("pm_nepool_second", TCurve([1293865200, 83.63]));')

secdb.evaluateSlang('TsdbUpdate("pm_nepool_second", TCurve([[1296568800, 1296572400], [83.63, 91.11]]));')

# test adding one month
times  <- seq(as.POSIXct("2011-04-01 01:00:00"), by="1 hour", length.out=8760)
values <- 200 + rnorm(length(times), 0, 10)

x <- data.frame(time=times, value=values)


cmd <- paste('TsdbUpdate("', name, '", TCurve([[',
  paste(as.numeric(times), sep="", collapse=", "), "], [",
  paste(sprintf("%f", values), sep="", collapse=", ")
  ,']]));', sep="")

res <- secdb.evaluateSlang(cmd)


