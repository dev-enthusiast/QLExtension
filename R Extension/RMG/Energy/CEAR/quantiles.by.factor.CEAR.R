# Calculates the quantiles of a variable given a factor,
# and makes a violin plot.
# Where data is a dataframe with columns [value factor]
#       save$filename.table
#       save$filename.plot
#
# OutT is a table with [factors x probs]
#
# Written by Adrian Dragulescu on 24-Mar-2005


quantiles.by.factor.CEAR <- function(data,hist.Payoff,save,options, PlotCaption){

  if (nargs()==1){ options <- list(); save <- list()}
  if (length(options$probs)==0){options$probs <- c(0,0.05,seq(0.1,0.9,0.1),0.95,1)}
  if (length(options$total.line)==0){options$total.line <- 0}# replace All w Total
  if (length(options$table)==0){options$table <- NULL}
  if (length(options$table$rotate)==0){options$table$rotate <- TRUE}
  if (length(save$Analysis$all)==0){save$Analysis$all <- 1}
  if (length(save$Analysis$tex)==0){save$Analysis$tex <- 1}
  if (length(save$Analysis$plots)==0){save$Analysis$plots <- 1}
  if (length(options$plot$main)==0){options$plot$main <- ""}
  if (length(options$plot$ylab)==0){options$plot$ylab <- ""}
  if (length(options$plot$rotate.ylab)==0){options$plot$rotate.xlab <- 2
  } else {options$plot$rotate.xlab <- 1}
  if (length(options$table$tex.caption)==0){options$table$tex.caption <- ""}
  if (length(options$table$tex.digits)==0){options$table$tex.digits <- 2}
  if (length(options$table$fontsize)==0){options$table$fontsize <- "small"}
  
  myMean  <- function(x){mean(x, na.rm=T)}
  mySum  <- function(x){sum(x, na.rm=T)}  
  myQtile <- function(x){quantile(x, probs=options$probs, na.rm=T)}

  lvls <- levels(data$factor)
  OutT <- tapply(data$value, data$factor, myQtile)
  OutT <- matrix(unlist(OutT), nrow=length(options$probs), ncol=length(lvls),
                 dimnames=list(100*options$probs, lvls))
  OutTT<-rbind(OutT, Mean = apply(OutT,2,myMean))
  OutTT<-rbind(OutTT, RealizedHist = t(hist.Payoff))
  rownames(OutTT)[dim(OutTT)[1]] = "Realized.Hist"

  if (options$total.line){OutTT[nrow(OutTT),ncol(OutTT)] <- myMean(total.values)}  
  aux <- cbind(Quantile=c(100*options$probs, "Mean", "History"), OutTT)
  if (options$table$rotate){OutTT <- t(OutTT)}
  if (save$Analysis$all){
    filename <- paste(save$filename.table,".csv", sep="")
    write.table(aux, filename, sep=",", quote=F, row.names=F)
  }

  if (save$Analysis$tex & save$Analysis$plots)
  {
    require(xtable) 
    require(Simple)

    TTable <- list()
    TPlot <- list()
    filenameT <- paste(save$filename.table,".tex", sep="")
    file.remove(filenameT)

    for (index in 1: ceiling(dim(OutTT)[1]/30))
    {
       BeginningIndex <- (index-1)*30 + 1
       EndingIndex <- min(index*30,dim(OutTT)[1])
       TTable[[index]] <- OutTT[BeginningIndex: EndingIndex, ]
       print(xtable(TTable[[index]],
                    caption=paste(options$table$tex.caption, "table",
                                  as.character(index),"of",ceiling(dim(OutTT)[1]/30),sep=" "),
                    display=c("s", rep("f",ncol(OutTT))),
                    digits=c(0,rep(options$table$tex.digits, ncol(OutTT)))),
             file=filenameT,size=options$table$fontsize,append = T)

       filenameP <- paste(save$filename.plot,as.character(index),".pdf", sep="")
       file.remove(filenameP)
       pdf(filenameP, width=6.5, heigh=5.0, onefile = F )
       par(xaxt="n")
       TPlot[[index]] <- subset(data,factor %in% data$factor[BeginningIndex:EndingIndex], select = value:factor)
       TPlot[[index]]$factor <- factor(TPlot[[index]]$factor, levels = unique(as.character(TPlot[[index]]$factor)))
       lvls <- levels(TPlot[[index]]$factor)
       cat(simple.violinplot(TPlot[[index]]$value ~ TPlot[[index]]$factor, col="wheat1"))
       points(1:length(lvls), tapply(TPlot[[index]]$value, TPlot[[index]]$factor, mean),
        pch=19)

       points(1:length(lvls), OutTT[BeginningIndex: EndingIndex,"Realized.Hist"],pch = 2,col="red")
       par(xaxt="s")
       cat(axis(1, at=1:length(lvls), labels=lvls,
               las=options$plot$rotate.xlab, cex.axis=0.8))
       cat(mtext(options$plot$ylab, side=2, line=2.0, font=2))
       cat(mtext(options$plot$main, side=3, line=2.2))
       graphics.off()

       cat("\\clearpage", file=filenameT, "\n",append=T)
       cat("\\begin{figure}",file=filenameT,"\n",append=T)
       PFile <- paste("\\centerline{\\includegraphics{",as.character(filenameP),"}}",sep="")
       PCaption <- paste("\\caption{", PlotCaption, "}", sep="")
       cat(paste(PFile,PCaption,sep="\n"),file=filenameT,"\n",append=T)
       cat("\\end{figure}",file=filenameT,"\n",append=T)
       cat("\\clearpage",file=filenameT,"\n",append=T)

    }
  }

  return(OutT)
}
