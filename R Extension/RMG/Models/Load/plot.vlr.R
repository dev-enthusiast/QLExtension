# Plot the vlr monthly distribution
#
#
# Written by Adrian Dragulescu on 3-Jan-2004


plot.vlr <- function(vlr, save, options){

  source("C:/R/Work/MyLibrary/TimeFunctions/ymd.R")
  source("C:/R/Work/MyLibrary/Statistics/quantiles.by.month.R")
  require(xtable)
  
  monthsF   <- seq(as.Date(options$forecast$startDate),
               as.Date(options$forecast$endDate), by="month")
  monthsVLR <- as.Date(gsub("\\.","-",colnames(vlr)), format="%d-%b-%Y")
  ind <- which(monthsVLR %in% monthsF)
  vlr <- vlr[,ind]
  vlr.Stk     <- stack(vlr)
  vlr.Stk$ind <- as.Date(as.character(vlr.Stk$ind), format="%d.%b.%Y")
  aux <- ymd(vlr.Stk$ind)
  VLR <- cbind(aux, vlr=vlr.Stk$values)
  VLR[1:10,]
 
  options$label$mmmyy <- options$label.vlr$mmmyy
  options$probs <- c(0,0.01, 0.05,seq(0.1,0.9,0.1),0.95, 0.99, 1)
  OutT <- quantiles.by.month(cbind(VLR, x=VLR$vlr), options, save)
  caption  <- paste("Quantiles of VLR distribution by month for the load ", options$loadName,".", sep="")
  fileName <- paste(save$dir$Rtables,"vlr_quantiles.tex", sep="")
  OutT[,1] <- c(100*options$probs, "Mean")
  print(xtable(t(OutT), caption=caption), file=fileName, size="scriptsize")
 
  #-----------------------------------Violin plot--------------
  days  <- as.Date(paste(VLR$year, VLR$month, VLR$day, sep="-"))
  fdays   <- format(days, "%b%y"); uMonths <- unique(fdays)
  VLR$monthF <- factor(fdays, levels=uMonths)
  means <- OutT[nrow(OutT),2:(ncol(OutT)-1)] 

  require(Simple)
  fileName <- "Plots/violin_vlr.pdf"
  if (save$Analysis$all){pdf(fileName, width=7.25, heigh=6.5)}
  par(xaxt="n")
  cat(simple.violinplot(vlr ~ monthF, data=VLR, col="wheat"))
  par(xaxt="s")
  axis(1, at=1:length(uMonths), labels=uMonths, las=2, cex.axis=0.7)
  mtext("VLR distribution, $/MWh", side=2, line=2.2)
  mtext(options$loadName, side=3, line=0, font=2)
  points(1:length(uMonths), means, pch=20)
  if (save$Analysis$all){dev.off()}

  

  

}
