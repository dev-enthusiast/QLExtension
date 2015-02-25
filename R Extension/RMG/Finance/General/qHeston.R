
source("H:/user/R/RMG/Finance/Heston/pHeston.R")
qHeston <- function(a, g,theta, p, DT) {
  if (p > 1) {p <- p/100}
  Sign <- 1
  if (p < 0.5) {
    Sign <- -1
    p <- 1 - p
  }
  qHeston1 <- function(DT) {
    x0 <- 0
    p0 <- 0.5
    x1 <- sqrt(theta*DT)
    p1 <- pHeston(a, g, theta, DT, x1)
    while (p1 < p) {
      x0 <- x1
      p0 <- p1
      x1 <- x1 + sqrt(theta*DT)
      p1 <- pHeston(a, g, theta, DT, x1)
    }
    nIt <- 0
    while (abs(p1-p0) > 0.001 & nIt < 10) {
      nIt <- nIt + 1
      x  <- (x0 + x1)/2
      px <- pHeston(a, g, theta, DT, x)
      if(px > p ){
        x1 <- x
        p1 <- px
      } else {
        x0 <- x
        p0 <- px
      }
    }
    return(Sign*(x0+x1)/2)
  }
  return(sapply(DT,qHeston1))
}