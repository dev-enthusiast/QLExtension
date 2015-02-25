# Keep here a bunch of plots and tables that justify the model 
# that was finaly chosen. 
#
# Written by Adrian Dragulescu on 4-Mar-2005


main.hist.analysis <- function(MM, dMM, sim.spread, save, options){

source(paste(save$source.dir, "Historical.Analysis/basic.tables.R"))

options$table$size <- "scriptsize"
s2Hub <- basic.tables(MM, save, options)

ind <- which(colnames(s2Hub) %in% c("year","month"))
ind <- (1:length(colnames(s2Hub)))[-ind]
X   <- s2Hub[,ind]
mMM <- bucket.ts(cbind(year=s2Hub$year, month=s2Hub$month, X), "monthly")

select.list(c("NEPOOL", "PJM"), title="Region")

seq(as.Date("2005-01-01"), as.Date("2005-01-01"), by="-1 month")


#-------------------------------------PLOTS---------------------------
uMonths <- as.Date(paste("1", rownames(mMM)), format="%d %b %Y")
# data.peak <- cbind(stack(mS.peak),
#                    label=rep(rownames(mS.peak), options$noZones))

# pk.offpk <- mS.peak-mS.offpeak
# matplot(pk.offpk)

# library(lattice)
# xyplot(values ~ label | ind, data=data)




# stk.mMM <- cbind(stack(mMM), month=rep(rownames(mMM), ncol(mMM)))
# library(lattice)
# xyplot(values ~ month | ind, data=stk.mMM,
#        layout=c(4,2),
#        xlab="Month",
#        ylab="Monthly spread to Hub, $")

# aux <- trellis.par.get()
# bkg.col <- trellis.par.get("background")
# bkg.col$col <- "white"
# trellis.par.set("background", bkg.col)
# xyplot(values ~ values | ind, data=stk.mMM,
#        panel=function(x,y){
#          NN <- length(x)
#          probF <- seq(1/(2*NN),1-1/(2*NN), length=NN)                 
#          panel.xyplot(qnorm(probF), sort(x), col="blue", pch=1)
#        },
#        layout=c(4,2),
#        xlab="Normal quantile",
#        ylab="Quantile of monthly spread to Hub, $")

}
