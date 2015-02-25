# X <- data.frame(parent = c("A", "B", "B", "E", "C"),
#                 child  = c("B", "C", "D", "F", "G")
# expandGraphs(X)
#

########################################################################
# expand a graph relationship into an explicit table. 
#
expandGraphs <- function(X)
{
  if (!all(names(X) %in% c("parent", "child")))
    stop("Please make input data.frame with two columns: parent, child.")

  EX <- X
  names(EX) <- c("level1", "level0")
  N  <- nrow(X)
  level <- 1
  repeat {
    indP  <- which(EX[,1] %in% EX[,2])  # these parents have a parent

    # the parents of the parents
    aux <- EX[which(EX[,2] %in% EX[indP,1]), 1:2]
    names(aux) <- c(paste("level", level+1, sep=""), names(aux)[1])
    aux <- unique(aux)
    # the remaining parents map to themselves
    otherParents <- unique(EX[(1:N)[-indP],1])
    bux <- data.frame(otherParents, otherParents)
    names(bux) <- names(aux)
    # put them together
    aux <- rbind(aux, bux)
    
    EX  <- merge(aux, EX, all.y=TRUE) # merge with previous levels
    EX  <- EX[,rev(order(names(EX)))]
    
    if (identical(EX[,1],EX[,2])){      # you're done
      EX <- EX[,-1]                     # remove the redundant column 
      break
    }
    
    level <- level + 1
  }
  
  return(EX)
}

