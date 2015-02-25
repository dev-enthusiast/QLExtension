create.FEA.file <- function(secSymbol="PRC_NG_EXCHANGE", dateSt=as.Date("2006-1-1"),
    dateEnd=as.Date("2008-1-1"), GDM="GasDailyMean_HENRY",  output="C:/temp/", write=TRUE){

# creates FEA data file that can be used in the storage model.
# author: Wouter Buitenhuis
# created: Nov, 8th, 2007
# last updates: Feb 28th, 2008

    print("create.FEA.file. Version Feb 28th 2008")
    require(SecDb)
    secDB.expirations  <- seq(from=dateSt, to=dateEnd, by="months")[-1] #removes first expiration to elliminate empty data returns.
    sec.symbol.vector <- c(GDM, paste(secSymbol,"_", substr(secDB.expirations,3,4), substr(secDB.expirations,6,7), sep=""))
    data <- tsdb.readCurve(sec.symbol.vector[1], dateSt, Sys.Date()-1)
    if (nrow(data)==0) print(paste(sec.symbol.vector[1], "did not return any data. Could it be a typo?"))
    colnames(data) <- c("date", sec.symbol.vector[1])
    n <- length(sec.symbol.vector)
    if (n > 1) {
      for (i in 2:length(sec.symbol.vector)) {
        new.data <- tsdb.readCurve(sec.symbol.vector[i], dateSt, dateEnd)
        if (nrow(new.data)==0) print(paste(sec.symbol.vector[i], "did not return any data. Could it be a typo?"))
        colnames(new.data) <- c("date", sec.symbol.vector[i])
        data <- merge(data, new.data, by="date", all=TRUE)
      }
    }
    dates <- rownames(data) <- data$date
    ndates <- length(data$date)
    dates <- sort(rep(dates, n)) #this is a vector with the observation dates
    dates.FEA.format <- format(dates, "%m/%d/%Y") # these are the observation dates in FEA format
    FEA.start <- seq(from=dateSt, to=(dateSt + 32), by="months")[2]
    FEA.end <- dateEnd
    FEA.expirations <- seq(from=FEA.start, to=FEA.end, by="months") - 1
    contract.dates.FEA.format <- contract.dates <- rep(c("N/A", as.character(FEA.expirations)), ndates)    
    contract.dates.FEA.format[contract.dates!="N/A"] <- format(as.Date(contract.dates[contract.dates!="N/A"]), "%m/%d/%Y") # reformat contract dates in FEA format
    valid.obs <- rep(TRUE,ndates*n)
    not.spot <- contract.dates != "N/A"
    BOM <- rep(FALSE,ndates*n)
    BOM[not.spot] <- substr(dates[not.spot],1,7) == substr(as.Date(contract.dates[not.spot]) + 1,1,7)
    valid.obs[not.spot] <- (as.Date(dates[not.spot]) <= as.Date(contract.dates[not.spot])) |  (BOM[not.spot]==T) #This may have been an ugly fix and not necessary
    contract.dates.FEA.format[BOM] <- "B"
    data <- data[-1]
    data <- round(data, digits=4)
    data <- matrix(t(data), ncol=1)
    data <- data.frame(dates.FEA.format, contract.dates.FEA.format, data)
    data <- data[valid.obs,] #may be ommited?
    colnames(data) <- c("date", "contract", "value")
    data <- na.omit(data)
    filename = paste(output, secSymbol, ".txt", sep="")
    
    prompt.ind <- which(paste(substr(data$date,1,2),substr(data$date,7,10)) == 
          paste(substr(data$contract,1,2),substr(data$contract,7,10)))
    spot.ind <- which(data$contract=="N/A")
    spot <- data.frame(data[spot.ind,"date"], data[spot.ind,"value"])
    prompt <- data.frame(data[prompt.ind,"date"], data[prompt.ind,"value"])
    colnames(spot) <- colnames(prompt) <- c("date", "value")
    all.data <- merge(spot, prompt, by.x="date", by.y="date", all=FALSE, sort=FALSE)
    colnames(all.data) <- c("date", "spot" , "prompt")
    rownames(all.data) <- all.data$date
    all.data <- all.data[-1] #remove date
    n <- nrow(all.data)
    data.tp1 <- all.data[c(-1,-2),] #remove first 2 obs
    data.tm1 <- all.data[c(-(n-1),-n),]
    data.t <- all.data[c(-1,-n),]
    rownames(data.tp1) <- rownames(data.tm1) <- rownames(data.t)
    spread.t <- log(data.t$spot / data.t$prompt)
    spread.t.tp1 <- log(data.tp1$spot / data.t$prompt)
    beta <- coef(lm("spread.t.tp1 ~ spread.t", data.frame(spread.t.tp1, spread.t)))[2]
    spot.resid <- spread.t.tp1 - beta * spread.t
    prompt.resid <- log(data.t / data.tm1 )$prompt
    spot.resid <- spot.resid[-length(prompt.resid)]
    prompt.resid <- prompt.resid[-1]

#    data <- data[-nrow(data),]  # remove last observation
#    spread.next <- log(data$prompt / next.data$spot) # 
#    spread <- log(data$spot / data$prompt)
#    beta <- coef(lm("spread.lag ~ spread", data.frame(spread.lag, spread)))[2]
#    prompt.resid <- log(data / lagged.data)$prompt
#    spot.resid <- spread.lag -exp(beta)*spread

    res <- c(-log(beta) * 252, cor(spot.resid, prompt.resid))
    names(res) <- c("spot.MR.rate", "spot.prompt.correl")
    print(res)
    if (write==T) {
      write.table(data, file=filename, sep = "\t", eol = "\n", dec=".",
         row.names = F, col.names=F, quote=F)
    } else {
      return(data)
    }
}