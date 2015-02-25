FwdSims <- function(directory, FCurve, nFwdSims, muYr1, sigYr1, muYr, sigYr, noPaths, price.ceiling, price.floor) {
  forecast <- FCurve   #c(100.00,	115.00,	126.00,	128.00,	125.00,	120.00,	110.00,	100.00,	95.00,	95.00,	96.00,	97.00,	98.00)
  numSims <- nFwdSims
  muYr1 <- muYr1
  sigYr1 <- sigYr1
  muYr <- muYr
  sigYr <- sigYr
  noPaths <- min(12, noPaths)
  wkdir <- directory
  price.ceiling <- price.ceiling
  price.floor <- price.floor
  
  fcst.alt <- data.frame(matrix(data=NA, nrow=numSims+1, ncol=length(forecast), byrow=T) )
  fcst.alt[numSims+1,] <- forecast

  for (i in 1:numSims) {
    yr1.2.fac <- rnorm(1, muYr1, sigYr1)
    fcst.alt[i,1] <- forecast[1] * (1 + yr1.2.fac)
    bumps <- rnorm(n=length(forecast)-1, muYr, sigYr)
    for (j in 2:length(forecast)) {   # number of years in forward curve
      fcst.alt[i,j] <- fcst.alt[i,j-1] * (1+(bumps[j-1]) )
      fcst.alt[i,j] <- max(fcst.alt[i,j], price.floor)
      fcst.alt[i,j] <- min(fcst.alt[i,j],price.ceiling)
    }
  }
   fcst.alt <- t(fcst.alt)

  color = c("mediumturquoise","black" ,"orange","deeppink", "coral", "red", "black", "maroon",
                "darkgrey", "darkblue", "darkgreen", "brown")
  y.range = range(fcst.alt[,1:noPaths])
  if(file.exists(paste(wkdir,"SimulatedUForecasts.jpg", sep=""))) file.remove(paste(wkdir,"SimulatedUForecasts.jpg", sep=""))
  jpeg(paste(wkdir, "SimulatedUFOrecasts.jpg", sep=""), width = 640, height = 520)
  plot(1:nrow(fcst.alt), fcst.alt[,ncol(fcst.alt)], ylim=y.range, type = "b", lwd=3, main = "Forecast Simulations")
  for (k in 1:(ncol(fcst.alt)-1)) {

    lines(1:nrow(fcst.alt), fcst.alt[,k], type="l", col=color[k], lty=1, lwd=1)
  }
  dev.off()
  fcst.alt <<- fcst.alt
  write.csv(fcst.alt, file = paste(wkdir,"ForecastSims.csv", sep=""))

 return(fcst.alt)
 } # end function