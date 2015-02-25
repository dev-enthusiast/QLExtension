# Use a combination of buddy algorithm and linked locations to fill in prices. 
# If you think there are too many if statements, talk to your price/vol  
# data maintainers to clean it up!!!
#
# hPV.mkt can be hP.mkt or hV.mkt, which are loaded from the mkt files. 
# 
# Called from fill.mkt.price.files.R
#
# Written by Adrian Dragulescu on 4-May-2005

fill.ts.ratio <- function(iPP, hPV.bc, ind.iPP.na, ind.bc.na){
  fPP <- iPP                            # filled prices

  if( length(which(ind.iPP.na)) == 0 )
  {
      rLog("no NAs, don't need to fill:", fPP$curve.name)
      return(fPP)
  }
  
  max.ind.iPP.na <- min(max(which(ind.iPP.na)), length(iPP)-1)
  if (any(ind.bc.na) & !is.na(hPV.bc[max.ind.iPP.na+1])){ 
    fPP[which(ind.iPP.na)] <- as.numeric(hPV.bc[max.ind.iPP.na+1])
    return(fPP)
  }
  ratio <- as.numeric(iPP[max.ind.iPP.na+1]/hPV.bc[max.ind.iPP.na+1])
  if (is.finite(ratio)){
    fPP[which(ind.iPP.na)] <- ratio*as.numeric(hPV.bc[which(ind.iPP.na)])    
  } else {
    fPP[which(ind.iPP.na)] <- as.numeric(hPV.bc[which(ind.iPP.na)])
  }
  return(fPP)
}
  

fill.one.ts.curve <- function(i, mkt, mkt.cinfo, hPV.mkt){
#  browser()
  iPP <- as.vector(subset(hPV.mkt, curve.name  == mkt.cinfo$curve.name[i] &
    contract.dt == mkt.cinfo$contract.dt[i]))   # incomplete prices
  ind.iPP.na <- is.na(iPP)
  if (!is.na(mkt.cinfo[i,"replacement.curve"])){  # have a buddy!
    hPV.bc <- subset(hPV.mkt, curve.name  == mkt.cinfo$replacement.curve[i] &
                              contract.dt == iPP$contract.dt)
    if (nrow(hPV.bc)>0){             # if you have a buddy
      hPV.bc <- as.vector(hPV.bc)
      ind.bc.na <- is.na(hPV.bc)
      fPP <- fill.ts.ratio(iPP, hPV.bc, ind.iPP.na, ind.bc.na)
      return(fPP)
    }  
  } else { 
#    cat(iPP$curve.name, as.character(iPP$contract.dt),
#        ": No buddy and no linked location.  Please report this.\n")
    ind   <- max(which(ind.iPP.na))
    if (ind < length(iPP)){
      value <- iPP[max(which(ind.iPP.na))+1]
    } else {
      value <- 0         # fill with zeros if you don't have any.
    }
    iPP[which(ind.iPP.na)] <- value
    return(iPP)  
  }
}


## fill.ts.basis <- function(){   # unimplemented yet
## }

##   if (!is.na(hPV.bc[max.ind.iPP.na+1])){  # do replacement
##     ratio <- as.numeric(iPP[max.ind.iPP.na+1]/hPV.bc[max.ind.iPP.na+1])
##     if (is.finite(ratio))
##     if (is.na(iPP[max.ind.iPP.na+1])){
##       fPP[which(ind.iPP.na)] <- as.numeric(hPV.bc[max.ind.iPP.na+1])
##     } else {                            # can calculate ratio
##       fPP[which(ind.iPP.na)] <- ratio*as.numeric(hPV.bc[which(ind.iPP.na)])
##     }
##   }
##   return(fPP)
## }



