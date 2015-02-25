# Payoff calculation for FTRs.
#
# PosMM is a data.frame with colums, "MW", "Bucket", "Type", "Source",
#   "Sink", "Cleared.Price"
# PriceMM is a list, with three components, "PEAK","OFFPEAK","FLAT"
# each component is a three-dimension array,
#   "Year", "Month", "Day", "Hour", NodeID1, NodeID2, ..., NodeIDN,
#   the third dimension has the simulated prices
#
# Return PayoffMM, a two-dimension matrix, Rownames = PathNames, Colnames = Sim #
#
#
# Written by Wang Yu, 7-Jul-2005

payoff.FTR <- function(PriceMM, PosMM){
  
     PriceDim <- dim(PriceMM$PEAK)
     PosDim   <- dim(PosMM)

     unit.Payoff <- payoff <- matrix(NA, PosDim[1],PriceDim[3])
     colnames(unit.Payoff) <- colnames(unit.Payoff,do.NULL = F,prefix = "Sim")
     rownames(unit.Payoff) <- rownames(unit.Payoff,do.NULL = F,prefix = "Path")

     for (index in 1:PosDim[1]){    # loop over all unique positions
       rownames(unit.Payoff)[index] <- paste(PosMM[index,"Class"],PosMM[index,"Type"],
                PosMM[index,"Source"],"to",PosMM[index,"Sink"],sep="_")
       if (PosMM[index,"Class"] == "PEAK")   {PMM <- PriceMM$PEAK}
       if (PosMM[index,"Class"] == "OFFPEAK"){PMM <- PriceMM$OFFPEAK}
       if (PosMM[index,"Class"] == "FLAT")   {PMM <- PriceMM$FLAT}
       ind.Source <- which(colnames(PMM) ==
            paste("NEPOOL_SMD_DA_",PosMM[index,"Source"],"_CongComp",sep=""))
       ind.Sink   <- which(colnames(PMM) ==
            paste("NEPOOL_SMD_DA_",PosMM[index,"Sink"],"_CongComp",sep=""))

       SimPrice <- PMM[,ind.Sink,] - PMM[,ind.Source,]
       if (PosMM[index,"Type"] == "Option"){SimPrice[SimPrice<0] <- 0}
       SimPrice <- SimPrice - PosMM[index,"Cleared.Price"]

       unit.Payoff[index,] <- apply(SimPrice, 2, sum, na.rm=T) # payoff if Q = 1 MW
       payoff[index,] <- unit.Payoff[index,]*PosMM$MW[index]
     }

     return(list(unitPayoff = unit.Payoff, sumPayoff = payoff))
  }


#          SimPrice <- SimPrice - PosMM[index,"Cleared.Price"]
#          PL <- SimPrice #  for unit quantities * PosMM[index,"MW"]
