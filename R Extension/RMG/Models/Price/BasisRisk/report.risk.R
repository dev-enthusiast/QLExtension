# Put together a bunch of plots/tables output. 
# Where risk.mth is an array with dim=c(fMonths, zones, noSims)
#
# Written by Adrian Dragulescu on 17-Mar-2005


report.risk <- function(sim.spread, risk.mth, save, options){
  
  fMonths  <- options$fdata$months
  options$plot$ylab <- "Basis risk, M$"
  #------------------------------------------Price Spread by month/zone-----------
  OutT <- tapply(sim.spread$spread, list(sim.spread$yyyymm, sim.spread$zone), mean)
  filename <- paste(save$dir.tables, "average.spreads.csv", sep="")
  write.csv(OutT, filename)
  OutT <- tapply(sim.spread$spread, list(sim.spread$yyyymm, sim.spread$zone), quantile,
         probs=0.10)
  filename <- paste(save$dir.tables, "q10.spreads.csv", sep="")
  write.csv(OutT, filename)
  OutT <- tapply(sim.spread$spread, list(sim.spread$yyyymm, sim.spread$zone), quantile,
         probs=0.90)
  filename <- paste(save$dir.tables, "q90.spreads.csv", sep="")
  write.csv(OutT, filename)

  acc.types <- dimnames(risk.mth)[[4]]
  #------------------------------------------Risk by month-----------
  risk.region <- apply(risk.mth, c(1,3,4), sum, na.rm=T); (risk.region[,1,])
  risk.region <- risk.region/options$scale.factor
  options$total.line <- 1                   # calculate total risk
  for (accounting in acc.types){
    aux  <- stack(as.data.frame(t(risk.region[,,accounting])))
    data <- data.frame(value=aux$values, factor=aux$ind)
    options$plot$main <- paste(options$run.name, ", all zones", sep="")
    options$table$tex.caption <- paste("Basis risk quantiles by month for ", 
        "accounting type: ", accounting, ".", sep="")
    save$filename.table <- paste(save$dir.tables, "base_risk_by_month_", accounting, sep="")
    save$filename.plot <-  paste(save$dir.plots, "base_risk_by_month_", accounting, sep="")
    res <- quantiles.by.factor(data,save,options)
  }
  
  #------------------------------------------Risk by zone-----------
  risk.zone <- apply(risk.mth, c(2,3,4), sum, na.rm=T);  (risk.zone[,1,])
  risk.zone <- risk.zone/options$scale.factor
  rows.zero <- function(x){all(x==0)}
  ind <- which(apply(risk.zone, 1, rows.zero))
  if (length(ind)>0){risk.zone[ind,] <- 10^(-6)*rnorm(length(ind)*options$noSims)}
  
  for (accounting in acc.types){
    aux  <- stack(as.data.frame(t(risk.zone[,,accounting])))
    data <- data.frame(value=aux$values, factor=aux$ind)
    options$plot$main <- paste(options$run.name, ", ", options$fdata$range, sep="")
    options$table$tex.caption <- paste("Basis risk quantiles by zone for ",
        "accounting type: ", accounting, ".", sep="")    
    save$filename.table <- paste(save$dir.tables, "base_risk_by_zone_", accounting, sep="")
    save$filename.plot <-  paste(save$dir.plots, "base_risk_by_zone_", accounting, sep="")
    res <- quantiles.by.factor(data,save,options)
  }
  
  

}
