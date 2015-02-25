# Produce a bunch of results. 
#
#
# Written by Adrian Dragulescu on 21-Apr-2005

analyze.generation <- function(MM, save, options){

  options$plot  <- list(main=options$main)
  options$table <- list(fontsize="footnotesize", tex.digits=0)
  options$total.line <- 1
  if (length(unique(MM$ID))>1){cat("Have more than one unit.  Sum over units first.")}
  options$factors <- c("YEAR", "MONTH", "ITERATION")
  yyyy.mm <- paste(MM$YEAR, MM$MONTH, sep="-")
  Months  <- as.Date(paste(MM$YEAR, MM$MONTH, "01", sep="-"))
  uMonths <- as.Date(paste(unique(yyyy.mm), "-01", sep=""))

  loptions=NULL
  loptions$asOfDate <- options$asOfDate;
  loptions$months   <- uMonths
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
  DF <- get.discount.factor(loptions)$DF; names(DF) <- uMonths
  MM$DF <-  DF[as.character(Months)]
  
  #------------------------------Distribution of gross margin-----
  MM$GM <- (MM$REVENUE-MM$COST)*MM$DF           # discounted GM
  aux <- tapply(MM$GM, list(MM$ITERATION, MM$YEAR), sum)
  aux <- stack(as.data.frame(aux))
  data <- data.frame(value=aux$values/options$scale.factor, factor=aux$ind)
  save$filename.table <- paste(save$dir$tables, "gm", sep="")
  save$filename.plot  <- paste(save$dir$plots, "gm", sep="")
  options$plot$ylab   <- "Gross margin, M$"
  options$table$tex.caption <- "Discounted gross margin, M\\$"
  options$probs <- c(0,0.01,0.05,seq(0.1,0.9,0.1),0.95,0.99,1)
  quantiles.by.factor(data, save, options)
  
  bux <- tapply(MM$GM, list(Months, MM$ITERATION), sum)
  gm  <- apply(bux, 2, sum)/1000000  # gross margin
  fileName <- paste(save$dir$plots,"pdf_gm.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=6, height=4.5)}
  plot(density(gm), lwd=2, col="blue", xlab="Gross Margin, M$",
      ylab="Density distribution", main=options$main)
  rug(gm)
  abline(v=mean(gm), col="grey")
  if (save$Analysis$all){dev.off()}
  
  fileName <- paste(save$dir$plots,"cdf_gm.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=6, height=4.5)}
  plot(sort(gm), (1:500)/500, type="o", col="blue", ylim=c(0,1), 
       xlab="Gross margin, M$", ylab="Cumulative Probability",
       main=options$main)
  abline(h=seq(0,1, by=0.1), col="lightgray", lty="dotted")
  if (save$Analysis$all){dev.off()}
  
  #---------------------------------------------------------------
   
  aux <- tapply(MM[,"REVENUE"]*MM$DF, list(MM$ITERATION, MM$YEAR), sum)
  aux <- stack(as.data.frame(aux))
  data <- data.frame(value=aux$values/options$scale.factor, factor=aux$ind)
  save$filename.table <- paste(save$dir$tables, "revenues", sep="")
  save$filename.plot  <- paste(save$dir$plots, "revenues", sep="")
  options$plot$ylab   <- "Revenues, M$"
  options$table$tex.caption <- "Discounted revenues, M\\$"
  quantiles.by.factor(data, save, options)

  aux <- tapply(MM[,"COST"]*MM$DF, list(MM$ITERATION, MM$YEAR), sum)
  aux <- stack(as.data.frame(aux))
  data <- data.frame(value=aux$values/options$scale.factor, factor=aux$ind)
  save$filename.table <- paste(save$dir$tables, "costs", sep="")
  save$filename.plot  <- paste(save$dir$plots, "costs", sep="")
  options$plot$ylab   <- "Costs, M$"
  options$table$tex.caption <- "Discounted costs, M\\$"
  quantiles.by.factor(data, save, options)

  aux <- tapply(MM[,"GENERATION"], list(MM$ITERATION, MM$YEAR), sum)
  aux <- stack(as.data.frame(aux))
  data <- data.frame(value=aux$values/options$scale.factor, factor=aux$ind)
  save$filename.table <- paste(save$dir$tables, "generation", sep="")
  save$filename.plot  <- paste(save$dir$plots, "generation", sep="")
  options$plot$ylab   <- "Generation, MMWh$"
  options$table$tex.caption <- "Generation, MMWh\\$"
  quantiles.by.factor(data, save, options)

  #---------------------------------------------------------------------------------
  #                  Distribution of prices by bucket
  #---------------------------------------------------------------------------------  
  options$table$tex.digits <- 2
  options$total.line <- 0

  uBuckets <- as.character(unique(MM$BUCKET))
  for (b in uBuckets){  
    data <- subset(MM, BUCKET=b)
    data <- data.frame(value=data$PRICE,factor=factor(data$YEAR))   
    save$filename.table <- paste(save$dir$tables, "price_",b, sep="")
    save$filename.plot  <- paste(save$dir$plots, "price_",b, sep="")
    options$plot$ylab   <- paste("Price for bucket",b,",$")
    options$table$tex.caption <- paste("Price for bucket",b,",\\$")
    quantiles.by.factor(data, save, options)
  }
  avg.prices <- aggregate(MM[,"PRICES"], by=list(MM$YEAR, MM$MONTH, MM$BUCKET), FUN=mean)
  colnames(avg.prices) <- c("YEAR","MONTH","BUCKET","PRICE")
  avg.prices$MONTH <- factor(avg.prices$MONTH, level=sort(as.numeric(levels(avg.prices$MONTH))))
  
  
  fileName <- paste(save$dir$plots,"tlp_avg_prices.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8, height=6)}
  aux <- trellis.par.get()
  bkg.col <- trellis.par.get("background")
  bkg.col$col <- "white"
  trellis.par.set("background", bkg.col)
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col  <- c("blue", "red", "black", "green")
  sym.col$pch <- c(1,2,3,4)
  trellis.par.set("superpose.symbol", sym.col)
  print(xyplot(PRICE ~ MONTH | YEAR, data=avg.prices, groups=BUCKET, type="l",
    col=sym.col$col  <- c("blue", "red", "black", "green"),
    main=options$unit.name, cex.main=1,           
    key=list(lines=Rows(sym.col, 1:length(uBuckets)),
             text=list(uBuckets),
             columns = length(uBuckets))))

  if (save$Analysis$all){dev.off()}
}


#   variables <- c("GENERATION", "REVENUE", "COST", "NETREVENUE", "FUELCONSUMED",
#                  "VOM", "STARTCOSTS")
#   MM.yyyy   <- aggregate(MM[,which(colnames(MM)%in%variables)],
#                    by=list(MM$YEAR, MM$ITERATION), FUN=sum)
#   colnames(MM.yyyy)[1:2] <- c("YEAR","ITERATION")
 
##   #-------------------------------------------------------------------------------------
##   data <- data.frame(value=MM$net.peak/options$scale.factor,factor=factor(MM$YEAR))
##   save$filename.table <- paste(save$dir$tables, "net_peak", sep="")
##   save$filename.plot  <- paste(save$dir$plots, "net_peak", sep="")
##   options$plot$ylab   <- "Onpeak Net PV, M$"
##   options$table$tex.caption <- "Onpeak net present value, M\\$"
##   quantiles.by.factor(data, save, options)

##   data <- data.frame(value=MM$net.offpeak/options$scale.factor,factor=factor(MM$YEAR))
##   save$filename.table <- paste(save$dir$tables, "net_offpeak", sep="")
##   save$filename.plot  <- paste(save$dir$plots, "net_offpeak", sep="")
##   options$plot$ylab   <- "Offpeak Net PV, M$"
##   options$table$tex.caption <- "Discounted offpeak net present value, M\\$"
##   quantiles.by.factor(data, save, options)

##   data <- data.frame(value=(MM$net.peak + MM$net.offpeak)/options$scale.factor,
##                      factor=factor(MM$YEAR))
##   save$filename.table <- paste(save$dir$tables, "net_total", sep="")
##   save$filename.plot  <- paste(save$dir$plots, "net_total", sep="")
##   options$plot$ylab   <- "Net PV, M$"
##   options$table$tex.caption <- "Discounted total net present value, M\\$"
##   quantiles.by.factor(data, save, options)
