# Calculate the historical payoff for a portfolio of FTR's. 
#
#
# Written by Adrian Dragulescu on 1-Sep-2005

historical.path.Payoff <- function(hdata, data, Position, options){

  probFun <- function(xHist, x){
  # Calculate the probability of the bid using prior realized payoffs 
    y <- array(0, dim=length(x))
    y[x>max(xHist)] <- 1
    ind <- (x<=max(xHist))&(x>=min(xHist))
    if (any(ind)){
      y[ind] <- approx(sort(xHist), seq(0, 1, length=length(xHist)), x[ind])$y}
    return(y)
  }
  
  if ("PEAK" %in% unique(Position$Bucket)){
    ind.PEAK <- select.onpeak(hdata[,1:4])$ind}
  if ("OFFPEAK" %in% unique(Position$Bucket)){
    ind.OFFPEAK <- select.offpeak(hdata[,1:4])$ind}
  if ("FLAT" %in% unique(Position$Bucket)){
    ind.FLAT <- 1:nrow(hdata)}
  dates  <- as.Date(paste(hdata$year, hdata$month, hdata$day, sep="-"))
  yyyymm <- format(dates, "%b.%y")
  yyyymm <- factor(yyyymm, levels=unique(yyyymm))

  bid.Probability=std=mu=q95=q05 <- array(NA, dim=length(options$paths))
  hpayoff <- matrix(NA, nrow=length(options$paths),
    ncol=length(levels(yyyymm)), dimnames=list(NULL, levels(yyyymm)))
  for (r in 1:nrow(Position)){  # Loop over unique positions
    ind.POI <- which(options$uNodes == Position[r,"Source"]) 
    ind.POW <- which(options$uNodes == Position[r,"Sink"])
    ind <- switch(as.character(Position$Bucket[r]), FLAT = ind.FLAT,
           PEAK = ind.PEAK, OFFPEAK = ind.OFFPEAK)
    hspread <- hdata[,ind.POW+4] - hdata[,ind.POI+4]
    hp      <- tapply(hspread[ind], yyyymm[ind], mean, na.rm=T)
    
    ind <- which(options$paths %in% options$uPaths[r])
    bid.Probability[ind] <- probFun(unique(hp[is.finite(hp)]), data$BidPrice[ind])
    mu[ind] <- mean(hp, na.rm=T); std[ind] <- sd(hp, na.rm=T)
    hpayoff[ind,] <- rep(hp, each=length(ind))
    q95[ind] <- round(quantile(hspread, probs=c(0.95), na.rm=T),2)
    q05[ind] <- round(quantile(hspread, probs=c(0.05), na.rm=T),2)
  }
  #----------------------------------------------Aggregate positions-----
  Position$Source <- as.character(Position$Source)
  Position$Sink   <- as.character(Position$Sink)
  Position$Bucket <- as.character(Position$Bucket)  
  sPosition <- data.frame(Symbols = c(Position$Sink, Position$Source),
                          MW      = c(Position$MW, -Position$MW),
                          Bucket  = c(Position$Bucket, Position$Bucket))
  for (m in as.character(unique(yyyymm))){
    

  }
  OutT <- data.frame(path=options$paths,
     Bid.Probability=round(bid.Probability,2), Hist.Mean=round(mu,2), 
     Hist.Std=round(std,2), Sharpe=round((mu-data$BidPrice)/std,2),
     q05, q95, Bid.Price=data$BidPrice, round(hpayoff, 2))
  filename <- paste(save$dir, "hist.analysis.csv", sep="")
  write.csv(OutT, file=filename)
  
  return(OutT)
}


# historical.portfolio.Payoff <- function(hdata, data, save, options){

#   dates  <- as.Date(paste(hdata$year, hdata$month, hdata$day, sep="-"))
#   yyyymm <- format(dates, "%b.%y")
#   yyyymm <- factor(yyyymm, levels=unique(yyyymm))
#   last.hdate  <- dates[length(dates)]
#   bom <- as.Date(paste(format(last.hdate, "%Y-%m"), "-01", sep=""))
#   last.1m <- seq(bom, by="-1 month", length.out=2)[2]
#   last.3m <- seq(bom, by="-1 month", length.out=4)[4]
#   last.6m <- seq(bom, by="-1 month", length.out=7)[7]
  
# }




















#   TMM<-hdata
#   TMM$DateTime<-ISOdatetime(TMM[,"year"],TMM[,"month"],TMM[,"day"],TMM[,"hour"],0,0)
#   DTime <- intersect(TMM$DateTime,fhours)
#   Dindex <- which (TMM$DateTime %in% DTime)
#   RMM<-TMM[Dindex,]

#   classes <- c("OFFPEAK", "PEAK", "FLAT")
#   for (c in classes)
#   {
#     ind <- grep(c, uPaths)
#     if (length(ind)==0){next}
#     ind.Nodes <- unique(c(as.character(Position[ind,"Source"]), as.character(Position[ind, "Sink"])))
#     ind <- which(uNodes %in% as.numeric(ind.Nodes))
#     HMM <- RMM[,c(1:4,4+ind)]
#     if (c=="OFFPEAK"){ HMM <- select.offpeak(HMM) }
#     if (c=="PEAK"){ HMM <- select.onpeak(HMM)  }

#     hist.prices[[c]] <- HMM[,-c(1:4,length(HMM))]
#   }

#   realized.prices.FTR = list()
#   realized.prices.FTR$OFFPEAK <- apply(hist.prices$OFFPEAK,2,sum,na.rm = T)
#   realized.prices.FTR$PEAK <- apply(hist.prices$PEAK,2,sum,na.rm = T)

#   for (index in 1:dim(Position)[1])
#   {
#        FTRClass <- as.character(Position[index,"Class"])
#        ind.Source <- which( names(realized.prices.FTR[[FTRClass]]) ==
#                             paste("NEPOOL","SMD","DA",as.character(Position[index,"Source"]),"CongComp",sep="_"))
#        ind.Sink <- which( names(realized.prices.FTR[[FTRClass]]) ==
#                             paste("NEPOOL","SMD","DA",as.character(Position[index,"Sink"]),"CongComp",sep="_"))
#        Position[index,"Realized.Price"] <- realized.prices.FTR[[FTRClass]][ind.Sink] -  realized.prices.FTR[[FTRClass]][ind.Source]
#   }
