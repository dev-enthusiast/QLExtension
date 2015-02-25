#
#
#
#
table.spread <- function(){
    N <- 10
    myQuantiles <- function(x){quantile(x, probs=seq(0,1,1/N), na.rm=T)}
    cuts   <- apply(XX, 2, myQuantiles)
    probs  <- table(cut(XX[,1], cuts[,1]), cut(XX[,2], cuts[,2]))
    cprobs <- probs
    for (c in 2:N){cprobs[,c] <- cprobs[,c-1] + probs[,c]}
    sumcol <- matrix(rep(cprobs[,N], times=N), nrow=N)
    cprobs <- cprobs/sumcol
}

