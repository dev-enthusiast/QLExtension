# email RT gen and prices for nepool
#
#
#


#################################################################
#
.pull_data <- function()
{
  startDt <- as.POSIXct(paste(format(Sys.Date()), "00:00:01"))
  endDt <- Sys.time()
  
  symbols <- c(
   "NEPOOL_SMD_DA_4000_lmp",           "hub.da",
   "NEPOOL_SMD_RT5m_4000_lmp",         "hub.rt",            
   "NeRpt_oi_unit_502_curMW",          "My7", 
   "NeRpt_oi_unit_1478_curMW",         "My8", 
   "NeRpt_oi_unit_1616_curMW",         "My9", 
   "NeRpt_oi_unit_40327_curMW",        "FR11", 
   "NeRpt_oi_unit_40328_curMW",        "FR12", 
   "NeRpt_oi_unit_14614_curMW",        "Kln",
   "NeRpt_oi_unit_1625_curMW",         "GR"            
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)

  # because sometimes I don't have all the data!
  aux <- symbols[,"shortNames"]; names(aux) <- symbols[,"tsdb"]
  colnames(HH) <- aux[colnames(HH)]  

  HH
}


#################################################################
#
.plot_data <- function(HH, savePlot=TRUE)
{
  if (savePlot) 
    png(filename="S:/All/Structured Risk/NEPOOL/Temporary/rt_gen_prices.png",
        width=640, height=480)

  ylim <- range(c(as.numeric(HH[,1]), as.numeric(HH[,2])), na.rm=TRUE)
  ylim[1] <- ylim[1] - (ylim[2]-ylim[1])/20
  ylim[2] <- ylim[2] + (ylim[2]-ylim[1])/20
  xlim <- range(index(HH))

  par(mar=c(2,3,1,3))
  plot(xlim, ylim, type="n", xlim=xlim, ylim=ylim, xlab="",
    ylab="Price, $/MWh")

  y2lim <- c(0, max(as.numeric(HH[,-c(1,2)]), na.rm=TRUE))  
  y2lim[2] <- y2lim[2] + (y2lim[2]-y2lim[1])/20
  slope <- (y2lim[2] - y2lim[1])/(ylim[2] - ylim[1])
  
  xTicks <- axTicks(1)
  yTicks <- axTicks(2)
  
  y2Ticks <- round((c(ylim[1], yTicks) - ylim[1])*slope + y2lim[1])
  axis(side=4, at=c(ylim[1],yTicks), labels=y2Ticks)   # add the second axis
  mtext("Generation, MW", side=4, line=2)
  mtext("Price, $/MWh", side=2, line=2)
  
  abline(v=xTicks,  lty=3, col="gray80")
  abline(h=c(ylim[1], yTicks), lty=3, col="gray80")

  if ("hub.da" %in% names(HH)) {
    aux <- na.omit(HH[,"hub.da"])
    lines(index(aux), as.numeric(aux), col="red", lty=3, type="o")  # DA
  }

  if ("hub.rt" %in% names(HH)) {
    aux <- na.omit(HH[,"hub.rt"])
    lines(index(aux), as.numeric(aux), col="red", lty=2)     # RT 5m
   
    aux <- aggregate(HH[,"hub.rt"], as.POSIXct(format(index(HH) - 
                1, "%Y-%m-%d %H:00:00")) + 3600, mean, na.rm = TRUE)
    lines(index(aux), as.numeric(aux), col="red", lwd=2)  # RT hourly
  }

  
  if ("My7" %in% names(HH)) {
    aux <- (na.omit(HH[,"My7"]) - y2lim[1])/slope + ylim[1]
    lines(index(aux), as.numeric(aux), col="magenta")   # My7
  }
  
  if ("My8" %in% names(HH)) {
    aux <- (na.omit(HH[,"My8"]) - y2lim[1])/slope + ylim[1]
    lines(index(aux), as.numeric(aux), col="blue")      # My8
  }
  
  if ("My9" %in% names(HH)) {
    aux <- (na.omit(HH[,"My9"]) - y2lim[1])/slope + ylim[1]
    lines(index(aux), as.numeric(aux), col="cyan")      # My9
  }
  
  if (all(c("FR11", "FR12") %in% names(HH))) {
    aux <- (na.omit(HH[,"FR11"]+HH[,"FR12"])- y2lim[1])/slope + ylim[1]
    lines(index(aux), as.numeric(aux), col="green")     # FR
  }
  
  if ("Kln" %in% names(HH)) {
    aux <- (na.omit(HH[,"Kln"])- y2lim[1])/slope + ylim[1]
    lines(index(aux), as.numeric(aux), col="tomato")     # Kln
  }
  
  if ("GR" %in% names(HH)) {
    aux <- (na.omit(HH[,"GR"])- y2lim[1])/slope + ylim[1]
    lines(index(aux), as.numeric(aux), col="gold")     # GR
  }

  cols <- c("magenta", "blue", "cyan", "green", "tomato", "gold")
  legend("top", c("My7", "My8", "My9", "FR", "Kln", "GR"),
    col=cols,
    horiz=TRUE, bty="n",
    fill=cols,
    border=cols,
    pt.bg=cols,
    inset=c(0, -0.02))

  if ( savePlot )
    dev.off()   
}




#################################################################
#################################################################
#
options(width=200)  
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase)
require(CEGterm)
require(SecDb)
require(zoo)
require(reshape)

rLog(paste("Start proces at ", Sys.time()))

HH <- .pull_data()

.plot_data(HH, savePlot=TRUE)

out <- c("Procmon job: RMG/R/Reports/Energy/Trading/Congestion/email_rt_gen+prices",
  "Contact: Adrian Dragulescu.\n")

rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
  "NECash@constellation.com",
#  "adrian.dragulescu@constellation.com",
  "Nepool prices and generation", out,
  attachments="S:/All/Structured Risk/NEPOOL/Temporary/rt_gen_prices.png")

rLog(paste("Done at ", Sys.time()))



returnCode <- 999    # succeed = 0
q( status = returnCode )

