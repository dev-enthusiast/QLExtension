# Tools to unmask energy offers
#
# .archive
# .characterize_participants
# .get_SCC
# .unmask_participant
# 

##################################################################
# Archive my best :-)
#
archive <- function(unmaskedParticipants)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/"
  
  fname <- paste(DIR, "RData/unmaskedParticipants.RData", sep="")
  save(unmaskedParticipants, file=fname)
  rLog("Wrote", fname)

  fname <- paste(DIR, "CSV/unmaskedParticipants.csv", sep="")
  write.csv(unmaskedParticipants, file=fname, row.names=FALSE)
  rLog("Wrote", fname)
}



##################################################################
# find important characteristics for participants how they bid
# - units, ecoMax, ecoMin, claim10, claim30
#
.characterize_participants <- function(month, participantId=NULL)
{
  load( EO.get_files(month) )   # loads res data.frame
  
  if (!is.null(participantId))
    res <- aux[which(res$participantId %in% participantId),]

  aux <- ddply(res, c("participantId", "nodeId"), function(x) {
    c(ecoMax=max(x$Economic.Maximum, na.rm=TRUE),
      ecoMin=max(x$Economic.Minimum, na.rm=TRUE),
      claim10=max(x$Claim.10, na.rm=TRUE),
      claim30=max(x$Claim.30, na.rm=TRUE))
  })
  aux <- aux[order(aux$participantId, -aux$ecoMax),]

  
  aux
}


##################################################################
# get the SCC report for a given month
#
.get_SCC <- function( month )
{
  SCC <- .read_scc_report( month, colIndex=c(1,2,3,5,6,10:14,16,22,28) )
  if (as.numeric(format(month, "%m")) %in% c(6:9)) {
    SCC$EcoMax <- SCC$SSCC
  } else {
    SCC$EcoMax <- SCC$WSCC    
  }
  SCC <- subset(SCC, Intermittent == "NON-INTERMITTENT")  # <-- only the ones you bid!

  SCC
}

##################################################################
# Try to figure out one participant
#
.unmask_participant <- function(participant, month)
{
  rLog("Adding by hand: ")
  rLog("  PSEG New Haven, LLC")
  participants[["427733"]]$name <- "PSEG New Haven, LLC"

  participants
}


##################################################################
# Try to figure out one participant
#
.unmask_participant <- function( month, participantId, SCC = .get_SCC(month),
   participants=NULL, fuzz=0.15 )
{
  rLog("Trying to unmaks ", participantId)

  EO <- participants[[ as.character(participantId) ]]
  noUnitsBid <- EO$noPlants   # how many units are bid

  # names of participants already identified
  identified <- setdiff(as.character(sapply(participants, "[[", "name")), "")
  toIdentify <- setdiff(unique(SCC$Current.Lead.Participant), identified)
  
  # Algo like this ...
  # For this participant, pick the highest unit bid in
  # Look in the SCC report, which one it can be, and generate a list of possible names
  # Take the second highest unit and try to refine the list of possible names
  # Take the next highest unit and refine the list of names until you have one candidate
  # left.  That must be your participant name!

  if (as.character(participantId) == "711538") browser()
  
  participantName <- toIdentify   # can be any of the remaining names
  aSCC <- SCC                     # start with all the plants
  for (u in 1:noUnitsBid) {
    ecoMax <- EO$summary$ecoMax[u]*c(1-fuzz, 1+fuzz)
    ind <- which(aSCC$EcoMax >  ecoMax[1] & aSCC$EcoMax < ecoMax[2])
    aux <- aSCC[ind,]
    newSet <- intersect(participantName, unique(aux$Current.Lead.Participant))    
    if ( length(newSet) != 0 && length(newSet) <= length(participantName) )
      participantName <- newSet

    # for each participant, reduce the universe of plants by one
    # so you don't selecting the same ones
    idRm <- ddply(aux, c("Current.Lead.Participant"), function(x){
      x[which.max(x$EcoMax),]})$Asset.ID
    aSCC <- subset(aSCC, !(Asset.ID %in% idRm) )
    
    if (length(participantName) == 1)  break
  }

  if ( length(participantName) > 1) {
    # do a simple heuristic.  Look at the number of units of remaining names
    # if you match exactly the number of units bid and the difference between noUnitsBid
    # and the next best choice is more than 3, assign it to the exact match.
    #
    aux <- abs(table(subset(SCC,
      Current.Lead.Participant %in% participantName)$Current.Lead.Participant) - noUnitsBid)
    if (min(aux) == 0 & min(aux[-which.min(aux)]) > 3) {
      rLog("  using heuristic")
      participantName <- names(aux)[which.min(aux)]
      participants[[as.character(participantId)]]$name <- participantName
    }
    
  } else {
    participants[[as.character(participantId)]]$name <- participantName
  }
  
  participants
}


##################################################################
##################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(SecDb)
  require(zoo)
  require(xlsx)

  source(paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/ISO_Data/EnergyOffers/",
    "lib.investigate.energy.offers.R", sep=""))
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Units/lib.NEPOOL.scc_report.R")

  source(paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/ISO_Data/EnergyOffers/",
    "lib.unmask.energy.offers.R", sep=""))

  months <- seq(as.Date("2012-01-01"), by="1 month", length.out=8)

  # these are all the guys
  aux <- EO.get_unique_participantsMonth( EO.get_files(months) )
  aux <- unique(aux[,"participantId", drop=F])
  uParticipants <- aux[order(aux$participantId), , drop=FALSE]


  month <- as.Date("2012-08-01")
  aux <- .characterize_participants( month )
  participants <- dlply(aux, c("participantId"), function(x) {
    list(name = "",             # <-- I should fill in with my guess!
         participantId = x$participantId[1],
         noPlants = nrow(x),
         summary = x[,-1])
  })

  SCC <- .get_SCC(month)

  # sort them by decreasing number of power plants
  participants <- participants[order(-sapply(participants, "[[", "noPlants"))]
  participants <- lapply(participants, function(x){x$name <- ""; x})
  
  source(paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/ISO_Data/EnergyOffers/",
    "lib.unmask.energy.offers.R", sep=""))
  for (i in seq_along(participants)) {
    participantId <- participants[[i]]$participantId
    participants  <- .unmask_participant( month, participantId, SCC, participants, fuzz=0.10 )
  }
  participants <- .unmaks_by_hand( participants, month )  # add the ones done by hand!
  identified <- setdiff(as.character(sapply(participants, "[[", "name")), "")
  toIdentify <- setdiff(unique(SCC$Current.Lead.Participant), identified)

  # this is what I identified
  unmaskedParticipants <- ldply(participants, function(x){data.frame(name=x$name)})
  names(unmaskedParticipants)[1] <- "participantId"
  comment(unmaskedParticipants) <- paste("Done on", as.character(Sys.Date())) 
  print(unmaskedParticipants, row.names=FALSE)
  # archive(unmaskedParticipants)
  

  month <- as.Date("2012-08-01")
  participantId <- 529988   # NRG easy!
  participantId <- 931987
  participants[[ as.character(participantId) ]]

  participants[[ as.character(participantId) ]]




  
  
  
  
}

