#
#
#
#


base.dir <- "S:/Risk/2006,Q3/Risk.Analytics/Historical.PnL/"
setwd(base.dir)
fileName <- paste(base.dir, "reports/mgmt/",
   "Daily_PNL_Estimate_2006-01-01_2006-06-30.csv", sep="")
MM <- read.csv(fileName)

MM$PNL <- as.numeric(gsub(",","",as.character(MM$PNL)))
MM <- MM[,1:4]        # drop the comments for now
MM <- na.omit(MM)     # remove missing PnL data

MM$Trader <- sub("[\n\n\f\r ]*$", "", as.character(MM$Trader))
unique(MM$Trader)
pnl <- tapply(MM$PNL, list(MM$Date, MM$Trader), mean, na.rm=T)
write.csv(pnl, "results.csv")

nobs <- table(MM$Trader)
std <- apply(pnl, 2, sd, na.rm=T)
mu  <- apply(pnl, 2, mean, na.rm=T)
sharpe <- mu/std
res <- data.frame(nobs, mu=mu, std=std, sharpe)
res <- res[order(sharpe, decreasing=T),]
rownames(res) <- 1:nrow(res)
colnames(res)[1] <- "Trader"



plot(mu, std)
identify(mu, std, labels=names(mu))

matplot(as.Date(rownames(pnl)), pnl)


pnl <- pnl

write.csv(pnl.trader, "results.csv")



pnl.Portfolio <- tapply(MM$PNL, list(MM$Date, MM$Portfolio), sum, na.rm=T)




write.csv(pnl.trader, "results.csv")




subset()
