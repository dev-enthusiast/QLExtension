
source("H:/user/R/RMG/Finance/Heston/kHeston.R")
dHeston <- function(a, g, theta, DT, x) {
  x <- abs(x)
  dHeston1 <- function(x) {
    if(g < 1e-5) {
      if( x == 0 ) {
        if( a <= 0.5) {
          PDFX = Inf
        } else {
          PDFX <- gamma(a-0.5)/gamma(a)*sqrt(a/(2*pi*theta*DT))
        }
      } else {
        y <- x*sqrt(2*a/(theta*DT))
        PDFX <- 1/(sqrt(pi)*gamma(a))*2^(0.5-a)*sqrt(2*a/(theta*DT))*y^(a-0.5)*besselK(y, 0.5-a)
      }
    } else {
      kMax <- 10
      while (kHeston(a,g,theta,DT,kMax) > 1e-5) # find kMax
        kMax <- kMax + 10
      integrand <- function(k) {cos(k*x)*kHeston(a,g,theta,DT,k)}
      PDFX <- integrate(integrand, lower = 0, upper = kMax)$value/pi
    }
    return(PDFX)
  }
  return(sapply(x,dHeston1))
}