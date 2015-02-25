
source("H:/user/R/RMG/Finance/Heston/dHeston.R")
pHeston <- function(a, g, theta, DT, x) {
  pHeston1 <- function(x) {
    if(x == 0) {
      res <- 0.5
    } else {
      integrand <- function(x){ dHeston(a, g, theta, DT, x) }
      res <- integrate(integrand, lower = 0, abs(x))$value
      if( x < 0) {res <- 0.5 - res}
      if( x > 0) {res <- res + 0.5}
    }
    return(res)
  }
  return(sapply(x,pHeston1))
}