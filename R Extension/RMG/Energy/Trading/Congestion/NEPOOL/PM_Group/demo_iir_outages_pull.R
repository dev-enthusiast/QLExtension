#
# Example of how to pull IIR data with R
#
#


require(CEGbase)
require(reshape)
require(FTR)
FTR.load.ISO.env("NEPOOL")


GO <- FTR.getGenerationOutagesIIR("ISNE",
  start.dt=as.Date("2012-05-01"), end.dt=as.Date("2012-05-31"))


head(GO)  # see a few

subset(GO, prim.fuel=="Natural Gas")  # pick the nat gas units

# show total mw out by state and fuel
cast(GO, unit.state + prim.fuel ~ ., sum, value="out.design")

write.csv(GO, file="C:/temp/genoutage.csv", row.names=FALSE)


# get historical outages and compare the average gen out by week 
OO <- FTR.load.tsdb.symbols("NEPOOL_MORN_GenOutOrRed",
  as.Date("2008-01-01"), Sys.Date())
OO2 <- data.frame(date=index(OO), genout=as.numeric(OO))
OO2 <- subset(OO2, format(date, "%m")=="05")  # only focus on May

OO2$day   <- as.numeric(format(OO2$date, "%d")) # day of the month
OO2$week  <- ifelse (OO2$day %in% 14:21, "week1",
  ifelse(OO2$day %in% 21:28, "week2", NA)) 
cast(OO2, week ~ ., mean, value="genout")  # average gen out by week


# compare the two periods
GO1 <- FTR.getGenerationOutagesIIR("ISNE",
  start.dt=as.Date("2012-05-14"), end.dt=as.Date("2012-05-21"))

GO2 <- FTR.getGenerationOutagesIIR("ISNE",
  start.dt=as.Date("2012-05-21"), end.dt=as.Date("2012-05-28"))

sum(GO1$out.design)

sum(GO2$out.design)









