# Plot CPSP data frames.
#
#
#

FTR.plot.CPSP <- function(CPSP, bid.limits=NULL, runName="",
  orderHistorical=TRUE, maxPlotsPage=10, maxMonths=NULL,
  layOut=NULL, save=TRUE, ...)
{
  if (!("package:lattice" %in% search()))
    require(lattice)
  if (!(all(c("path.no", "auction", "CP", "SP") %in% names(CPSP))))
    stop("Missing one of columns path.no, auction, CP, SP in input data.frame CPSP.")

  CPSP$month <- FTR.AuctionTerm(auction=CPSP$auction)$start.dt
  if (is.null(maxMonths))
    maxMonths <- length(unique(CPSP$month))

  uMonths <- sort(unique(CPSP$month))
  if (maxMonths != length(uMonths))
    CPSP <- subset(CPSP, month %in% rev(uMonths)[1:maxMonths])
  
  # need to fill missing data with NA's 
  CPSP <- merge(CPSP, expand.grid(path.no=unique(CPSP$path.no),
                month=unique(CPSP$month)), all.y=TRUE)
  
  
  if (!("auction" %in% names(CPSP)))
    CPSP <- cbind(CPSP, auction=FTR.AuctionTerm(CPSP$month, CPSP$month))
  
  if (orderHistorical)
    CPSP$auction <- factor(CPSP$auction,
      levels=FTR.AuctionTermLevels(CPSP$auction))
  
  if (!("panelName" %in% names(CPSP)))
    CPSP <- cbind(panelName=paste("path.no", CPSP$path.no), CPSP)
  
  aux <- melt(CPSP[,c("panelName", "path.no", "auction", "CP", "SP")], id=1:3)
  uPaths <- sort(unique(as.numeric(aux$path.no)))

  noPlots <- length(uPaths)              # how many panels
  if (!is.null(layOut))
    maxPlotsPage <- prod(layOut)
  noPages <- ceiling(length(uPaths)/maxPlotsPage)

  
  if (save){
    filename <- paste(getwd(), "/", runName, "CPSP.pdf", sep="")
    trellis.device(pdf, file=filename, width=11, height=8.5)
  }
  for (batch in 1:noPages){
    ind.b <- ((batch-1)*maxPlotsPage+1):min(batch*maxPlotsPage, length(uPaths))
    bux <- subset(aux, path.no %in% uPaths[ind.b])
    bux <- bux[order(bux$path.no),]
    
    xrange <- cast(bux, path.no ~ ., function(x)c(min(x, na.rm=TRUE), max(x, na.rm=TRUE)))
    names(xrange)[2:3] <- c("xmin","xmax")
    
    if (!is.null(bid.limits)){
      bux <- merge(bux, bid.limits[,c("path.no", "min.bid","max.bid")])
      xrange <- merge(xrange, bid.limits[,c("path.no", "min.bid","max.bid")])
      xrange$xmin <- pmin(xrange$xmin, xrange$min.bid)
      xrange$xmax <- pmax(xrange$xmax, xrange$max.bid)
    } else {
      bux$min.bid <- bux$max.bid <- NA
    }
    
    bux$panelName <- factor(bux$panelName, levels=unique(bux$panelName))
    bux$col <- ifelse(bux$variable=="CP", "blue", "red")
    bux$pch <- ifelse(bux$variable=="CP", 1, 3)

    bux <- merge(bux, xrange)
    xrange <- split(xrange[,c("xmin", "xmax")], xrange$path.no)

    sym.col <- trellis.par.get("superpose.symbol")
    sym.col$col  <- c("blue", "red")
    sym.col$pch <- c(1,3)
    trellis.par.set("superpose.symbol", sym.col)
    trellis.par.set("clip", list(panel = "off"))  # very important

    if (is.null(layOut)){
      if (noPages==1){
        if (noPlots <= maxPlotsPage/2){
          layOut <- c(length(ind.b),1)        
        } else {  
          layOut <- c(length(ind.b)%/%2,2)
        }
      } else {
        layOut <- c(maxPlotsPage/2,2)   # for multiple pages keep scale
      }
      if (noPlots==1) layOut <- c(1,1)  # if only one plot, take all page
    }
    
    plot(dotplot(auction ~ value | panelName, data=bux, groups=variable,
            col=bux$col, pch=bux$pch, 
      prepanel=function(x,y, groups, ..., subscripts ){
        list(xlim=bux[subscripts[1],c("xmin", "xmax")]) 
#             xat=pretty(bux[subscripts[1],c("xmin", "xmax")]))
      },           
      panel=function(x,y, groups, ..., subscripts){
        vbar <- bux[subscripts[1],c("min.bid", "max.bid")]
# current.panel.limits()
        prettyL=pretty(current.panel.limits()$xlim)
        panel.xyplot(x,y, ...)
        panel.abline(v=vbar, col="gray")
        panel.axis(side="bottom", outside=TRUE,
          at=prettyL, labels=as.character(prettyL), rot=0)
     },
      key=simpleKey(space="top", points=T,
               text=c("Clearing price", "Settle price"),
               columns=2),
      scales=list(relation="free", x=list(xlim=xrange)),
      layout=layOut, ...
    ))
  }
  if (save){
    cat(paste("Wrote the plots to", filename, "\n"))
    dev.off()
  }
}
