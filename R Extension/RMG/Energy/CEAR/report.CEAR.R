# Put togethers some tables and plots for a given portfolio. 
#
#
#
# Written by Adrian Dragulescu on 8-Jul-2005

report.CEAR <- function(sim.Payoff, hist.Payoff, Position, save, options)
{

  #--------------------------------Summarize Positions
  write.csv(Position, file=paste(save$dir, "quantities.csv", sep=""))
  aux <- data.frame(MW=c(-Position$MW, Position$MW), Bucket=c(Position$Class, Position$Class),
                    node=c(as.character(Position$Source), as.character(Position$Sink)))
  aux <- tapply(aux$MW, list(aux$node, aux$Bucket), sum)
  colnames(aux) <- c("PEAK","OFFPEAK")
  rownames(aux) <- paste("node",rownames(aux))

  TTable <- list()
  file.remove(paste(save$dir, "quantities.tex",sep=""))
  for (index in 1: ceiling(dim(aux)[1]/30))
  {
     BeginningIndex <- (index-1)*30 + 1
     EndingIndex <- min(index*30,dim(aux)[1])
     TTable[[index]] <- aux[BeginningIndex: EndingIndex, ]
     Tcaption=paste("Quantity by node and bucket table",as.character(index),"of",as.character(ceiling(dim(aux)[1]/30)),sep=" ")
     print(xtable(TTable[[index]],caption = Tcaption),
           file=paste(save$dir, "quantities.tex",sep=""),append = T)
  }

  #--------------------------------For Simulatoin Only

       unit.Payoff <- sim.Payoff$unitPayoff
       payoff <- sim.Payoff$sumPayoff

  #--------------------------------Table of unique paths
  paths <- paste("path.", 1:nrow(unit.Payoff), sep="")
  rownames(Position$MW) <- NULL
  aux <- data.frame(Path=paths, MW=Position$MW, Bucket=Position$Class, Type=Position$Type,
                    Source=Position$Source, Sink=Position$Sink,
                    #SourceName=Position$SourceName,SinkName=Position$SinkName,
                    Awarded=I(Position$Awarded),
                    Awarded.Price= Position$Cleared.Price,
                    Realized.Price=Position$Realized.Price)

  TTable <- list()
  file.remove(paste(save$dir, "allpaths.tex", sep=""))
  for (index in 1: ceiling(dim(aux)[1]/30))
  {
     BeginningIndex <- (index-1)*30 + 1
     EndingIndex <- min(index*30,dim(aux)[1])
     TTable[[index]] <- aux[BeginningIndex: EndingIndex, ]
     Tcaption <- paste("All unique paths","table",as.character(index),"of",as.character(ceiling(dim(aux)[1]/30)),
                       "Simulation assumes Zero prices for non-awarded bids",
                       "Realized prices are assumed to be zero if history is not available",
                       sep=" ")
     print(xtable(TTable[[index]], caption = Tcaption),
           file=paste(save$dir, "allpaths.tex", sep=""),append = T)
  }

  #--------------------------------Portfolio Payoff
  PPayoff <- apply(payoff, 2, sum)
  HPayoff <- sum(hist.Payoff[[2]])
  probs   <- c(0,0.01, 0.05, seq(0.1,0.9, by=0.1), 0.95, 0.99, 1)
  qpayoff <- data.frame(Probability = probs,
                        Portfolio.Value=round(quantile(PPayoff, probs=probs),digits=0))
  rownames(qpayoff) <- 1:nrow(qpayoff)
  qpayoff<-rbind(qpayoff,c("Realized.Hist",HPayoff))
  qpayoff$Portfolio.Value <- as.numeric(qpayoff$Portfolio.Value)

  TTable <- list()
  file.remove(paste(save$dir, "portfolioQuantiles.tex",sep=""))
  for (index in 1: ceiling(dim(qpayoff)[1]/30))
  {
     BeginningIndex <- (index-1)*30 + 1
     EndingIndex <- min(index*30,dim(qpayoff)[1])
     TTable[[index]] <- qpayoff[BeginningIndex: EndingIndex, ]
     TTable[[index]]$Portfolio.Value <- TTable[[index]]$Portfolio.Value/1000
     print(xtable(TTable[[index]],
           caption=paste("Quantiles of simulated portfolio values in thousand dollars table",
                          as.character(index),"of",ceiling(dim(qpayoff)[1]/30),sep=" ")),
           file=paste(save$dir, "portfolioQuantiles.tex",sep=""),append = T)
  }

  #--------------------------------Plot the unit Payoff

  data <- data.frame(value =stack(as.data.frame(unit.Payoff))$values,
                    factor=factor(rep(paths, options$noSims), levels=paths))
  options$probs <- c(0,0.01,0.05,0.25, 0.5, 0.75, 0.95,0.99,1)
  options$plot$xlab <- "Path"
  options$plot$ylab <- "Spread price distribution"
  options$plot$main <- options$run.name
  options$table$tex.caption <- "Quantiles of simulated unit payoff (Q=1MW) by path"
  options$table$tex.digits  <- 0
  options$table$rotate <- 1
  save$filename.table <- paste(save$dir, "unit_payoff_by_path", sep="")
  save$filename.plot  <- paste(save$dir, "unit_payoff_by_path", sep="")
  save$Analysis$plots <- 1
  PlotCaption<-"Distribution of simulated unit payoff (Q=1MW) by path. Solid circles mark the means of simulated distributions.  Empty Triangles mark history."
  quantiles.by.factor.CEAR(data, hist.Payoff[[1]], save, options, PlotCaption )

  #--------------------------------Plot the total Payoff
  data  <- data.frame(value =stack(as.data.frame(payoff))$values,
                    factor=factor(rep(paths, options$noSims), levels=paths))
  options$plot$xlab <- "Path"
  options$plot$ylab <- "Payoff distribution"
  options$plot$main <- options$run.name
  options$table$tex.caption <- "Quantiles of simulated payoff by path in thousand dollars"
  options$table$rotate <- 1
  save$filename.table <- paste(save$dir, "payoff_by_path", sep="")
  save$filename.plot  <- paste(save$dir, "payoff_by_path", sep="")
  save$Analysis$plots <- 1
  PlotCaption<-"Distribution of payoffs by path. Solid circles mark the means of simulated distributions.  Empty Triangles mark history."
  hist.Payoff[[2]] <- round(hist.Payoff[[2]]/1000,0)
  data$value <- round(data$value/1000,0)
  quantiles.by.factor.CEAR(data, hist.Payoff[[2]], save, options, PlotCaption)

}
