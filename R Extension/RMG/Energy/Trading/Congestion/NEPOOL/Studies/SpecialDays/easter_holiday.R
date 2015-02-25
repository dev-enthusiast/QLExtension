
require(CEGbase)
require(SecDb)


### Easter Sunday ###
days <- as.Date(c("2003-04-20", "2004-04-11", "2005-03-27", 
  "2006-04-16", "2007-04-08", "2008-03-23", "2009-04-12",
  "2010-04-04", "2011-04-24", "2012-04-08", "2013-03-31"))

symbs <- c("nepool_smd_da_4000_lmp", "nepool_smd_rt_4000_lmp")
res <- FTR:::FTR.load.tsdb.symbols(symbs, as.Date("2003-01-01"), Sys.Date())
colnames(res) <- c("da", "rt")
res <- na.omit(res)

ind <- which(as.Date(format(index(res)-1, "%Y-%m-%d")) %in% days)
x <- res[ind,]
x <- cbind(time=index(x), as.data.frame(x))
rownames(x) <- NULL
x$year <- format(x$time, "%Y")
#x$hour <- as.numeric(format(x$time, "%H"))
x$diff <- x$da - x$rt

z <- cast(x, year ~ ., function(x){round(mean(x),2)}, value="diff")
names(z)[2] <- "7X24"

y <- subset(x, hour %in% 0:7)


z <- cast(x, year ~ ., function(x){round(mean(x),2)}, value="diff",
  subset=hour %in% c(0:7), fill=NA)
names(z)[2] <- "7X8"

z <- cast(x, year ~ ., function(x){round(mean(x),2)}, value="diff",
  subset=hour %in% c(8:23), fill=NA)
names(z)[2] <- "5X16"


