# Main function to create the tex and csv tables for the VaR report for a
# given run.  Writes the IVaR to xls at the end.  
# 
#
# Written by Adrian Dragulescu on 24-Nov-2006



make.VaR.summary <- function(PnL, class.Factors, file){
  summary.Fun <- function(x){c(4*sd(x), mean(x)-quantile(x, probs=0.05), min(x))}
  OutT <- array(0, dim=c(3,3), dimnames=list(c("4std", "95\\\%", "MaxLoss"),
                                 c("Delta", "Vega", "Total")))
  ind  <- which(class.Factors$type == "Delta")
  if (length(ind)>0){OutT[,1] <- apply(as.matrix(colSums(PnL[ind,])),2, summary.Fun)}
  ind  <- which(class.Factors$type == "Vega")
  if (length(ind)>0){OutT[,2] <- apply(as.matrix(colSums(PnL[ind,])),2, summary.Fun)}
  OutT[,3] <- apply(as.matrix(colSums(PnL)),2, summary.Fun) 
  OutT     <- apply(OutT,2,dollar)# prettify only the numeric variables
  print(xtable(OutT, caption="VaR summary."), file=paste(file, ".tex", sep=""),
    size="small", floating=FALSE, include.rownames=TRUE)
  write.csv(OutT, file=paste(file, ".csv", sep=""), row.names=F)
  return(OutT)
}

write.2D.table <- function(OutT,var.names,VaR,caption,file,add.totals=T,add.to.row=NULL){
  if (!is.data.frame(OutT)){OutT <- data.frame(OutT)}
  colnames(OutT) <- gsub("X","",colnames(OutT)) # if cast was used
  if (add.totals){
    OutT <- cbind(OutT, Total=VaR[[var.names[1]]]$Total$value)
    OutT <- rbind(OutT, c(NA, VaR[[var.names[2]]]$Total$value,VaR$Total))
    OutT[nrow(OutT),1] <- "Total"
  }
  ind <- which(sapply(OutT, is.numeric))
  OutT[,ind] <- apply(OutT[,ind],2,dollar)# prettify only the numeric variables
  OutT[,ind] <- apply(OutT[,ind], 2, function(x){gsub("NA", "  ",x)})
  size <- "small"
  if (ncol(OutT) > 15){size <- "scriptsize"} 
  if (ncol(OutT) > 18){size <- "tiny"}
  print(xtable(OutT, caption=caption), file=paste(file, ".tex", sep=""), size=size,
    tabular.environment="longtable", floating=FALSE, include.rownames=FALSE,
    add.to.row=add.to.row)
  write.csv(OutT, file=paste(file, ".csv", sep=""), row.names=F)
}

make.VaR.report <- function(IVaR, class.Factors, curve.info, options){

  if (length(options$summary.only)==0){options$summary.only <- FALSE}
  indDates <- grep("-",colnames(IVaR$Changes))
  dP  <- IVaR$Changes[,indDates]
  PnL <- dP * IVaR$Changes$position
  VaR <- list(Total=4*sd(colSums(PnL, na.rm=T)))

  #----------------------------------------- can do summary VaR only! -----
  file <- paste(options$save$reports.dir.run, "var_summary", sep="")
  summary.VaR <- make.VaR.summary(PnL, class.Factors, file)
  if (options$summary.only){return(summary.VaR)}

  #--------------- VaR by one classification factor -----------------------
  for (i in 1:ncol(class.Factors)){
    cFactor <- data.frame(class.Factors[,i])
    colnames(cFactor) <- colnames(class.Factors)[i]
    VaR[[colnames(cFactor)]] <- list(Total=core.calc.VaR(PnL, cFactor))
  }
  #--------------- VaR by market and year----------------------------------
  res  <- core.calc.VaR(PnL, class.Factors[,c("market","year")])
  VaR[["market"]] <- c(VaR[["market"]], list(year=list(Total=res)))
  OutT <- cast(res, market ~ year)
  OutT$market <- as.character(OutT$market)
  caption <- "VaR by market and year."  
  file <- paste(options$save$reports.dir.run, "var_by_market_year", sep="")
  write.2D.table(OutT, c("market", "year"), VaR, caption, file)

  #--------------- VaR by curve.name and year-----------------------------
  res  <- core.calc.VaR(PnL, class.Factors[, c("curve.name","year")])
  VaR[["curve.name"]] <- c(VaR[["curve.name"]], list(year=list(Total=res)))
  OutT <- cast(res, curve.name ~ year)
  OutT$curve.name <- as.character(OutT$curve.name) 
  caption <- "VaR by commodity name and year."  
  file <- paste(options$save$reports.dir.run, "var_by_commodityname_year", sep="")
  add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
  write.2D.table(OutT, c("curve.name", "year"), VaR, caption, file)
  
  #--------------- VaR by curve.name and type-----------------------------
  res  <- core.calc.VaR(PnL, class.Factors[, c("curve.name","type")])
  VaR[["curve.name"]] <- c(VaR[["curve.name"]], list(type=list(Total=res)))
  OutT <- cast(res, curve.name ~ type)
  OutT$curve.name <- as.character(OutT$curve.name) 
  caption <- "VaR by commodity name and type."  
  file <- paste(options$save$reports.dir.run, "var_by_commodityname_type", sep="")
  add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
  write.2D.table(OutT, c("curve.name", "type"), VaR, caption, file, TRUE, add.to.row)

  #--------------- VaR by curve.name, type, and year----------------------
  res  <- core.calc.VaR(PnL, class.Factors[, c("curve.name", "type", "year")])
  VaR[["curve.name"]] <- c(VaR[["curve.name"]],
                            list(type= list(year= list(Total=res))))
  res  <- rbind(res, cbind(VaR$curve.name$year$Total, type="Total"))
  res  <- rbind(res, cbind(VaR$curve.name$type$Total, year="Total"),
                     cbind(VaR$curve.name$Total, type="Total", year="Total"))
  OutT <- cast(res, curve.name + type ~ year)
  caption <- "VaR by commodity name, type and year."  
  file <- paste(options$save$reports.dir.run, "var_by_commodityname_type_year", sep="")
  add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
  write.2D.table(OutT, NULL, VaR, caption, file, add.totals=F, add.to.row)

  if ("gasseason" %in% names(class.Factors)){
    #--------------- VaR by curve.name and gas season -----------------------------
    res  <- core.calc.VaR(PnL, class.Factors[, c("curve.name","gasseason")])
    VaR[["curve.name"]] <- c(VaR[["curve.name"]], list(gasseason=list(Total=res)))
    OutT <- cast(res, curve.name ~ gasseason)
    OutT$curve.name <- as.character(OutT$curve.name) 
    caption <- "VaR by commodity name and gasseason."  
    file <- paste(options$save$reports.dir.run, "var_by_commodityname_gasseason", sep="")
    add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
    write.2D.table(OutT, c("curve.name", "gasseason"), VaR, caption, file, FALSE, add.to.row)
    
    #--------------- VaR by curve.name, type, and gas season ----------------------
    res  <- core.calc.VaR(PnL, class.Factors[, c("curve.name", "type", "gasseason")])
    VaR[["curve.name"]] <- c(VaR[["curve.name"]],
                            list(type= list(gasseason= list(Total=res))))
    res  <- rbind(res, cbind(VaR$curve.name$gasseason$Total, type="Total"))
    res  <- rbind(res, cbind(VaR$curve.name$type$Total, gasseason="Total"),
                     cbind(VaR$curve.name$Total, type="Total", gasseason="Total"))
    OutT <- cast(res, curve.name + type ~ gasseason)
    caption <- "VaR by commodity name, type and gas season."  
    file <- paste(options$save$reports.dir.run, "var_by_commodityname_type_gasseason", sep="")
    add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
    write.2D.table(OutT, NULL, VaR, caption, file, add.totals=F, add.to.row)
  }
  if ("in.20072008" %in% names(class.Factors)){
    #--------------- VaR by in.20072008 and type ----------------------------------
    res  <- core.calc.VaR(PnL, class.Factors[,c("in.20072008","type")])
    VaR[["in.20072008"]] <- c(VaR[["in.20072008"]], list(type=list(Total=res)))
    OutT <- cast(res, in.20072008 ~ type)
    OutT$in.20072008 <- as.character(OutT$in.20072008)
    caption <- "VaR by 2007-2008 classification and type."  
    file <- paste(options$save$reports.dir.run, "var_by_20072008_type", sep="")
    write.2D.table(OutT, c("in.20072008", "type"), VaR, caption, file)
  }
  if ("hub.basis" %in% run$classification){
    # --------------------------- MTM = Q_A*(P_A - P_H) + Q_B*(P_B - P_H) + (Q_H+Q_A+Q_B)*P_H
    aux <- IVaR$Changes; aux$ind.nrow <- 1:nrow(aux)
    curve.info <- merge(aux[, c("curve.name", "contract.dt", "vol.type", "ind.nrow")],
                        curve.info, all.x=T)
    ind <- curve.info$curve.name == curve.info$hub.curve & is.na(curve.info$vol.type)
    curve.info$is.Hub <- ifelse(ind, TRUE, FALSE)
    
    aux$ind.Hub <- 1:nrow(aux); names(aux)[1] <- "hub.curve"
    curve.info <- merge(curve.info, aux[, c("hub.curve", "contract.dt", "vol.type", "ind.Hub")], all.x=T)
    curve.info <- curve.info[order(curve.info$ind.nrow),]
    
    ind.withHub <- which(!is.na(curve.info$ind.Hub) & !curve.info$is.Hub)
    aux[ind.withHub, indDates] <- aux[ind.withHub, indDates] -   # substract Hub price changes 
       aux[curve.info$ind.Hub[ind.withHub], indDates]            # from the basis curves
    Q.by.hub <- tapply(aux$position[!curve.info$is.Hub], # sum all basis positions to their hub
       list(curve.info$ind.Hub[!curve.info$is.Hub]), sum)
    ind <- na.omit(curve.info$ind.Hub[curve.info$is.Hub])
    ind <- ind[ind %in% rownames(Q.by.hub)]
    aux$position[ind] <- aux$position[ind] + Q.by.hub  # add basis from the hub positions
    
    PnL <- aux[,indDates] * aux$position
    cF  <- data.frame(season=class.Factors[, c("gasseason")], region=curve.info$region) 
    res <- core.calc.VaR(PnL, cF)
    OutT <- cast(res,  region ~ season)
    caption <- "Basis VaR by region and season."  
    file <- paste(options$save$reports.dir.run, "basis_var_by_region_season", sep="")
    add.to.row <- list(); add.to.row$pos <- list(0); add.to.row$command <- c("\\endhead \n")
    write.2D.table(OutT, c("region", "season"), VaR, caption, file, FALSE, add.to.row)
  }

  
  #----------------------------------------------save the IVaR results-----------
  if (length(options$save$IVaR.filename)>0){
     write.xls(IVaR$Prices, options$save$IVaR.filename, sheetname="Prices")
     write.xls(IVaR$Changes, options$save$IVaR.filename, sheetname="Changes")
  }

}


#   mPnL <- melt(data.frame(class.Factors, PnL))
#   source(paste(options$source.dir, "core.calc.VaR.R", sep=""))
#   res  <- core.calc.VaR(mPnL, c("market", "year"))
#   res
#   caption <- "VaR by market and year."  
#   file <- paste(options$save$reports.dir.run, "var_by_market_year.tex", sep="")
#   write.tex.table(OutT, caption, file)
  
#   res  <- core.calc.VaR(mPnL, c("year", "market"))
#   res
#   res  <- core.calc.VaR(mPnL, c("year"))
#   res


#   caption <- "VaR by market and year."  
#   file <- paste(options$save$reports.dir.run, "var_by_market_year.tex", sep="")
#   write.tex.table(OutT, caption, file)


  
