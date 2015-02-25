# Find if a path is counterflow or not
#
FTR.isCounterflow <- function( hSP, ISO.env )
{
  hSP <- hSP[,order(colnames(hSP))] # needed when I assemble the df below
  BS  <- FTR.basicStats(hSP, ISO.env)

  CC <- matrix(NA, nrow=6, ncol=nrow(BS))
  colnames(CC) <- BS$path.no
 # CC[1,] <- (BS$p100 - BS$p50)/(BS$p50 - BS$p0)
 # CC[2,] <- (BS$p99  - BS$p50)/(BS$p50 - BS$p1)
 # CC[3,] <- (BS$p95  - BS$p50)/(BS$p50 - BS$p5)
  CC[4,] <- (BS$p75  - BS$p50)/(BS$p50 - BS$p25)
  CC[5,] <- BS$EWin / BS$ELoss
  CC[6,] <- BS$days.gt.0 / BS$days.lt.0

  CC[2,which(BS$p99==BS$p1)]  <- NA
  CC[3,which(BS$p95==BS$p5)]  <- NA
  CC[4,which(BS$p75==BS$p25)] <- NA

  CC <- log(CC)
  CC[!is.finite(CC)] <- NA
  
  aux <- apply(CC, 2, sum, na.rm=T)

  r <- rep(NA, length(aux))
  names(r) <- names(aux)
  ind <- which(aux > log(nrow(CC)))
  if (length(ind)>0) r[ind] <- FALSE
  ind <- which(aux < -log(nrow(CC)))
  if (length(ind)>0) r[ind] <- TRUE

  r <- data.frame(path.no=as.numeric(names(aux)),
                  r.cntflw=aux, is.counterflow=r)
  r <- r[order(r$path.no), ]
  return(r)
}  
