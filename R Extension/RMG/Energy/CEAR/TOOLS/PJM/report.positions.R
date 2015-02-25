#
#
#
#

report.positions <- function(save, options){

  MM <- read.csv(options$outfile, header=F)
  positions <- data.frame(node=c(as.character(MM$Source),
               as.character(MM$Sink)), MW=c(-MM$MW, MM$MW),
               bucket=c(MM$Bucket.char, MM$Bucket.char))
  
  res <- sort(tapply(positions$MW, positions$node, sum), decreasing=T)
  plot(res)

  N <- 30; R <- length(res)
  OutT <- data.frame(Long.Top=names(res[1:N]), Long.MW=res[1:N])
  OutT <- cbind(OutT, data.frame(Short.Top=names(res[(R-N+1):R]),
                                 Short.MW=rev(res[(R-N+1):R])))
  rownames(OutT) <- 1:N
  OutT

  positions <- data.frame(node=c(as.character(MM$ptid.source),
               as.character(MM$ptid.sink)), MW=c(-MM$MW, MM$MW),
               bucket=c(MM$Bucket.char, MM$Bucket.char))
  res   <- tapply(positions$MW, list(positions$node, positions$bucket), sum)
  sRes  <- cbind(stack(as.data.frame(res)), nodes=rep(rownames(res), 3))
  sRes  <- na.omit(sRes)
  ind   <- sort(sRes$values, index.return=T)$ix
  (ssRes <- sRes[ind,])
  
  plot(res)



  

#   #----------------look at the positions-------------------------
#   require(vcd)
#   positions$LS <- ifelse(positions$MW>0, "Long", "Short")
#   positions$MW <- abs(positions$MW)
  
#   res <- tapply(positions$MW, list(positions$node, positions$bucket,
#                                    positions$LS), sum)

#   res <- round(res)
  
#   ind <- which(is.na(res), arr.ind=T)
#   res[ind] <- 0
#   tab <- as.table(res)
  
#   mosaicplot(tab, col=T)
  
   
}
