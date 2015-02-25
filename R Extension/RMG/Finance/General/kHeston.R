
kHeston <- function(a,g,theta,DT,k) {
  kHeston1 <- function(k){
    if (k == 0) {
      Ftk <- 0
    } else {
      if(g < 1e-5) {
        Ftk <- - log(1 + k*k*theta*DT/(2*a))
      } else {
        gt <- g*DT
        omega <- sqrt(1 + 2*k*k*theta/(g*a))
        omegaGammaTo2 <- omega*gt/2
        if (omegaGammaTo2 > 10) {
          Ftk <- gt/2*(1-omega) - log((omega*omega+2*omega+1)/(4*omega))
        } else {
          Ftk <- gt/2 - log(cosh(omegaGammaTo2) + (omega*omega + 1)/(2*omega)*sinh(omegaGammaTo2))
        }
      }
    }
    return(exp(a*Ftk))
  }
  return(sapply(k,kHeston1))
}