# Please paste the file "FC.PCA.Fncs.r" in the following directory or make the necessary changes!!!
source("H:/user/R/RMG/Models/Price/ForwardCurve/MultiFactor/FC.PCA.Fncs.r")         #Was: "S:/Risk/Temporary/PCA/FC.PCA.Fncs.r"  Changed at 10/26/2006 9:13:34 AM

# Details for each function inputs and outputs are provided in the file "FC.PCA.Fncs.r"

# Obtained historical returns, recent forward curve and its corresponding date
hist.data <- get.hist.FwdCurve(commodity1="Close of NG",start.mo.FC=1, start.year.FC=2007, num.months=36, start.date.hist="05/31/2003", Verbose=FALSE, ShowPrices=FALSE)
                             
# Create a sequence of pricing dates
pricing.dates = seq(as.Date(hist.data[[2]])+1,by="day",length.out=as.numeric(as.Date("2009-12-31")-as.Date(hist.data[[2]])))

# Conduct a Principal Component Analysis
PC <- PC.analysis(pricing.dates, seasons=list(noSeas=3, months.in.season=list(c(5:9),c(11,12,1:3),c(4,10))) , hist.returns=hist.data[[1]])

# Plot the factor loadings (volatility functions as defined in Clewlow and Strickland)  based on the variation plot
# Let's say we plot the first four factors for each season!!

no.seas = dim(PC[[1]])[3]
noFac=rep(4,no.seas)
par(mfrow=c(no.seas,1))

for( i in 1:no.seas){
  y.range = range(PC[[1]][,1:noFac[i],i])
  color = c("red", "green", "mediumturquoise","coral","darkorchid","black")
  #windows()
  plot(PC[[1]][,1,i],xlab="Maturity (Months out)", ylab="Factor Loadings", ylim=y.range, type="l", lty=1, lwd=2,col=color[1],
  main=paste("The First ",noFac[i],"PC of the hist. fwd Curve for season",i,sep=""))
  for( j in 2:noFac[i]) {lines(1:nrow(PC[[1]][,,i]), PC[[1]][,j,i], col=color[j], lty=1, lwd=2)}

  legend(cex=0.6,x="topleft", inset=0, legend=paste("Factor",1:noFac[i],sep=""), lty=rep(1,noFac[i]),lwd=rep(2,noFac[i]), col=color[1:noFac[i]]
  ,text.col=color[1:noFac[i]])
}  
  
# Simulate the forward curve based on PCA

MarketData <- options <- NULL
MarketData$asOfDate <- as.Date(hist.data[[2]])
MarketData$contract.dt <- colnames(hist.data[[1]])
MarketData$pricing.dt <- pricing.dates
MarketData$forward.prices <- hist.data[[3]]
options$noSims <- 100
options$noFac <- 3

# Simulate the Fwd Curve
res = simulate.Futures.PC(MarketData, PC, options)

# Let's plot the first DD days of the first N simulations 
N=9
DD=length(pricing.dates)
color = c("red", "green", "mediumturquoise","coral","darkorchid","black", "blue", "darkgray", "hotpink", "chocolate", "lightsalmon")
par(mfrow=c(3,3))
for( i in 1:N){
  y.range = range(res[1:DD,,i])
  #windows()
  plot(res[1,,i], type="l",xlab="Maturity (Months out)", ylab="Sim Prices", lty=1, lwd=1, col=color[1], main=" Simulated Fwd Curve")
  
  for(j in 2:DD) {    #
    lines(1:ncol(res[,,1]), res[j,,i], col=color[j], lty=1, lwd=1)
  }

  legend(cex=0.6,x="topleft", inset=0, legend=format(as.Date(rownames(res[,,i])), "%d%b")[1:DD], lty=1,lwd=1, col=color[1:DD] ,text.col=color[1:DD])
  #
} 


   ## Make a 3D Plot for one simulation 
    require(TeachingDemos)
    TS_FC = t(as.matrix(res[1:DD,,16]))
  
  TS_FC_rot =  TS_FC[,sort(1:ncol(TS_FC), decreasing = TRUE)]
  if(ncol(TS_FC_rot)<=6){y.denom <- ncol(TS_FC_rot)}else{y.denom<-6}
  if(nrow(TS_FC_rot)<=6){x.denom <- nrow(TS_FC_rot)}else{x.denom <-6}
  
  print(wireframe(TS_FC_rot, scales =list(cex=0.8,distance = c(2,10,2),x=list(arrows=FALSE, at=seq(1,nrow(TS_FC_rot),by=floor(nrow(TS_FC_rot)/x.denom)),
  labels=c(rownames(TS_FC_rot)[seq(1,nrow(TS_FC_rot),by=floor(nrow(TS_FC_rot)/x.denom))]),rot=0)
  ,y=list(arrows=FALSE,cex=0.8, tck=.5, at= seq(1,ncol(TS_FC_rot),floor(ncol(TS_FC_rot)/y.denom)), labels=c(colnames(TS_FC_rot)[seq(1,ncol(TS_FC_rot),floor(ncol(TS_FC_rot)/y.denom))]), rot=45)
  ,z=list(arrows=FALSE)),ylab="Contracts",xlab="", zlab="", drape=TRUE, main="Simulations in 3D")   )
  
  
