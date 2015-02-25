# Moved the bootstrap analysis outside analyze.one.spread.R
#
#
# Written by Adrian Dragulescu on 17-Feb-2005

bootstrap.spread <- function(MM, save, options){

  no.sims         <- 10000   # number of simulations
  days.in.month   <- 31
  #-----------------------------bootstrap the hourly spread-----
  avg.spread.monthly <- matrix(NA, ncol=12, nrow=no.sims)
  colnames(avg.spread.monthly) <- month.abb
  for (m in 1:12){
    ind    <- which(MM[,"month"]==m)
    spread <- MM[ind,"spread.fc"]
    sim.spread <- sample(spread, days.in.month*no.sims, replace=TRUE)
    sim.spread <- matrix(sim.spread, ncol=no.sims, nrow=days.in.month)
    avg.spread.monthly[,m] <- apply(sim.spread, 2, mean, na.rm=T) 
  }
  avg.spread.yearly <- apply(avg.spread.monthly, 1, mean)

  #------------- Quantile table of bootstraped spread-----------
  aux   <- cbind(avg.spread.monthly, Year=avg.spread.yearly)
  probs <- c(0,0.05,seq(0.1,0.9,0.1),0.95,1) 
  OutT=NULL
  for (c in 1:ncol(aux)){
     OutT <- cbind(OutT, quantile(aux[,c], probs=probs, na.rm=T))
   }
  colnames(OutT) <- c(month.abb, "Year")
  OutT <- round(as.data.frame(OutT),3)
  OutT <- cbind(Quantile=rownames(OutT), OutT)
  rownames(OutT) <- c(1:13)
  fileName <- paste(save$dir, "bootstrap.quantiles.",options$main,
                    ".csv", sep="")
  write.table(OutT, fileName, sep=",", row.names=FALSE)
  caption  <- paste("Bootstraped monthly spread ", options$between,
    " based on historical data from ", options$dateRange, " using ",
    no.sims, " simulations.", sep="")
  fileName <- paste(save$dir,"bootstrap.tex", sep="")
  OutT[,1] <- gsub("%","",as.character(OutT[,1]))
  print(xtable(OutT, caption=caption), file=fileName, size="small")

  #-----------------------------------Violin plot--------------
  require(Simple)
  fileName <- "violin_Bootstraped_Spread.pdf"
  if (save$Analysis$all){pdf(fileName, width=7.25, heigh=6.5)}
  DF <- data.frame(spread=as.numeric(avg.spread.monthly),
                 month=rep(1:12, each=no.sims))
  cat(simple.violinplot(DF$spread ~ DF$month, col="wheat"))
  mtext("Month of the year", side=1, line=2.2)
  mtext("Bootstraped monthly spread, $ ", side=2, line=2.2)
  mtext(paste(options$between,"", sep=""),
        side=3, line=0, font=2)
  points(1:12, apply(avg.spread.monthly,2,mean), pch=20)
  if (save$Analysis$all){dev.off()}

    #------------- Quantile table of bootstraped spread for DEAL-----------
  if ((length(options$deal.start)!=0)&(length(options$deal.end)!=0)){
     options$dealRange <- paste(format(options$deal.start, "%b%y"), "to",
                           format(options$deal.end, "%b%y")) 
     value  <- matrix(0, ncol=no.sims, nrow=1)
     months <- seq(options$deal.start, options$deal.end, by="month")
     for (month in 1:length(months)){
       m <- as.numeric(format(months[month],"%m"))
       ind    <- which(MM[,"month"]==m)
       spread <- MM[ind,"spread.fc"]
       sim.spread <- sample(spread, days.in.month*no.sims, replace=TRUE)
       sim.spread <- matrix(sim.spread, ncol=no.sims, nrow=days.in.month) 
       ind <- which(sim.spread<0, arr.ind=TRUE)
       sim.spread[ind] <- 0
       value <- value + apply(sim.spread, 2, mean)
     }
     value <- value/length(months)
     OutT <- round(quantile(value, probs=probs, na.rm=T), digits=2)
     OutT <- as.data.frame(OutT)
     OutT <- cbind(100*probs, OutT)
     colnames(OutT) <- c("Quantile", "Deal Value")
     rownames(OutT) <- 1:nrow(OutT)
     fileName <- paste(save$dir, "bootstrap.values.",options$main,
                    ".csv", sep="")
      write.table(OutT, fileName, sep=",", row.names=FALSE)
      caption  <- paste("Bootstraped value of the deal ", options$between, " from ",
         options$dealRange, " based on historical data from ", options$dateRange,
         ".", sep="")
      fileName <- paste(save$dir,"bootstrap_value.tex", sep="")
      print(xtable(OutT, caption=caption), file=fileName, size="small")
   }
}
