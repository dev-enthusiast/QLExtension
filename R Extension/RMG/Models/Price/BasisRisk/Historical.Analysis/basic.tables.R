# Basic historical tables 
#
#
# Written by Adrian Dragulescu on 19-Apr-2005

basic.tables <- function(MM, save, options){

#------------------------------------HUB statistics--------------
data <- cbind(subset(MM, select=c("year","month","day","hour")), x=MM$Hub)
data.peak    <- select.onpeak(data)
data.offpeak <- data[-data.peak$ind.PEAK,]
ind.PEAK     <- data.peak$ind.PEAK            # save it for reference

save$filename <- paste(save$dir.model,"hist_avg_hub_7x24", sep="")
options$table$tex.caption <- "Average 7x24 Hub price."
res  <- mean.by.month(data, options, save)
save$filename <- paste(save$dir.model,"hist_avg_hub_peak", sep="")
options$table$tex.caption <- "Average peak Hub price."
res  <- mean.by.month(data.peak, options, save)
save$filename <- paste(save$dir.model,"hist_avg_hub_offpeak", sep="")
options$table$tex.caption <- "Average offpeak Hub price."
res  <- mean.by.month(data.offpeak, options, save)

#------------------------------------SPREAD statistics--------------
ind   <- which(colnames(MM) %in% c("year","month","day","hour"))
ind   <- (1:length(colnames(MM)))[-ind]
s2Hub <- MM[,ind]-MM[,ind[1]]; s2Hub <- s2Hub[,-1]  # spread to Hub

data <- cbind(year=MM$year, month=MM$month, s2Hub)
mS   <- bucket.ts(data, "monthly")
fileName <- paste(save$dir.model,"hist_avg_spread-to-hub_7x24.tex", sep="")
print.xtable(xtable(round(mS,2), caption="Historical 7x24 spread (\\$) to hub by month."), file=fileName, size=options$table.size)
fileName <- paste(save$dir.model,"hist_avg_spread-to-hub_7x24.csv", sep="")
aux <- cbind(month=rownames(mS), round(mS,2))
write.table(aux, file=fileName, sep=",", row.names=F)

mS.peak  <- bucket.ts(data[ind.PEAK,], "monthly")
fileName <- paste(save$dir.model,"hist_avg_spread-to-hub_peak.tex", sep="")
print.xtable(xtable(round(mS.peak,2), caption="Historical peak spread (\\$) to hub by month."), file=fileName, size=options$table.size)
fileName <- paste(save$dir.model,"hist_avg_spread-to-hub_peak.csv", sep="")
aux <- cbind(month=rownames(mS.peak), round(mS.peak,2))
write.table(aux, file=fileName, sep=",", row.names=F)

mS.offpeak <- bucket.ts(data[-ind.PEAK,], "monthly")
fileName <- paste(save$dir.model,"hist_avg_spread-to-hub_offpeak.tex", sep="")
print.xtable(xtable(round(mS.offpeak,2), caption="Historical offpeak spread (\\$) to hub by month."), file=fileName, size=options$table.size)
fileName <- paste(save$dir.model,"hist_avg_spread-to-hub_offpeak.csv", sep="")
aux <- cbind(month=rownames(mS.offpeak), round(mS.offpeak,2))
write.table(aux, file=fileName, sep=",", row.names=F)

pk.offpk <- mS.peak-mS.offpeak
fileName <- paste(save$dir.model,"hist_avg_peak-offpeak.tex", sep="")
print.xtable(xtable(round(pk.offpk,2), caption="Historical peak spread to hub - offpeak spread to hub (\\$) by month."), file=fileName, size=options$table.size)
fileName <- paste(save$dir.model,"hist_avg_peak-offpeak.csv", sep="")
aux <- cbind(month=rownames(pk.offpk), round(pk.offpk,2))
write.table(aux, file=fileName, sep=",", row.names=F)

return(data)
}
