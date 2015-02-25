# Calculate historical correlation between different commodities.
# Where MM is a data.frame with curve.name contract.dt 
#
#
# First version by Adrian Dragulescu on 25-Jun-2007


calc.correlation <- function(MM, options){

  if (length(options$calc$corr$average)==0){
    options$calc$corr$average <- TRUE   # calc an average correlation 
  }
    
  if (!("contract.dt" %in% names(MM))){
    bom <- as.Date(format(options$asOfDate, "%Y-%m-01"))
    contract.dt <- seq(bom, by="month", length.out=25)[-1]   # go for the next 2 yrs
    MM <- expand.grid(curve.name=MM$curve.name, contract.dt=contract.dt)
   }
   MM$vol.type <- MM$position <- "NA"
   MM$curve.name <- as.character(MM$curve.name)

   hPP <- aggregate.run.prices.mkt(MM, options)
   hPP$vol.type <- hPP$position <- NULL
   indDates <- grep("-", names(hPP))
  
   # calculate log-returns
   hPP[,indDates[-1]] <- log(hPP[,indDates[-1]]/hPP[,indDates[-length(indDates)]])
   hPP <- hPP[,-3] # remove the first date    
   indDates   <- grep("-", names(hPP))
   hvol <- apply(hPP[,-(1:2)], 1, sd, na.rm=T)*sqrt(252)
  
   uContracts <- sort(unique(hPP$contract.dt))
   if (length(uContracts)>24){uContracts <- uContracts[1:24]}  # look at first 24 contracts
   res <- vector(mode="list", length=length(uContracts))
   for (ct in 1:length(uContracts)){
     aux <- subset(hPP, contract.dt == uContracts[ct])
     CM  <- cor(t(aux[,indDates]), use="pairwise.complete.obs")
     aux <- expand.grid(aux$curve.name, aux$curve.name)
     aux <- cbind(aux, contract.dt=rep(uContracts[ct], nrow(aux)), cor=as.numeric(CM))
     res[[ct]] <- aux
   }
   res <- do.call("rbind", res)

   if (options$calc$corr$average){
     names(res)[4] <- "value"
     res <- cast(res, Var1 + Var2 ~ ., median)
     names(res)[3] <- "cor"
     uCurves <- as.character(unique(res$Var1))
     res <- matrix(res$cor, nrow=length(uCurves), ncol=length(uCurves),
                   dimnames=list(uCurves, uCurves))
   }
   return(list(res, hvol))
}

##    curve1 <- MM$curve.name[17]
##    curve2 <- MM$curve.name[10]
##    ind <- grep(curve1, as.character(res$Var1))
##    CM  <- res[ind,]
##    ind <- grep(curve2, as.character(CM$Var2))
##    CM  <- CM[ind,]
     


