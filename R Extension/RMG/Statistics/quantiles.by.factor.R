# Calculates the quantiles of a variable given a factor,
# and makes a violin plot.
# Where data is a dataframe with columns [value factor]
#       save$filename.table
#       save$filename.plot
#
# OutT is a table with [factors x probs]
#
# Written by Adrian Dragulescu on 24-Mar-2005


quantiles.by.factor <- function(data,save,options){

  if (nargs()==1){ options <- list(); save <- list()}
  if (length(options$probs)==0){options$probs <- c(0,0.05,seq(0.1,0.9,0.1),0.95,1)}
  if (length(options$probs.csv)==0){options$probs.csv <- options$probs}  
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
  if (!identical(options$probs.csv, options$probs)){
    OutT.csv <- tapply(data$value, data$factor, quantile,
                       probs=options$probs.csv, na.rm=T)
    OutT.csv <- matrix(unlist(OutT.csv), nrow=length(options$probs.csv),
                ncol=length(lvls), dimnames=list(100*options$probs.csv, lvls))   
  }
  if (options$total.line){
    aux <- matrix(data$value, ncol=length(unique(data$factor)))
    total.values <- apply(aux, 1, mySum)
    OutTT <- cbind(OutT, Total=myQtile(total.values))    
  } else {
    OutTT <- cbind(OutT, All=myQtile(data$value))
  }
  OutTT <- rbind(OutTT, Mean=c(tapply(data$value, data$factor, myMean),
                        myMean(data$value)))
  if (options$total.line){OutTT[nrow(OutTT),ncol(OutTT)] <- myMean(total.values)}  
  aux <- cbind(Quantile=c(100*options$probs, "Mean"), OutTT)
  if (options$table$rotate){OutTT <- t(OutTT)}
  if (save$Analysis$all){
    filename <- paste(save$filename.table,".csv", sep="")
    if (identical(options$probs.csv, options$probs)){
      write.table(aux, filename, sep=",", quote=F, row.names=F)
    } else {
      write.csv(OutT.csv, filename)     
    }
  }
  if (save$Analysis$tex){
    require(xtable) 
    filename <- paste(save$filename.table,".tex", sep="")
    print.xtable(xtable(OutTT, caption=options$table$tex.caption,
      display=c("s", rep("f",ncol(OutTT))),
      digits=c(0,rep(options$table$tex.digits, ncol(OutTT)))),
      file=filename, size=options$table$fontsize)
  }
  if (save$Analysis$plots){
    require(Simple) 
    filename <- paste(save$filename.plot,".pdf", sep="")
    pdf(filename, width=6.5, heigh=5.0)
    par(xaxt="n")
    simple.violinplot(data$value ~ data$factor, col="wheat1")
    points(1:length(lvls), tapply(data$value, data$factor, median, na.rm=T),
      pch=19) 
    par(xaxt="s")
    axis(1, at=1:length(lvls), labels=lvls,
             las=options$plot$rotate.xlab, cex.axis=0.8)
    mtext(options$plot$ylab, side=2, line=2.0, font=2)
    mtext(options$plot$main, side=3, line=2.2)
    dev.off()
  }
  
  return(OutT)
}
