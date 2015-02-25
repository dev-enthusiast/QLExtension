#
# EO.by_maskedNodeId  - extract one unit in P, MW, segment form
# EO.by_month         - cast the EO by month in P, MW, segment form
# EO.clean_PQ_pairs_of_NAs
# EO.get_unique_participantsMonth
#
# analyze_NorthfieldMountain
# analyze_SalemHarbor
#
# get_cleared_mw_lessThanHub
# get_cleared_mw_lessThanHR
# get_must_take_mw
#
# subsetMaskedNodeId  - filter files by a MaskedNodeId
# subsetParticipantId - filter files by a participant id 
#

##############################################################
# One masked ID at a time, sift through the monthly files, 
# Cast the data to keep only the P, MW, segment
# If it's been processed already don't do it again.
# 
EO.by_maskedNodeId <- function(maskedNodeId, redo=FALSE)
{
  files <- list.files(DIR_EO, full.names=TRUE, pattern="^DB_")
  
  fname <- paste(DIR_EO, "Split/", maskedNodeId, ".RData", sep="")
  if (file.exists(fname)){
    load(fname)
    # subset existing data with the files
    uMonths <- unique(format(EO$datetime - 60*60, "%Y%m")) # time is HE!
    if (redo) {
      fProcessed <- NULL
    } else {  
      fProcessed <- paste(DIR_EO, "DB_", uMonths, ".RData", sep="")
    } 
    files <- setdiff(files, fProcessed)
    if (length(files)==0){
      rLog("All done.  Exiting")
      return()
    }  
  } else {
    EO <- NULL      # energy offers variable
  }

  X <- finalize(files, subsetMaskedNodeId, agg="rbind", maskedNodeId)
  X <- EO.clean_PQ_pairs_of_NAs(X)
  cnames <- as.vector(outer(c("P.", "MW."), 1:10, paste, sep=""))
  ind <- c(1, 3, which(colnames(X) %in% cnames))
  X <- X[, ind]
  ind <- which(is.na(X$datetime))
  if (length(ind)>0)
    X <- X[-ind, ] # some NA crap from daylight savings

  Y <- melt(X, id=1:2)
  Y <- subset(Y, !is.na(value))
  Y$segment  <- gsub(".*\\.(.*)", "\\1", Y$variable)
  Y$variable <- gsub("(.*)\\..*", "\\1", Y$variable)
  #subset(Y, datetime == as.POSIXct("2011-03-14 01:00:00"))
  Y <- cast(Y, datetime + nodeId + segment ~ variable, I, fill=NA) # 2 mins
     
  Y <- Y[order(Y$datetime, Y$P),]
  Y <- data.frame(Y)

  # put them together
  EO <- rbind(EO, Y)
  EO <- EO[order(EO$datetime, EO$P),]
 
  # save the file back
  save(EO, file=fname)
  rLog("Wrote file", fname)
}


##############################################################
# make the long file ...
#
EO.by_month <- function(fname, outdir, NO_BLOCKS=4)
{
  rLog("Working on", fname)
  month <- gsub("DB_(.*)\\.RData", "\\1", basename(fname))
  fnameLong <- paste(outdir, "RData/Long/long_", month, ".RData", sep="")
  if (file.exists(fnameLong)){
    rLog("File exists. Done.")
    return()
  }
  
  load(fname)
  
  cnames <- as.vector(outer(c("P.", "MW."), 1:10, paste, sep=""))
  ind <- which(colnames(res) %in% c("datetime", "nodeId", cnames))
  res <- res[, ind]
  ind <- which(is.na(res$datetime))
  if (length(ind)>0)
    res <- res[-ind, ] # some NA crap from daylight savings

  uNodeId <- unique(res$nodeId)
  BLOCK_SIZE <- ceiling(length(uNodeId)/NO_BLOCKS)
  
  EO <- vector("list", length=NO_BLOCKS)
  for (b in 1:NO_BLOCKS){
    rLog("  block", b)
    ind <- ((b-1)*BLOCK_SIZE+1):min((b*BLOCK_SIZE),length(uNodeId))
    nodes <- uNodeId[ind]
    aux <- melt(subset(res, nodeId %in% nodes), id=1:2)
    aux <- subset(aux, !is.na(value))
    aux$segment  <- gsub(".*\\.(.*)", "\\1", aux$variable)
    aux$variable <- gsub("(.*)\\..*", "\\1", aux$variable)
    aux <- cast(aux, datetime + nodeId + segment ~ variable, I, fill=NA) 
    # some extra values due to daylight savings time
    #aux <- aux[order(aux$datetime, aux$P),]
    EO[[b]] <- data.frame(aux)
  }
  EO <- do.call("rbind", EO)
  EO <- EO[order(EO$datetime, EO$nodeId, EO$P),]
  rownames(EO) <- NULL
  
  save(EO, file=fnameLong)
  rLog("Wrote", fnameLong)  
}


##############################################################
EO.clean_PQ_pairs_of_NAs <- function(X)
{
  cnames <- as.vector(outer(c("P.", "MW."), 1:10, paste, sep=""))
  ind <- which(colnames(X) %in% cnames)
  ind.rm <- NULL        # remove P,Q segments with NA values
  for (i in ind){
     if (all(is.na(X[,i])))
       ind.rm <- c(ind.rm, i)
  }
  if (length(ind.rm)>0)
    X <- X[,-ind.rm]

  X
}


##############################################################
EO.get_unique_participantsMonth <- function(files=NULL)
{
  if (is.null(files))
    files <- list.files(DIR_EO, full.names=TRUE, pattern="^DB_")

  FUN <- function(file=file){
    yyyymm <- gsub(".*_(.*)\\.RData", "\\1", file)
    month <- as.Date(paste(yyyymm, "01", sep=""), format="%Y%m%d")
    load(file)
    aux <- data.frame(month=as.character(month),
      participantId=sort(unique(res$participantId)))
    aux
  }
  res <- finalize(files, FUN)

  fileOut <- paste(DIR_EO,"../uParticipantsMonth.csv", sep="")
  write.csv(res, file=fileOut, row.names=FALSE)
  rLog("Wrote file", fileOut)
  
  res  
}  


##############################################################
# Unit 1 = 250 MW coal
# Unit 2 = 250 MW coal
# Unit 3 = 640 MW coal
# Unit 4 = 445 MW oil
analyze_Brayton <- function()
{
  maskedNodeIds <- c(43551, 59958, 23789, 43414)
  names(maskedNodeIds) <- c("Unit 1", "Unit 2", "Unit 3", "Unit 4 (Oil)")
  for (id in maskedNodeIds) EO.by_maskedNodeId(id)  # extract the Energy Offers
  
  files <- paste(DIR_EO, "Split/", maskedNodeIds, ".RData", sep="")
  EO <- finalize(files, function(file=file){load(file); EO})
  EO <- merge(EO, HUB[,c("datetime", "hub")], by="datetime")
  EO <- merge(EO, data.frame(nodeId=maskedNodeIds, unit=names(maskedNodeIds)))

  # pick only one hour during the day, for the coal units only
  EO$HE <- as.numeric(format(EO$datetime, "%H"))
  eo <- subset(EO, HE==17 & nodeId %in% c(43551, 59958, 23789))

  
  # get cleared mw less than hub price
  qL <- cast(EO, datetime ~ ., sum, value="MW", subset=P<=hub)
  names(qL)[2] <- "MW<Hub"
  summary(qL$`MW<Hub`)
  xyplot(`MW<Hub` ~ datetime, data=qL, type=c("g", "l"),
    subset=datetime >= as.POSIXct("2008-01-01"))

  # get the average offer price of the coal units
  aux <- subset(EO, nodeId %in% c(43551, 59958, 23789))
  avgP <- ddply(aux, .(datetime), function(x){
    data.frame(avgP = sum(x$MW * x$P)/sum(x$MW),
               minP = min(x$P),
               maxP = max(x$P))})
  
  
  # plot the price of first block  
  aux <- subset(EO, datetime >= as.POSIXct("2008-01-01"))
  xyplot(P ~ datetime | unit, data=aux, 
    subset=segment=="1" & nodeId %in% c(43551, 59958, 23789) & P < 100, 
    layout=c(1,3), type=c("g", "p"),
    ylab="Price of first energy offer point, $/MWh",
    xlab="",
    main="Brayton Point, coal units")

  aux <- aux[order(aux$unit, aux$segment, aux$datetime),]
  xyplot(P ~ datetime | unit, data=aux, groups=segment, 
    subset=nodeId %in% c(43551) & P < 300, 
    layout=c(1,1), type=c("g", "l"),
    ylab="Price of first energy offer point, $/MWh",
    xlab="",
    main="Brayton Point, coal units")
  
  

  # how does quantity cleared correlates with hub prices?
  res <- merge(qL, HUB[,c("datetime", "hub")], by="datetime")
  res <- merge(res, avgP, by="datetime")
  plot(res$hub, res$`MW<Hub`)

  aux <- melt(res[,-2], id=1)
  xyplot(value ~ datetime, data=aux, groups=variable,
         type=c("g", "l"),
         subset=datetime >= as.POSIXct("2008-01-01"),
         ylim=c(0, 220))


  xyplot(minP ~ datetime, data=res, type=c("g", "l"),
         subset=datetime >= as.POSIXct("2008-01-01"),
         ylim=c(0, 220))
  
  
}



##############################################################
# 
analyze_Manchester <- function()
{
  maskedNodeIds <- c(37274, 92137, 72183)
  for (id in maskedNodeIds) EO.by_maskedNodeId(id)  # extract the Energy Offers

  files <- paste(DIR_EO, "Split/", maskedNodeIds, ".RData", sep="")
  totalQ <- get_cleared_mw_lessThanHub(files, HUB)

  hr <- seq(6, 19, by=1)
  
  
  aux <- subset(totalQ, datetime > as.POSIXct("2010-05-01"))
  plot(aux$datetime, aux$MW, type="l", col="blue",
    main="Manchester CC", cex.main=1, ylab="MW")
  
}


##############################################################
# 
analyze_Mystic <- function()
{
  maskedNodeIds <- c(60658, 11009, 72020)  # Mystic 7, 8, 9 in this order
  for (id in maskedNodeIds) EO.by_maskedNodeId(id)  # extract the Energy Offers

  files <- paste(DIR_EO, "Split/", maskedNodeIds, ".RData", sep="")
  totalQ <- get_cleared_mw_lessThanHub(files, HUB)

  # only Mystic7
  files <- paste(DIR_EO, "Split/", maskedNodeIds[1], ".RData", sep="")
  totalQ <- get_cleared_mw_lessThanHub(files, HUB)
  
  totalQ <- zoo(totalQ$MW, totalQ$datetime)
  aux <- subset(totalQ, datetime > as.POSIXct("2008-01-01"))
  plot(aux$datetime, aux$MW, type="l", col="blue",
    main="Mystic 7", cex.main=1, ylab="MW in the money")

  
}



##############################################################
# pass in the 
analyze_NorthfieldMountain <- function(X)
{
  maskedNodeIds <- c(53298, 60188, 66602, 83099)

  files <- paste(DIR_EO, "Split/", maskedNodeIds, ".RData", sep="")
  EO <- finalize(files, function(file=file){load(file); EO})
  EO <- merge(EO, HUB[,c("datetime", "hub")], by="datetime")
  EO$nodeId <- factor(EO$nodeId)

  
  xyplot(P ~ datetime | nodeId, data=EO, groups=segment,
         xlab="", ylab="Price", layout=c(1,4))

  windows()
  xyplot(P ~ datetime | nodeId, data=EO, groups=segment,
         xlab="", ylab="Price", layout=c(2,2),
         subset=datetime >= as.POSIXct("2011-01-01"))

  day <- as.Date("2011-04-15")
  DD <- subset(EO, (datetime >= as.POSIXct(paste(day, "01:00:00")) &
                   datetime <= as.POSIXct(paste(day+1, "00:00:00"))) )
  DD$HE <- format(DD$datetime, "%H")
  DD <- DD[order(DD$datetime, DD$nodeId, DD$segment), ]
  
  split(DD, DD$HE)

  
  

  

  subset(EO, datetime >= as.POSIXct(paste(day, "01:00:00")) )
  
  
  
  
  #EO <- merge(EO, data.frame(nodeId=maskedNodeIds, unit=names(maskedNodeIds)))

  
  aux <- subset(Y, cMW >= 810)
  p810 <- ddply(aux, .(datetime), function(x){x$P[1]})  # 30 secs
  plot(p810$datetime, p810[,2], type="l", col="blue")
  
  
  
  aux <- Y[1:120,]
  aux$cMW <- unlist(tapply(aux$MW, aux$datetime, cumsum))

  
  # price of the first 3 units
  p810 <- ddply(aux, .(datetime), function(x){
    #browser()
    res <- NA
    ind <- which(cumsum(x$MW) >= 810)
    if (length(ind)>0){
      res <- x$P[ind[1]]
    }
    return(res)  
  })
}

##############################################################
# 
analyze_SalemHarbor <- function()
{
  maskedNodeIds <- c(79866, 34993, 16337, 45823)
  for (id in maskedNodeIds) EO.by_maskedNodeId(id)  # extract the Energy Offers

  files <- paste(DIR_EO, "Split/", maskedNodeIds, ".RData", sep="")
  totalQ <- get_cleared_mw(files, HUB)
  
  aux <- subset(totalQ, datetime > as.POSIXct("2010-05-01"))
  plot(aux$datetime, aux$MW, type="l", col="blue",
    main="Salem Harbor", cex.main=1, ylab="MW") 
}



#####################################################################
# sum up the offered MWs with prices < Hub price for that hour
# 
#
get_cleared_mw_lessThanHub<- function(files, HUB)
{
  EO <- finalize(files, function(file=file){load(file); EO})

  EO <- merge(EO, HUB[,c("datetime", "hub")], by="datetime")
  EO <- subset(EO, P <= hub)

  totalQ <- cast(EO, datetime ~ ., sum, value="MW")
  names(totalQ)[2] <- "MW"

  totalQ  
}

#####################################################################
# sum up the bid quantity only for bids < gasPrice*HR for that hour
# hr can be a vector
#
get_cleared_mw_lessThanHR <- function(files, HUB, hr)
{
  EO <- finalize(files, function(file=file){load(file); EO})
  EO <- merge(EO, HUB[,c("datetime", "gas")], by="datetime")

  QQ <- vector("list", length=length(hr))
  for (i in seq_along(hr)){
    aux <- subset(EO, P <= gas*hr[i])
    aux <- cast(aux, datetime ~ ., sum, value="MW")
    names(aux)[2] <- "MW"
    aux$hr <- hr[i]
    QQ[[i]] <- aux
  }  
  QQ <- do.call("rbind", QQ)

  QQ  
}


#####################################################################
# Define self scheduled as MW offered as must take + MW offered at
# a HR < 4
#
get_must_take_mw <- function(plots=FALSE)
{
  subsetFun <- function(file=file){
    load(file)
    mustTake <- subset(res, Must.Take.Energy != 0)
    mustTake <- cast(mustTake, datetime ~ ., sum, value="Must.Take.Energy")
    as.data.frame(mustTake)
  }
  
  # load all ISO files with demand bids, filter by constellation
  files <- list.files(DIR_EO, full.names=TRUE, pattern="^DB_")

  res <- finalize(files, subsetFun)
  names(res)[2] <- "mustTakeEnergy"

  if (plots){
    aux <- subset(res, datetime > as.POSIXct("2010-01-01"))
    plot(aux$datetime, aux$mustTakeEnergy, type="l")
  
    aux <- res
    aux$date <- as.Date(res$datetime)
    aux <- cast(aux, date ~ ., mean, value="mustTakeEnergy")
    names(aux)[2] <- "mustTakeEnergy"
    plot(aux$date, aux$mustTakeEnergy, type="l")
    aux$year <- format(aux$date, "%Y")
    aux$month <- as.numeric(format(aux$date, "%m"))
    xyplot(mustTakeEnergy ~ month|year, data=aux, layout=c(3,1))

    aux <- res
    aux$date <- as.Date(res$datetime)
    aux <- cast(aux, date ~ ., mean, value="mustTakeEnergy")
    names(aux)[2] <- "mustTakeEnergy"
    aux$yyyymm <- format(aux$date, "%Y-%m")
    aux$year <- format(aux$date, "%Y")
    aux$month <- as.numeric(format(aux$date, "%m"))
    bwplot(mustTakeEnergy ~ year | month, data=aux)
  }
  
  res
}




subsetMaskedNodeId <- function(file=file, maskedNodeIds){
  load(file)
  res <- subset(res, nodeId %in% maskedNodeIds)
  as.data.frame(res)
}

subsetParticipantId <- function(file=file, maskedParticipantId){
  load(file)
  res <- subset(res, participantId == maskedParticipantId)
  as.data.frame(res)
}


.clean.files <- function()
{
  files <- list.files(paste(DIR_EO, "Long", sep=""), full.names=TRUE,
    pattern="^long_")
  for (f in files){
    load(f)
    EO <- Y
    rownames(EO) <- NULL
    save(EO, file=f)
  }
}
