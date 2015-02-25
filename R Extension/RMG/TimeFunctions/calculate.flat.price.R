# The function returns 7x24 prices for the given months.
# Where: data is a data frame with rownames formatted "2005-03", and
#             colnames "5x16", "7x8", "2x16"
#
# Written by Zhendong Xia on 5-May-2004
#
# requires source("C:/R/Work/MyLibrary/TimeFunctions/days.in.month.R")


calculate.flat.price <- function(data){
    m <- length(rownames(data))
    n <- length(colnames(data))
    rn <- rownames(data)
    cn <- colnames(data)

    hs <- matrix(0, n, 1)
    flatp <- data.frame(matrix(0, m, 1))
    colnames(flatp) <- "7x24"
    rownames(flatp) <- rn

    mrn <- paste(rn, "-01", sep="")    
    x <- as.Date(mrn)
    for (i in 1:m){
        dsinam <- days.in.month(x[i])
        tmp <- x[i]+0:(dsinam-1)
        tmp1<-format.dates(tmp, "m/d/y")
        wd <- sum(is.weekend(tmp1))
        hs[1] <- (dsinam-wd)*16
        hs[2] <- dsinam*8
        hs[3] <- wd*16
        flatp[rn[i],"7x24"] <- sum(hs*data[rn[i],])/sum(hs)
    }
    return(flatp)
  }
