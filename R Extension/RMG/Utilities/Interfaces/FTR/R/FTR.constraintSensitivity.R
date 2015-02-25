# Calculate the sensitivity of a number of paths to a set of
# constraints.
#
# Written by Adrian Dragulescu on 9-Sep-08

FTR.constraintSensitivity <- function(paths, BC, 
  ISO.env, start.dt=NULL)
{
  if (is.null(start.dt))
    start.dt <- BC$time[1]
  if (!("CP" %in% names(paths)))
    paths$CP <- 0

  # read historical prices  
  hSP <- FTR.getPathReturns( paths, start.dt=start.dt,
                          ISO.env=ISO.env, hourly=TRUE)

  # cast constraints, and make them zoo 
  BC$value <- 1
  aux <- cast(BC, time ~ constraint.name, I)
  BC  <- zoo(as.matrix(aux[,-1]), aux[,1])
  colnames(BC) <- names(aux)[-1]

  # extend the BC index if necessary
  BC <- cbind(hSP[,1], BC)
  BC <- BC[,-1, drop=FALSE]
  BC[is.na(BC)] <- 0             # replace NA's with zeros (not binding)
 
  CS <- vector("list", length=nrow(paths))
  for (p in 1:nrow(paths)){
    PP <- hSP[,p,drop=FALSE]     # settle price for one path
    PP <- na.omit(PP)

    PP <- merge(PP, BC, all=c(TRUE,FALSE))

    aux <- melt(as.matrix(PP[,-1]))
    names(aux) <- c("time", "constraint", "on.off")
    aux <- cbind(aux, value=as.numeric(PP[,1]))
    
    count <- cast(aux, constraint + on.off ~ ., length)
    names(count)[3] <- "no.hours"
    
    aux <- cast(aux, constraint + on.off ~ ., summary)
    aux <- merge(aux, count)
    CS[[p]] <- cbind(path.no=paths$path.no[p], aux)
  }
  CS <- do.call("rbind", CS)

  return(CS)   # constraint sensitivity
}

.constraintEffectOnPortfolio <- function()
{
  
}
