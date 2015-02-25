################################################################################
## This utility has two parts: Plotting for simulated prices for future times,
## and plotting for backtesting. There are two other functions that can be used 
## in the future: to plot multi curve and to plot heat rate.
## The plots include:
##     1. A randomly chosen daily price simulation for the daily simulation period (usually 2 months
##        after AOD.
##     2. A randomly chosen price simulation agains all contract months
##     3. Quantiles of price simulation for some chosen contract months
##     4. Violin plots(this has problem for some FRT, FRC curves so I turned them off)
##     5. Price changes comparision with histroical price changes
## Note that for backtesting, historical data are plotted against simulated data for comparison
## Also note, PE plots are done in seperate codes(e.g. H:\user\R\RMG\Energy\VaR\PE\PEInteractiveUtils.R)     
#################################################################################


#####################################################
## smooth Scatter plot. I stole it from geneplotter
## package in BioConductor and made some modifications.
## I don't want to install the package because it
## involves many other packages.
## 
#####################################################

smoothScatter <- function(x, y, colormap=colorRampPalette(c("white","blue")),
                          add=TRUE) {
  ## calculate 2D density
  require(KernSmooth)
  xy <- cbind(x,y)
  ## calculate smoothing bandwidth
  bw <- diff(apply(xy, 2, quantile, probs = c(0.05, 0.95), na.rm = TRUE))/20
  map <- bkde2D(xy, bandwidth=bw, grid=c(75,75))

  ## generate plot
  xm <- map$x1
  ym <- map$x2
  dens <- map$fhat
  image(xm, ym, z = dens, col = colormap(256), add=add)
}

  
#####################################################
## function to plot one curve
#####################################################
plot.OneCurve <- function(curvename, FP.options, type=c("pdf", "png")) {
  type <- match.arg(type)
  library(lattice)
  if (file.access(FP.options$report.dir)!=0){mkdir(FP.options$report.dir)}
  if(is.null(FP.options$simCurves.report.dir)){
        FP.options$simCurves.report.dir = paste(FP.options$report.dir,"simulatedCurves/", sep="")
    }
  if(file.access(FP.options$simCurves.report.dir)!=0){mkdir(FP.options$simCurves.report.dir)}
  
  ##---------------------------------------------------------------------------
  ## load simulated price
  FP.options$sPtmp.dir = paste(FP.options$simCurves.report.dir,"SimPriceTemp/", sep="")
  if (file.access(FP.options$sPtmp.dir)!=0){mkdir(FP.options$sPtmp.dir)}
  from.dir <-paste(FP.options$targetdata.dir,tolower(curvename), sep ="")
  to.dir <- paste(FP.options$sPtmp.dir,"/", tolower(curvename), sep="")
  filecopy <- file.copy(from.dir, to.dir, overwrite=TRUE)
  if(filecopy){
    sPtemp.dir <-substr(FP.options$sPtmp.dir, 1, nchar(FP.options$sPtmp.dir)-1)
    sPP.env <- .getFileHashEnv(sPtemp.dir)
  }else
  { rLog("Simulated price file cannot be copied! Check if they have been simulated.")
    return()
   }
  res <- sPP.env[[tolower(curvename)]]#t x M x N matrix
  file.remove(to.dir)
  
  ##---------------------------------------------------------------------------
  ## load historical price
  loptions <- NULL
  loptions$asOfDate <- FP.options$asOfDate
  loptions$calc$hdays <- seq(FP.options$asOfDate - FP.options$N.hist,
                             FP.options$asOfDate, by="day")  
  QQ <- data.frame(curve.name=curvename, contract.dt=FP.options$contract.dts)
  
  #take out historical data for this curve to a temp folder
  FP.options$hPtmp.dir = paste(FP.options$simCurves.report.dir,"HistPriceTemp/", sep="")
  if (file.access(FP.options$hPtmp.dir)!=0){mkdir(FP.options$hPtmp.dir)}
  from.dir <-paste("//nas-msw-07/rmgapp/Prices/Historical","/",tolower(curvename), sep ="")
  to.dir <- paste(FP.options$hPtmp.dir,"/", tolower(curvename), sep="")
  filecopy <- file.copy(from.dir, to.dir, overwrite=TRUE)
  if(filecopy){
    hPtemp.dir <-substr(FP.options$hPtmp.dir, 1, nchar(FP.options$hPtmp.dir)-1)
    hPP.env <- .getFileHashEnv(hPtemp.dir)
  }else
  { rLog("Historical price file cannot be copied!")
    return()
   }
  hp <- try(get.prices.from.R(QQ, loptions, hPP.env))  # get prices from the hash 
  #remove file from the environment after historical data are obtained
  file.remove(to.dir)
  
  hp.thiscurve <- hp[, 3:ncol(hp)]
  rownames(hp.thiscurve) <- hp[,"contract.dt"]
  tmp <- diff.bday(FP.options$sim.days)
  T <- length(which(tmp==1)) + 1
  D <- dim(res)[2]
  
  ##----------------------------------------------------------------------------
  ## 1. Simulation prices for all simulation, each month per season
  ## only plot forward price for daily dates
  if(type=="pdf") {
    filename <- paste(FP.options$simCurves.report.dir, curvename, ".dailysims.pdf", sep="")
    pdf(filename, width=5.5, height=8, pointsize=8)
  } else if (type=="png") {
    filename <- paste(FP.options$SimCurves.report.dir, curvename, ".dailysims.png", sep="")
    png(filename, width=550, height=800)
  }
  layout(1:4)
  par(mar=c(4.5, 4.5, 3,2))

  ## get month index for Jan, Apr, Jul, Oct
  allmon <- as.integer(format(as.Date(colnames(res)), "%m"))
  mon.plot <- c(1,4,7,10)         # plot Jan, Apr, Jul, Oct
  #--------------------------------------------------------------------------
  # plot recent historical data vs short term forecast

  for(i in 1:length(mon.plot)) {
    idx.mon <- which(allmon==mon.plot[i])[1] # get the first sim Jan/Apr/Jul/Oct 
    ## simulation for this month
    dat <- res[1:T,idx.mon,]
    ## remove NAs                       
    dat <- dat[!is.na(dat[,1]),]
    ## historical for this month
    thishp <- as.numeric(hp.thiscurve[colnames(res)[idx.mon],])
    ## only plot last 40 historical days
    thishp <- thishp[(-39:0)+length(thishp)]
    ## start plotting
    ylim <- range(dat, thishp, na.rm=T)
    xlim <- c(0, length(thishp)+dim(dat)[1])
    plot(thishp, xlim=xlim, ylim=ylim, type="l", lwd=2, xlab="", ylab="",
         main=paste(curvename, colnames(res)[idx.mon]))
    xcoord <- length(thishp)+(1:dim(dat)[1])
    ## smoothScatter for all simulations 
    smoothScatter(rep(xcoord, FP.options$nsim), as.vector(dat))
    ## sometimes smooth scatter will erase part of the line so I'll redraw the line
    lines(thishp, lwd=2)
    ## some lines 
    apply(dat[,sample(FP.options$nsim, 5)], 2, function(x) lines(xcoord, x, col="darkgray"))
    quantiles<-apply(dat,1, quantile, c(0.50, 0.95,0.99))
    apply(t(quantiles), 2, function(x) lines(xcoord, x, col="red"))
  }
  graphics.off()
  
 
  ##--------------------------------------------------------------------------
  ## 3. Put all months together for a randomly chosen simulations
  idx.sim <- sample(FP.options$nsim, 1)
  if(type=="pdf") {
    filename <- paste(FP.options$simCurves.report.dir, curvename, ".allmon.pdf", sep="")
    pdf(filename, width=8, height=5.5, pointsize=8)
  } else if(type=="png") {
    filename <- paste(FP.options$simCurves.report.dir, curvename, ".allmon.png", sep="")
    png(filename, width=550, height=800)
  }
#  layout(1:4) #matrix(1:8,ncol=2, byrow=T))
  layout(matrix(1:4, 2, 2)); #layout.show(4)
  par(mar=c(4.5, 4.5, 3,2))
  ## simulations result
  dat <- res[,,idx.sim]
  matplot(dat, type="l", xlab="Pricing date, t=0 is asOfDate", ylab="",
    cex.main=1, main=paste(curvename, ", simulation", idx.sim, sep=""))
  abline(v=T+0.5)   # where the monthly contracts start
  matplot(t(dat), type="l", xlab="Contract month", ylab="", cex.main=1, 
          main=paste(curvename, ", simulation", idx.sim, sep=""))
  ## historical
  matplot(t(hp.thiscurve), type="l", xlab=paste("Pricing date, t=",
    ncol(hp.thiscurve)," is asOfDate", sep=""), ylab="",
    main=paste(curvename, "historical"))
  matplot(hp.thiscurve, type="l", xlab="Contract month", ylab="",
          main=paste(curvename, "historical"))
  graphics.off()
  
  ##--------------------------------------------------------------------------
  ## 4. Show quantile of the sim forward prices at different pricing dates
  ind <- c(3, 6, 12, 24, 36)
  asOfDates <- as.character(FP.options$contract.dts[ind])
  dimnames(res)[[1]] <- format(as.Date(dimnames(res)[[1]]), "%Y-%m-01")
  sPP <- melt(res[asOfDates,,])    # simulated prices
  sPP <- na.omit(sPP)
  colnames(sPP) <- c("pricing.dt", "contract.dt", "sim", "value")
  qPP <- cast(sPP, pricing.dt + contract.dt ~ ., quantile,
              c(0.01, 0.05, 0.95, 0.99))
  qPP <- melt(qPP, id=1:2)
  colnames(qPP)[4] <- "quantile"
  rownames(qPP) <- NULL
  qPP$quantile <- factor(qPP$quantile, labels=c(0.01, 0.05, 0.95, 0.99))
  qPP$contract.dt <- factor(substr(as.character(qPP$contract.dt), 1, 7))
  qPP$pricing.dt  <- paste("As of:", qPP$pricing.dt) 

  filename <- paste(FP.options$simCurves.report.dir, curvename, ".simquantiles.pdf", sep="")
  pdf(filename, width=6.5, height=8)  
  print(xyplot(value ~ contract.dt | pricing.dt, data=qPP, groups=quantile,
    type="l", layout=c(1, length(asOfDates)), scales=list(x=list(rot=90)),
    main=curvename, auto.key = list(points = FALSE, lines = TRUE, space = "top",
                      columns=4), ylab="Simulated price quantiles"))
  graphics.off()
  ##--------------------------------------------------------------------------
  ## 5. Show standard deviation of prices
  SD <- apply(res, c(1,2), sd, na.rm=T)
  filename <- paste(FP.options$simCurves.report.dir, curvename, ".std.dev.pdf", sep="")  
  pdf(filename, width=8, height=6)  
  matplot(as.Date(colnames(SD)), t(SD), type="l", xlab="Contract Date",
          xaxt="n", ylab="Standard deviation of simulated prices",
          main=curvename)
  axis(side=1, at=as.Date(colnames(SD)), labels=format(as.Date(colnames(SD)),
    "%Y-%m"), las=2, cex.axis=0.7)
  graphics.off()
  
  ##---------------------------------------------------------------------------
  ## 6. Plot price changes in 3mo, 6mo, and 1year. 

   trial<-try(plot.oneCurve.priceChange(curvename, FP.options, hp, res, type, backtest=FALSE))
    if(class(trial)=="try-error"){
       rLog("From backtest: The price change pltos cannot be printed")
    }else{
       rLog("Price changes for", curvename, "was printed")
   }
   
  ##Jing: This part is move to the end because sometimes the BW.SJ used by simple.violinplot doesn't work
  ## Need to check the ste and dpi method and findout why later  
  ##--------------------------------------------------------------------------
  ## 2. violin plots
  require(UsingR)
  nmon <- 24                            # plot 24 future months pricing days
  days <- T+(1:nmon)
  if(type=="pdf") {
    filename <- paste(FP.options$simCurves.report.dir, curvename, ".violin.pdf", sep="")
    pdf(filename, width=5.5, height=8, pointsize=8)
  }
  else if(type=="png") {
    filename <- paste(FP.options$simCurves.report.dir, curvename, ".violin.png", sep="")
    png(filename, width=550, height=800)
  }
  layout(1:4)
  par(mar=c(4.5, 4.5, 3,2))
  MM <- 3 ## at least 3 months away from today, otherwise there's little data
  for(i in 1:length(mon.plot)) { # plot first Jan, Apr, Jul, Oct 3+ months out
    idx.mon <- which(allmon[-(1:MM)]==mon.plot[i])[1] + MM
    dat <- res[days,idx.mon,]
   ## bw <- diff(quantile(dat, probs = c(0.05, 0.95), na.rm=T))/15
   #Jing: turn off the plotting temporarily until the bandwidth problem is solved.
   #simple.violinplot(as.vector(dat)~rep(FP.options$sim.days[days], FP.options$nsim),
   #   bw="SJ", xlab="", col="wheat")
   # points(apply(dat, 1, mean), pch=19)
   # title(paste(curvename, ", contract ", colnames(res)[idx.mon], sep=""))
  }
  graphics.off() 
}


#####################################################
## function to plot multiple curves
#####################################################
plot.MultiCurve <- function(curvenames, FP.options, filename="multiCurve",
                            type=c("pdf", "png"), rescale=TRUE) {
  type <- match.arg(type)

  nc <- length(curvenames) ## number of curves
  
  ## load in simulation result
  simres <- NULL
  for(i in 1:nc) {
    ## load simulation result
    load(paste(FP.options$targetdata.dir, curvenames[i], ".sim.RData", sep=""))
    simres[[i]] <- res
  }

  ## 1. matplot for 4 months, randomly chosen simulation
  if(type=="pdf") {
    filename <- paste(FP.options$report.dir, filename, ".pdf", sep="")
    pdf(filename, width=5.5, height=8, pointsize=8)
  }
  else if(type=="png") {
    filename <- paste(FP.options$report.dir, filename, ".png", sep="")
    png(filename, width=550, height=800)
  }
    
  tmp <- diff.bday(FP.options$sim.days)
  T <- length(which(tmp==1)) + 1
  D <- dim(simres[[1]])[2]

  par(mar=c(3.5, 3.5, 3,2), mfrow=c(4,1))
  
  ## get month index for Jan, Apr, Jul, Oct
  allmon <- as.integer(format(as.Date(colnames(res)), "%m"))
  mon.plot <- c(1,4,7,10) # months to plot

  ## plot
  MM <- 3
  idx.sim <- sample(FP.options$nsim, 1)
  for(i in 1:4) {
    idx.mon <- which(allmon[-(1:MM)]==mon.plot[i])[1] + MM
    dat <- matrix(NA, nrow=length(FP.options$sim.days), ncol=nc)
    for(j in 1:nc) {
      dat[,j] <- simres[[j]][,idx.mon, idx.sim]
    }
    ## remove NAs
    dat <- dat[!is.na(dat[,1]),]
    dat <- dat
 
    ## If requested, rescale data to make them have the same mean and var
    if(rescale) {
      mm <- colMeans(dat, na.rm=T)
      ss <- apply(dat, 2, sd, na.rm=T)
      nd <- dim(dat)[1]
      dat <- (dat-rep(mm,each=nd)) / rep(ss,each=nd)
    }
    matplot(dat, type="l", xlab="", ylab="",
            main=colnames(simres[[1]])[idx.mon])
    abline(v=T+0.5)
    if(i==1)
      legend("topleft", legend=curvenames, lty=1:nc, col=1:nc)
  }
  graphics.off()

}


#####################################################
## function to do histogram of heat rate
#####################################################
plot.HeatRate <- function(elec.curve, gas.curve, FP.options, filename="heatrate",
                            type=c("pdf", "png")) {
  type <- match.arg(type)
  
  ## init devices
  if(type=="pdf") {
    filename <- paste(FP.options$report.dir, filename, ".pdf", sep="")
    pdf(filename, width=6, height=3, pointsize=8)
  }
  else if(type=="png") {
    filename <- paste(FP.options$report.dir, filename, ".png", sep="")
    png(filename, width=800, height=400)
  }

  ## load simulation data
  load(paste(FP.options$targetdata.dir, elec.curve, ".sim.RData", sep=""))
  elec.sim <- res
  load(paste(FP.options$targetdata.dir, gas.curve, ".sim.RData", sep=""))
  gas.sim <- res

  ## load historical data
  mkt <- strsplit(elec.curve, " " )[[1]][2]
  load(paste(FP.options$swapdata.dir, "all.", mkt, ".hp.RData", sep=""))
  elec.hp <- hp[hp[,1]==elec.curve, 3:dim(hp)[2]]
  
  mkt <- strsplit(gas.curve, " " )[[1]][2]
  load(paste(FP.options$swapdata.dir, "all.", mkt, ".hp.RData", sep=""))
  gas.hp <- hp[hp[,1]==gas.curve, 3:dim(hp)[2]]

  par(mfrow=c(1,2), mar=c(2.5,1.5,4,1))
  ## historical heat rate
  hc.hist <- elec.hp/gas.hp
  hist(as.matrix(hc.hist), 50, yaxt="n", main="Historical")

  ## simulated heat rate
  hc.sim <- elec.sim/gas.sim
  hist(as.matrix(hc.sim), 50, yaxt="n", main="Simulated")

  graphics.off()
}



################################################################################
## function to plot curves for backtesting
################################################################################
plot.OneCurve.backtest <- function(curvename, FP.options, type = c("pdf", "png")){
 ##----------------------------------------------------------------------------
 # generate pdf reports of backtest results
 # Plot the following curves: 
 #          -- short-term forecast vs historical forward prices in 2 months following asOfDate
 #          -- violin plot of forecasts on the (up to) next 24 months
 #          -- all months-contracts plot for a randomly selected simulation
 #          -- quantile plot                                                
 #          -- standard deviation plot
 ##---------------------------------------------------------------------------- 
  type <- match.arg(type)
  library(lattice)
  require(UsingR)
  require(reshape)
  require(SecDb)
  
  if (file.access(FP.options$report.dir)!=0){mkdir(FP.options$report.dir)} 
  if(is.null(FP.options$backtest.report.dir)) FP.options$backtest.report.dir=FP.options$report.dir
  if(file.access(FP.options$backtest.report.dir)!=0){mkdir(FP.options$backtest.report.dir)}
  
  ##----------------------------------------------------------------------------
  ## load simulation result
  FP.options$sPtmp.dir = paste(FP.options$backtest.report.dir,"SimPriceTemp/", sep="")
  if (file.access(FP.options$sPtmp.dir)!=0){mkdir(FP.options$sPtmp.dir)}
  from.dir <-paste(FP.options$targetdata,tolower(curvename), sep ="")
  to.dir <- paste(FP.options$sPtmp.dir,"/", tolower(curvename), sep="")
  filecopy <- file.copy(from.dir, to.dir, overwrite=TRUE)
  if(filecopy){
    sPtemp.dir <-substr(FP.options$sPtmp.dir, 1, nchar(FP.options$sPtmp.dir)-1)
    sPP.env <- .getFileHashEnv(sPtemp.dir)
  }else
  { rLog("Simulated price file cannot be copied! Check if they have been simulated.")
    return()
   }
  res <- sPP.env[[tolower(curvename)]]
  rm(to.dir) 
  
  
  ##---------------------------------------------------------------------------
  ## load historical price
  loptions <- NULL
  loptions$asOfDate <- min(Sys.Date(), FP.options$sim.days[length(FP.options$sim.days)])
  loptions$calc$hdays <- seq(FP.options$asOfDate - FP.options$N.hist, loptions$asOfDate, by="day")  
  QQ <- data.frame(curve.name=curvename, contract.dt=FP.options$contract.dts)
  #take out historical data for this curve to a temp folder
  FP.options$hPtmp.dir = paste(FP.options$backtest.report.dir,"HistPriceTemp/", sep="")
  if (file.access(FP.options$hPtmp.dir)!=0){mkdir(FP.options$hPtmp.dir)}
  from.dir <-paste("//nas-msw-07/rmgapp/Prices/Historical","/",tolower(curvename), sep ="")
  to.dir <- paste(FP.options$hPtmp.dir,"/", tolower(curvename), sep="")
  filecopy <- file.copy(from.dir, to.dir, overwrite=TRUE)
  if(filecopy){
    hPtemp.dir <-substr(FP.options$hPtmp.dir, 1, nchar(FP.options$hPtmp.dir)-1)
    hPP.env <- .getFileHashEnv(hPtemp.dir)
  }else
  { rLog("Historical price file cannot be copied from filehash! Check if they exist.")
    return()
   }
  hp <- try(get.prices.from.R(QQ, loptions, hPP.env))  # get prices from the hash 
  #NOTE: get.prices.from.R may need to be updated, because it will delete expired contract dates, 
  #however, for backtesting, those expired contract dates may still be needed
  #remove file from the environment after historical data are obtained
  file.remove(to.dir)
  
  hp.thiscurve <- hp[, 3:ncol(hp)] 
  rownames(hp.thiscurve) <- hp[,"contract.dt"]
  
  allmon <- as.integer(format(as.Date(colnames(res)), "%m"))
  if(is.null(FP.options$backtest.contract.dts)){rWarn("No backtest contract period assigned!")}
  idx.mon = which(as.Date(colnames(res))>= FP.options$backtest.contract.dts[1] & as.Date(colnames(res))<= 
                  FP.options$backtest.contract.dts[length(FP.options$backtest.contract.dts)]) 
                  #index of months within backtest contract period
   
  ##
  ##-------------------------------------------------------------------------------------
  ## 1. Simulation prices for all simulation, each month per season                      
  ## only plot for daily pricing dates for 4 contract months                                            

  if(type=="pdf") {
    filename <- paste(FP.options$backtest.report.dir, curvename, ".backtest.dailysims.pdf", sep="")
    pdf(filename, width=5.5, height=8, pointsize=8)
  } else if (type=="png") {
    filename <- paste(FP.options$backtest.report.dir, curvename, ".backtest.dailysims.png", sep="")
    png(filename, width=550, height=800)
  }
  layout(1:4)
  par(mar=c(4.5, 4.5, 3,2))
  
  #find the first Jan, Apr, Jul, Oct
  idx.plots = NULL
  for (i in c(1,4,7,10)) idx.plots <- c(idx.plots, which(allmon[idx.mon]== i)[1])
  idx.plots <- idx.mon[idx.plots] 
  #idx.plots = idx.mon[which(allmon[idx.mon[1:max(12,length(idx.mon))]] %in% c(1,4,7,10))] #works but too complicated
  
  #find number of business days between two consecutive days in the input date vector
  #so that only simulations on consecutive pricing days(i.e. priced daily in the 2 months following asOfDate) are used
  tmp <- diff.bday(FP.options$sim.days) 
  T <- length(which(tmp==1)) + 1
  for(i in 1:length(idx.plots)) {
    ## simulation for this month
    dat <- res[1:T, idx.plots[i],]
    #dat <- res[1:T,idx.mon,]                          
    ## remove NAs
    dat <- dat[!is.na(dat[,1]),]
    ## find historical for this month
    colIdx <- which(colnames(hp.thiscurve) == as.character(FP.options$sim.days[T])) #which(tmp ==1)+1])))
    if(is.null(colIdx)){
      rLog("No such a pricing date exist in hist dates, check backtest starting time and re-do!")
      next()
    }else{
      hp.currentcurve <- hp.thiscurve[, 1:colIdx]
      thishp <- as.numeric(hp.currentcurve[colnames(res)[idx.plots[i]],])
   
      #number of historical days to plot together with the smoothscatter plot
      num.histD <-  dim(hp.currentcurve)[2]-which(colnames(hp.currentcurve)==as.character(FP.options$asOfDate))    
      ## the last 60 historical days plus the dates on which forecasts are performed
      ndays <- -(59 + num.histD)
      thishp <- thishp[(ndays:0)+length(thishp)]
    
      ## start plotting
      ylim <- range(dat, thishp, na.rm=T)
      xlim <- c(0, length(thishp))
      plot(thishp, xlim=xlim, ylim=ylim, type="l", lwd=2, xlab="", ylab="",
            main=paste(curvename, colnames(res)[idx.plots[i]]))
      xcoord <- length(thishp)-num.histD +(1:dim(dat)[1])
 
      ## smoothScatter for all simulations 
      smoothScatter(rep(xcoord, FP.options$nsim), as.vector(dat))
      ## sometimes smooth scatter will erase part of the line so I'll redraw the line
      lines(thishp, lwd=2)
      ##plot quantile data
      meltdata = melt(dat)
      meltdata = na.omit(meltdata)
      colnames(meltdata)= c("pricing.dt", "sim", "value")
      castdata = cast(meltdata, pricing.dt ~ ., quantile, c(0.1, 0.99))
      quantilY = castdata[,c("X10.", "X99.")]
      colorList = c("blue", "red")
      for (i in 1:length(quantilY)) lines(xcoord, quantilY[,i],col =colorList[i])
      legend("topleft",  col = c(colorList, "black"), legend = c("10 prctil", "99 prctil", "hist"),
            lty = 1, text.col = c(colorList, "black"))
      abline(v = 60)      
      mtext(FP.options$asOfDate,side =1, at =60, padj = 0.5, font = 0.5) #position of text should be adjusted according to figure layouts
    }
  }
  graphics.off()

  
  ##--------------------------------------------------------------------------
  ## 3. Put all months together for a randomly chosen simulations
  idx.sim <- sample(FP.options$nsim, 1)
 
  if(type=="pdf") {
    filename <- paste(FP.options$backtest.report.dir, curvename, ".allmon.pdf", sep="")
    pdf(filename, width=8, height=5.5, pointsize=8)
  } else if(type=="png") {
    filename <- paste(FP.options$backtest.report.dir, curvename, ".allmon.png", sep="")
    png(filename, width=550, height=800)
  }

  layout(matrix(1:4, 2, 2)); #layout.show(4)
  par(mar=c(4.5, 4.5, 3,2), mgp =c(2.5, 1, 0))
  ## simulations result 
  valid.contract.dts =                #for contracts only within the contract period (specified in Excel)
        which(dimnames(res)[[2]]>= FP.options$backtest.contract.dts[1] 
              & dimnames(res)[[2]]<= FP.options$backtest.contract.dts[length(FP.options$backtest.contract.dts)])
  valid.pricing.dts =                 #for pricing dates between backtest starting date and now (so we have historical data)
        which(dimnames(res)[[1]]>= FP.options$asOfDate 
              & dimnames(res)[[1]]<= (loptions$asOfDate-1))                
  dat <- res[valid.pricing.dts,valid.contract.dts,idx.sim]
  matplot(dat, type="l",ylab="", xlab=paste("Pricing date, t=0 is backtest as-of-date:",FP.options$asOfDate, ",t =",
          length(valid.pricing.dts), " is", dimnames(res)[[1]][max(valid.pricing.dts)], sep = ""), 
           cex.main=1, main=paste(curvename, "simulation", idx.sim, sep=""), sub = paste("daily pricing ends at x = ", as.character(T), sep =""))
  abline(v=T+0.1)   # where the monthly contracts start
  matplot(t(dat), type="l", xlab=paste("Contract month, from ", dimnames(res)[[2]][valid.contract.dts[1]]," to ", 
          dimnames(res)[[2]][max(valid.contract.dts)], sep =""), ylab="", cex.main=1, main=paste(curvename, "simulation", idx.sim, sep=""))
 
  ## historical
  valid.contract.dts = 
        which(dimnames(hp.thiscurve)[[1]]>= FP.options$backtest.contract.dts[1]                                                     
        & dimnames(hp.thiscurve)[[1]]<= FP.options$backtest.contract.dts[length(FP.options$backtest.contract.dts)])           
  valid.pricing.dates = dimnames(res)[[1]][valid.pricing.dts]

  hp.currentcurve = hp.thiscurve[valid.contract.dts, valid.pricing.dates]
                                                                                         
  ## the following part has some problem, there are many more historical pricing dates than
  matplot(t(hp.currentcurve), type="l", xlab=paste("Pricing date, t=0 is backtest as-of-date:", valid.pricing.dates[1], 
          " t =",length(valid.pricing.dates), " is", valid.pricing.dates[length(valid.pricing.dates)], sep =""),              
          ylab="", cex.main=1, main=paste(curvename, "historical", sep=""))   
  abline(v=T+0.5)   # where the monthly contracts start   
  matplot(hp.currentcurve, type="l", xlab=paste("Contract months, t=0 is",dimnames(hp.currentcurve)[[1]][1],                                             
          ", t =",length(valid.contract.dts), " is", dimnames(hp.currentcurve)[[1]][length(valid.contract.dts)] ),                                    
          ylab="", cex.main=1, main=paste(curvename, "historical", sep=""))                                                                      
 
  graphics.off()

  ##--------------------------------------------------------------------------
  ## 4. Show quantile of the sim forward prices at different pricing dates
  ##
  ind <- c(3, 6, 12, 24, 36)
  asOfDates <- as.character(FP.options$contract.dts[ind])
  dimnames(res)[[1]] <- format(as.Date(dimnames(res)[[1]]), "%Y-%m-01")
  sPP <- melt(res[asOfDates,,])    # simulated prices
  sPP <- na.omit(sPP)
  colnames(sPP) <- c("pricing.dt", "contract.dt", "sim", "value")
  qPP <- cast(sPP, pricing.dt + contract.dt ~ ., quantile,
              c(0.01, 0.05, 0.95, 0.99))
  

  names =format(as.Date(dimnames(hp.thiscurve)[[2]]), "%Y-%m-01")
  #among pricing dates, find the first day of each months  
  datlst = tapply(dimnames(hp.thiscurve)[[2]], names, function(x) {sort(x)[1]})
  tmpcurve <- hp.thiscurve[, datlst]
  #find historical data for the same pricing month (data of the first pricing day of the month)
  dimnames(tmpcurve)[[2]] <- format(as.Date(dimnames(tmpcurve)[[2]]), "%Y-%m-01")
  indhp<-which(dimnames(tmpcurve)[[2]] %in% levels(qPP$pricing.dt))

  if(!length(indhp)==0){
    hp.currentcurve = tmpcurve[,indhp]
    hP <- melt(t(hp.currentcurve))
    hP <- na.omit(hP)
    colnames(hP)[1:2] <- c("pricing.dt","contract.dt")
    hP = cbind(hP, rep("hist", length = dim(hP)[1]))
  }
  
  qPP <- melt(qPP, id=1:2)
  if(!length(indhp)==0){
    colnames(hP) =colnames(qPP)
    qPP  <- rbind(qPP, hP)
  }
  qPP$pricing.dt  <- paste("As of:", qPP$pricing.dt)
  qPP$contract.dt <- factor(substr(as.character(qPP$contract.dt), 1, 7))
   
  colnames(qPP)[4] <- "quantile"                                                                            
  rownames(qPP) <- NULL   
  if(length(levels(qPP$quantile))==5){
    qPP$quantile <- factor(qPP$quantile, labels=c(0.01, 0.05, 0.95, 0.99, "hist"))
  }else{
    qPP$quantile <- factor(qPP$quantile, labels=c(0.01, 0.05, 0.95, 0.99))
  }
 
  filename <- paste(FP.options$backtest.report.dir, curvename, ".simquantiles.pdf", sep="")
  pdf(filename, width=6.5, height=8)  
  print(xyplot(value ~ contract.dt | pricing.dt, data=qPP, groups=quantile,
    type="l", layout=c(1, length(asOfDates)), scales=list(x=list(rot=90)),
    main=curvename, auto.key = list(points = FALSE, lines = TRUE, space = "top",
                      columns=4), ylab="Simulated price quantiles"))
  graphics.off()
  
  
  ##--------------------------------------------------------------------------
  ## 5. Show standard deviation of prices
  SD <- apply(res, c(1,2), sd, na.rm=T)
  filename <- paste(FP.options$backtest.report.dir, curvename, ".std.dev.pdf", sep="")  
  pdf(filename, width=8, height=6)  
  matplot(as.Date(colnames(SD)), t(SD), type="l", xlab="Contract Date",
          xaxt="n", ylab="Standard deviation of simulated prices",
          main=curvename)
  axis(side=1, at=as.Date(colnames(SD)), labels=format(as.Date(colnames(SD)),
    "%Y-%m"), las=2, cex.axis=0.7)
  graphics.off()

 ##---------------------------------------------------------------------------
 ##6. Print cdf of hist and simulated price change (3mo, 6mo, and 1yr)
 ## 
 trial<-try(plot.oneCurve.priceChange(curvename, FP.options, hp, res, type, backtest=TRUE))
 if(class(trial)=="try-error"){
    rLog("From backtest: The price change plots cannot be printed")
 }else{
    rLog("Price changes for", curvename, "was printed")
 }
 
  ##Jing: This part is move to the end because sometimes the BW.SJ used by simple.violinplot doesn't work
  ## Need to check the ste and dpi method later 
  ##--------------------------------------------------------------------------
  ## 2. violin plots
  nmon <- 24          # plot 24 future months pricing days
  days <- T+(1:nmon)  # pricing days, beyond the first two months
  if(type=="pdf") {
    filename <- paste(FP.options$backtest.report.dir, curvename, ".backtest.violin.pdf", sep="")
    pdf(filename, width=5.5, height=8, pointsize=8)
  }  else if(type=="png") {
    filename <- paste(FP.options$backtest.report.dir, curvename, ".backtest.violin.png", sep="")
    png(filename, width=550, height=800)
  }
  layout(1:4)
  par(mar=c(4.5, 4.5, 3,2))
  
  # find the first Jan, Apr, Jul & Oct within backtesting period,
  # and at least 3 months away from today(otherwise there's little data)
  MM <- 3  
  idx.mon <- idx.mon[which(idx.mon > MM)[1]:length(idx.mon)]  #idx.mon records the index of those months that are within backtesing period
  #idx.plots = idx.mon[which(allmon[idx.mon[1:max(12,length(idx.mon))]] %in% c(1,4,7,10))] 
  idx.plots = NULL
  for (i in c(1,4,7,10)) idx.plots <- c(idx.plots, which(allmon[idx.mon]== i)[1])
  idx.plots <- idx.mon[idx.plots]                                                        
  
  # plot first Jan, Apr, Jul, Oct 3+ months out
  for(i in 1:length(idx.plots)) { 
    dat <- res[days,idx.plots[i],]
    ## bw <- diff(quantile(dat, probs = c(0.05, 0.95), na.rm=T))/15
    #Jing: temporarily turn off the plotting of violin curves
    #trial<-try(simple.violinplot(as.vector(dat)~rep(FP.options$sim.days[days], FP.options$nsim),
    #   bw="SJ-ste", xlab="", col="wheat") )
    #if(class(trial)=="try-error") next()
    #points(apply(dat, 1, mean), pch=19)
    #title(paste(curvename, ", contract ", colnames(res)[idx.plots[i]], sep=""))
  }
  graphics.off() 

}


##==============================================================================
##Function to find 3month, 6month, and 1year price changes historically and by simulation
##Plot the frequency plot of changes, compare with the simulation quantiles
## This can be used in both backtesting and forecasting
## Jing: note: this function is written in a rush, I will try to re-write later.
##============================================================================== 
plot.oneCurve.priceChange <- function(curvename,FP.options,hp=NULL,res=NULL,
                                      type = c("pdf", "png"), backtest=TRUE){
  rLog("plotting price changes...")
  if(is.null(hp) |is.null(res)){
    rLog("no data input for plotting price changes, quit plotting!")
    return()
  }
  asOfDate=FP.options$asOfDate
  tmpdate <-c(as.Date(secdb.dateRuleApply(asOfDate, "+4m")),as.Date(secdb.dateRuleApply(asOfDate, "+7m")),
                  as.Date(secdb.dateRuleApply(asOfDate, "+13m")))
  contract.mo <-format(tmpdate, "%Y-%m-01")
  hp.subset<-hp[which(hp$contract.dt %in% as.Date(contract.mo)),]
  ##Find simulation pricing dates (these dates can be anything, but we only look at contracts of 1 month later.
  pricing.dt <-c(as.Date(secdb.dateRuleApply(asOfDate, "+3m")),as.Date(secdb.dateRuleApply(asOfDate, "+6m")),
                  as.Date(secdb.dateRuleApply(asOfDate, "+12m")))
  hp.subset <- hp.subset[,!apply(hp.subset, 2, function(x){any(is.na(x))})]
  hist.dts <- as.Date(colnames(hp.subset)[-(1:2)])
  
  ##
  ##Find historical 3month, 6month and 1 year price changes for the above contracts
  data.3m <- NULL
  data.6m <- NULL
  data.1y <- NULL
  # 3 month
  if(!is.null(hist.dts)& length(hist.dts)>=120){
    hist.dts.last <- as.Date(secdb.dateRuleApply(hist.dts[length(hist.dts)], "-3m"))
    valid.dts.3m <- hist.dts[which(hist.dts<=hist.dts.last)]
    valid.dts.3m <- as.Date(valid.dts.3m)
    #colnames(valid.hist.dts.1m)=c("hist.dts")
    data.3m <- matrix(data=NA, nrow=length(valid.dts.3m))
    for(i in 1:length(valid.dts.3m)){
      next.dt<-as.character((secdb.dateRuleApply(valid.dts.3m[i], "+3m")))
      if(!is.null(hp.subset[1,next.dt])){
        data.3m[i]<-hp.subset[1,next.dt]-hp.subset[1,as.character(valid.dts.3m[i])]
      }
    }
    data.3m<-data.3m[!is.na(data.3m)]
  }
  
  # 6 months
  if(!is.null(hist.dts)& length(hist.dts)>=150){ #assume 252 b.days per year 
    hist.dts.last=as.Date(secdb.dateRuleApply(hist.dts[length(hist.dts)], "-6m"))
    valid.dts.6m<-hist.dts[which(hist.dts<=hist.dts.last)]
    valid.dts.6m<-as.Date(valid.dts.6m)
    #colnames(valid.hist.dts.1m)=c("hist.dts")
    data.6m <- matrix(data=NA, nrow=length(valid.dts.6m))             
    for(i in 1:length(valid.dts.6m)){
      next.dt<-as.character((secdb.dateRuleApply(valid.dts.6m[i], "+6m")))
      if(!is.null(hp.subset[2,next.dt])){
        data.6m[i]<-hp.subset[2,next.dt]-hp.subset[2,as.character(valid.dts.6m[i])]
      }
    }
    data.6m<-data.6m[!is.na(data.6m)]
  }
  
  ## 1 year
  if(!is.null(hist.dts)& length(hist.dts)>=260){  #assume 252 business days per year
    hist.dts.last=as.Date(secdb.dateRuleApply(hist.dts[length(hist.dts)], "-1y"))
    valid.dts.1y<-hist.dts[which(hist.dts<=hist.dts.last)]
    valid.dts.1y<-as.Date(valid.dts.1y)
    #colnames(valid.hist.dts.1m)=c("hist.dts")
    data.1y <- matrix(data=NA, nrow=length(valid.dts.1y))
    for(i in 1:length(valid.dts.1y)){
      next.dt<-as.character((secdb.dateRuleApply(valid.dts.1y[i], "+1y")))
      if(!is.null(hp.subset[3,next.dt])){
        data.1y[i]<-hp.subset[3,next.dt]-hp.subset[3,as.character(valid.dts.1y[i])]
      }
    }
    data.1y<-data.1y[!is.na(data.1y)]
  }

  ##
  ## For Simulated data...
  #find indices the pricing dates in res to obtain results
  pricing.idx <-matrix(NA,nrow=3)
  for(i in 1:length(tmpdate)){
    timediff<-as.numeric(as.Date(dimnames(res)[[1]])-pricing.dt[i])
    pricing.idx[i,1]=which.min(abs(timediff))
  }
  #find the prices on asOfDate
  F0=hp.subset[,as.character(asOfDate)] 
  #Calculate price changes of the simulated prices
  sim.price.change=as.list(NULL)
  for(i in 1:length(tmpdate)){
      sim.price.change[[i]]<-res[pricing.idx[i],contract.mo[i],]-F0[i]
  }

  ##Make Plots
  if(type=="pdf"){
    if(backtest){
        filename <- paste(FP.options$backtest.report.dir, curvename, ".backtest.pricechanges.pdf", sep="")
      }else{
        filename <- paste(FP.options$simCurves.report.dir, curvename, ".pricechanges.pdf", sep="")
      }
      pdf(filename, width=5.5, height=8, pointsize=8)
  }else if(type=="png"){
    if(backtest){
      filename <- paste(FP.options$backtest.report.dir, curvename, ".backtest.pricechange.png", sep="")
    }else{
      filename <- paste(FP.options$simCurves.report.dir, curvename, ".pricechange.png", sep="")
    }
    png(filename, width=550, height=800)
  }
  layout(1:3)
  
  xlim=range(sim.price.change[[1]],data.3m)
  if(!is.null(data.3m)) {
    plot(ecdf(data.3m), xlim=xlim, xlab="price change $",main=curvename, col.point="red", col.hor="red")
    lines(ecdf(sim.price.change[[1]]), do.points=FALSE, col.hor="blue")
    legend("topleft", col=c("red","blue"), lty=1, legend=c("hist","simulation"),pch=c(1,-1))
    legend("bottomright", legend=paste("3-month change, contract month ", as.character(contract.mo[1]), ", simulation pricing on ", pricing.dt[1],sep=""))
  }else{
    plot(ecdf(sim.price.change[[1]]),xlim=xlim, xlab = "price change $", main =curvename, do.points=FALSE, col.hor="blue")
    legend("topleft", col=c("blue"), lty=1, legend=c("simulation"),pch=c(-1))
    legend("bottomright", legend=paste("3-month change, contract month ", as.character(contract.mo[1]), ", simulation pricing on ", pricing.dt[1],sep=""))
  }
   
    
  
  xlim=range(sim.price.change[[2]],data.6m)
  if(!is.null(data.6m)){
    plot(ecdf(data.6m), xlim=xlim, xlab="price change $",main=curvename, col.point="red", col.hor="red")
    lines(ecdf(sim.price.change[[2]]), do.points=FALSE, col.hor="blue")
    legend("topleft", col=c("red","blue"), lty=1, legend=c("hist","simulation"),pch=c(1,-1))
    legend("bottomright", legend=paste("6-month change, contract month ", as.character(contract.mo[2]), ", simulation pricing on ", pricing.dt[2],sep=""))
  }else{
    plot(ecdf(sim.price.change[[2]]), xlm=xlim, xlab="price change $",main=curvename, do.points=FALSE, col.hor="blue")
    legend("topleft", col=c("blue"), lty=1, legend=c("simulation"),pch=c(-1))
    legend("bottomright", legend=paste("6-month change, contract month ", as.character(contract.mo[2]), ", simulation pricing on ", pricing.dt[2],sep=""))
  }
    
  xlim=range(sim.price.change[[3]],data.1y)
  if(!is.null(data.1y)){
    plot(ecdf(data.1y),xlim=xlim, xlab="price change $",main=curvename, col.point="red", col.hor="red")
    lines(ecdf(sim.price.change[[3]]), do.points=FALSE, col.hor="blue")
    legend("topleft", col=c("red","blue"), lty=1, legend=c("hist","simulation"),pch=c(1,-1))
    legend("bottomright", legend=paste("1-yr change, contract month ", as.character(contract.mo[3]), ", simulation pricing on ", pricing.dt[3],sep=""))
  }else{
    plot(ecdf(sim.price.change[[3]]),do.points=FALSE, xlim=xlim, xlab="price change $",main=curvename, col.hor="blue")
    legend("topleft", col=c("blue"), lty=1, legend=c("simulation"),pch=c(-1))
    legend("bottomright", legend=paste("1-yr change, contract month ", as.character(contract.mo[3]), ", simulation pricing on ", pricing.dt[3],sep=""))
  }

  
  
  graphics.off()
  
  }


