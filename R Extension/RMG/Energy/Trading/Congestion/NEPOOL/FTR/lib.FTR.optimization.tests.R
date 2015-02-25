# Test cases for the quadratic optimization
#
#
#


#################################################################
# 2 paths, 2 constraints
#
.test1_P2C2 <- function()
{
  constraints <- list(
    constraint(mean=10, std=5, name="A"),
    constraint(mean=20, std=5, name="B")                  
  )

  # u_{c,i} size [C,P]
  unit_payoff <- matrix(c(20, 2,
                          -3, 6),
    byrow=TRUE, ncol=2,
    dimnames=list(c("A", "B"), c("path.1", "path.2")))
  unit_payoff

  q_min <- c(0,   0)
  q_max <- c(1000, 1000)
  pi_star <- 10000
  
  
  inp <- .prepare_solver_input(constraints, unit_payoff,
                               q_min, q_max, pi_star)

  sol <- solve.QP(inp$Dmat, inp$dvec, t(inp$Amat), inp$bvec, inp$meq)
  print(sol)
  rLog("Portfolio stddev =", round(sqrt(sol$value)))

  
}

#################################################################
#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(quadprog)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/FTR/lib.FTR.optimization.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/FTR/lib.FTR.optimization.tests.R")





  ## require(SecDb)
  ## require(FTR)
  ## FTR.load.ISO.env("NEPOOL")


}
