# Bins the returns 
# Where: r - the returns
#        limit=c(lowerLimit,upperLimit)
#        bins = number of bins
#
# written by Adrian Dragulescu on 31-Jul-2003


binReturns <- function(r, options){

if (nargs()==1){options <- list()}
if (length(options$prctile)==0){options$prctile=3}
N <- length(r)
if (length(options$bins)==0){bins=floor(sqrt(N))}
if (length(options$plot)==0){options$plot=1}
if (length(options$scale)==0){options$scale="log"}
if (length(options$limit)==0){
  limit <- as.numeric(quantile(r,probs=c(0.03,0.97)))}

iBelow <- which(r<limit[1]) # index of returns below the limit
iAbove <- which(r>limit[2]) # index of returns above the limit
rCut <- r[-union(iBelow,iAbove)]
res <- hist(rCut, breaks=seq(limit[1],limit[2],length=bins))
res$belowL <- length(iBelow)
res$aboveL <- length(iAbove)

if (options$plot==1){
  if (options$scale=="log"){
     plot(res$mids,log(res$density), type="p")
   }else{
     plot(res$mids,log(res$density), type="p")
   }
}
return(res)
}
