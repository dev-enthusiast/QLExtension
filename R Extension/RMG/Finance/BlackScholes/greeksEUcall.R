#
# function z=greeksEUcall(S,sigma,t,K,r,options)
#
# where- S is the asset price
#	   - sigma is the asset volatility per year
#      - r is the interest rate per year
#      - t is the time to expiry in years
#      - K is the excercise price of the option (strike price)
#      - options.index is the index of greeks to compute, =1(0)
#        if you want(don't want) [delta gamma theta vega rho]     
#      - z=[delta gamma theta vega rho]
#
# S, sigma, r, t, K can be vectors of size [N,1]
#-------------------------------------------------------------------
# written by Adrian Dragulescu on 02/29/2001
# ported to R from Matlab on 21-May-2003

greeksEUcall <- function(S,sigma,t,K,r,options){

if (nargs()==5){ options <- list()}
if (length(options$greeks)==0){ options$greeks <- c(1,1,1,1,1)}

d1 <- (log(S/K) + (r+0.5*sigma^2)*t)/(sigma*sqrt(t))
d2 <- (log(S/K) + (r-0.5*sigma^2)*t)/(sigma*sqrt(t))
Nd1 <- pnorm(d1, mean=0, sd=1) 
Nd2 <- pnorm(d2, mean=0, sd=1)
dNd1 <- exp(-d1^2/2)/sqrt(2*pi)

#--------------------------------------------------------------------
aux <- max(length(S),length(sigma),length(r),length(t),length(K))
z <- matrix(NA,nrow=aux, ncol=5, dimnames=list(NULL, c("Delta",
     "Gamma", "Theta", "Vega", "Rho")))

if (options$greeks[1]==1){
    z[,1] <- Nd1}                                      # Delta
if (options$greeks[2]==1){
    z[,2] <- dNd1/(sigma*S*sqrt(t))}                   # Gamma
if (options$greeks[3]==1){
    z[,3] <- -sigma*S*dNd1/(2*sqrt(t))
             - r*K*exp(-r*t)*Nd2}                      # Theta
if (options$greeks[4]==1){
    z[,4]=S*sqrt(t)*dNd1}                              # Vega
if (options$greeks[5]==1){
    z[,5] <- K*t*exp(-r*t)*Nd2}                        # Rho    
return(z)
}
######################################################################
