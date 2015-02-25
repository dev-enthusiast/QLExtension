`FTR.filter.hSP` <-
function( hSP, month=as.Date("2008-05-01") )
{
  months <- c(seq(month, by="-1 month", length.out=2)[2],
              seq(month, by="month", length.out=2))
  if (class(hSP)=="list"){
    months <- substr(as.character(months), 5, 8)
    for (bucket in names(hSP)){
      ind <- substr(as.character(index(hSP[[bucket]])), 5, 8)
      ind <- which(ind %in% months)
      ind <- index(hSP[[bucket]])[ind]
      hSP[[bucket]] <- hSP[[bucket]][ind,]
    }
  }
  if (class(hSP)=="data.frame"){  # if hSP is melted
    months <- as.numeric(format(months, "%m"))
    ind <- as.POSIXlt(hSP$day)$mon+1
    ind <- which(ind %in% months)
    hSP <- hSP[ind,]  
  }
  return(hSP)
}

