# Compare the simulated spreads with the historical data. 
#
#
#

plot.spreads <- function(sim.spread, dMM, save, options){

  ind   <- which(colnames(dMM) %in% options$zone.names)
  ind.H <- which(colnames(dMM) == options$ref.name)
  X     <- dMM[,ind]-dMM[,ind.H]    # the spread to the reference location

  fdays    <- as.Date(rownames(sim.spread))
  loptions=NULL; loptions$season <- options$season
  fSeason <- calculate.season(data.frame(year=format(fdays, "%Y"),
             month=format(fdays, "%m")), loptions)$season
  uSeasons <- as.character(unique(fSeason))
  for (season in uSeasons){
    ind <- which(season==dMM$season)
    hSS <- data.frame(X[ind,], type="Historical")   # historical spread for this season
    ind <- which(season==fSeason)
    fSS <- apply(sim.spread[ind,,], 2, rbind)
    fSS <- data.frame(fSS[1:dim(hSS)[1],], type="Simulated")
    data <- rbind(hSS, fSS)   #pairs(hSS, pch=".")
    
    #---------------------------------------------Plot----
    filename  <- paste(save$dir.plots, "splom_",season,".pdf", sep="")
    pdf(filename, width=10, height=10)
    bkg.col <- trellis.par.get("background")
    bkg.col$col <- "white"
    trellis.par.set("background", bkg.col)
    sym.col <- trellis.par.get("superpose.symbol")
    sym.col$col <- c("blue", "red")
    sym.col$pch <- c(1)
    trellis.par.set("superpose.symbol", sym.col)
    print(splom( ~ data[,1:(ncol(data)-1)],
          key = list(space = "top", column=2, text=list(levels(data$type)),
            points=Rows(sym.col, 1:2)),
          groups = data$type,      
          main=paste(options$run.name,", ", season),
          xlab=""))
    dev.off()
  }

  
}
