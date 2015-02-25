# Use quadratic programming to minimize variance of a portfolio of
# paths.
#
# constraint
# find_solution
# .prepare_solver_input
# read_xlsx_template
#

#################################################################
# Each constraint is modeled with as a Gaussian random variable
# @param mean is the average number of hours this constraint will bind
#    in the term
# @param std is the standard deviation for the number of hours the
#    constraint will bind
# @param min_exposure is the minimum $ exposure you want to have on a path
# @param max_exposure is the maximum $ exposure you want to have on a path
# 
constraint <- function(mean, std, min_exposure, max_exposure, name="")
{
  list(name          = name,
       mu_c          = mean,
       sigma_c       = std,
       minexposure_c = min_exposure,
       maxexposure_c = max_exposure)
}


#################################################################
# Make a nice summary of the solution
# @param sol the result of the quadratic optimization as returned
#    by solve.QP
# @param inp the inputs to the solver
#
find_solution <- function(inp)
{
  # try it straight
  sol <- try(solve.QP(inp$Dmat, inp$dvec, t(inp$Amat), inp$bvec, inp$meq,
                      factorized=FALSE), silent=TRUE)
  
  if (class(sol) == "try-error") {
     rLog("Failed.  Quadratic matrix Rho is not positive definite.  Making it positive definite.")
     inp$Dmat_fixed <- as.matrix( nearPD(inp$Dmat)$mat ) 
     sol <- try(solve.QP(inp$Dmat_fixed, inp$dvec, t(inp$Amat), inp$bvec, inp$meq,
                      factorized=FALSE), silent=TRUE)
     
     if (class(sol) == "try-error") {
       rLog("Failed.  Try to calculate the Cholesky matrix directly")
       inp$R <- chol(inp$Dmat_fixed)          # R^T x R = Dmat
       inp$Dmat_factorized <- solve(inp$R)        # the inverse of the Choleski matrix
       sol <- try(solve.QP(inp$Dmat_factorized, inp$dvec,
                           t(inp$Amat), inp$bvec, inp$meq, factorized=TRUE), silent=TRUE)
       
       if (class(sol) == "try-error") {
         rLog("Failed.  Doubling the smallest eigenvalue.")
         aux <- eigen(inp$Dmat_fixed, symmetric=TRUE)
         aux$values[length(aux$values)] <- 2*aux$values[length(aux$values)] 
         inp$Dmat_increased <- aux$vectors %*% diag(aux$values) %*% solve(aux$vectors)
  
         sol <- try(solve.QP(inp$Dmat_increased, inp$dvec, t(inp$Amat), inp$bvec, inp$meq,
                      factorized=FALSE), silent=FALSE)
         if (class(sol) == "try-error") {
           #browser()
           if (grepl("constraints are inconsistent", sol)) {
             rLog("Inconsistent constraints.  Try changing the expected portfolio value or the position limits.")
           } else {
             rLog("Failed.  Don't know what to do anymore  Giving up.")
           }
           return(NULL)
         }
       }
     }
  }
  
  
  P <- length(sol$solution)
  C <- nrow(inp$unit_payoff)
    
  q <- data.frame(path=1:P, position=round(sol$solution),
                  q_min=inp$bvec[2:(1+P)], q_max=-inp$bvec[(P+2):(2*P+1)])
  #browser()
  rLog("Portfolio expected value =",
       paste("$", formatC(inp$bvec[1], format="fg", big.mark=",", digits=0), sep=""))
  rLog("Portfolio stddev =", dollar(sqrt(sol$value), digits=0))
  rLog("\nVariance minimizing positions:")
  print(q, row.names=FALSE)
  
  rLog("\nExposure by constraint:")
  mu_c <- sapply(inp$constraints, function(x){x$mu_c})
  print(data.frame(constraint=1:C,
    exposure=round(as.numeric(diag(mu_c) %*% inp$unit_payoff %*% as.matrix(sol$solution))),
    min_exposure = sapply(inp$constraints, function(x){x$minexposure_c}),
    max_exposure = sapply(inp$constraints, function(x){x$maxexposure_c})),
        row.names=FALSE)
 
  
  sol
} 


#################################################################
# Get all the inputs ready for optimization.
# @param paths, a list of Paths
# @param constraints a list of lists as returned by constraint.
#
make_input_from_paths <- function( paths )
{
  DIR <- paste("//CEG/CERShare/Trading/Nodal FTRs/NYISO/Auctions/", auctionName, sep="")
  load(paste(DIR, "/Data/mcc_regression.RData", sep=""))

  ptids <- sort(unique(unlist(
    lapply(paths, function(path) c(path$source.ptid, path$sink.ptid)))))

  aux <- subset(reg, ptid %in% ptids)
  
  
  
  inp <- .prepare_solver_input(constraints,
                               unit_payoff,
                               q_min, q_max,
                               pi_star)

  inp$unit_payoff <- unit_payoff
  inp$constraints <- constraints 
  
  inp

}

#################################################################
# Prepare the data for optimization
# @param constraints a list of lists as returned by constraint.
#   There should be C constraints.  
# @param unit_payoff the unit payoff matrix u_{c,i}, dim [C,P] 
# @param pi_star the expected portfolio payoff, a numeric value
# @param q_min minimal values for the quantities, a vector size P 
# @param q_max maximal values for the quantities, a vector size P 
# @return a list with (Dmat, dvec, Amat, bvec, meq) ready for the solver
#
.prepare_solver_input <- function(constraints, unit_payoff,
                                  q_min, q_max, pi_star)
{
  # check dimensions
  stopifnot(length(q_min)==length(q_max),
            length(q_min)== ncol(unit_payoff))
  stopifnot(length(constraints)==nrow(unit_payoff))
  
  # number of constraints
  C <- length(constraints)
  
  # number of paths
  P <- ncol(unit_payoff)

  sigma_c <- sapply(constraints, function(x){x$sigma_c})
  mu_c <- sapply(constraints, function(x){x$mu_c})
  maxexposure_c <- sapply(constraints, function(x){x$maxexposure_c})
  minexposure_c <- sapply(constraints, function(x){x$minexposure_c})
  
  v <- mu_c %*% unit_payoff

  Dmat <- matrix(NA, ncol=P, nrow=P)  
  for (i in seq_len(P)) {
    for (j in i:P) {
      Dmat[i,j] <- 2*sum(sigma_c^2*unit_payoff[,i]*unit_payoff[,j])
      if (i!=j) 
        Dmat[j,i] <- 2*sum(sigma_c^2*unit_payoff[,i]*unit_payoff[,j])
    }
  }

  dvec <- rep(0, length=P)
  Amat <- matrix(NA, nrow=2*P+2*C+1, ncol=P)
  Amat[1,] <- v                                  # the expected portfolio
  Amat[2:(P+1), 1:P] <- diag(1, P, P)            # the lower position bound
  Amat[(P+2):(2*P+1), 1:P] <- diag(-1, P, P)     # the upper position bound
  Amat[(2*P+2):(2*P+C+1),1:P] <-  -diag(mu_c) %*% unit_payoff   # the max exposure constraint
  Amat[(2*P+C+2):(2*P+2*C+1),1:P] <- diag(mu_c) %*% unit_payoff  # the min exposure constraint

  bvec <- c(pi_star, q_min, -q_max, -maxexposure_c, minexposure_c)  # size 2*P+2*C+1
  meq  <- 1           # one equality, the expected portfolio value 

  
  list(Dmat=Dmat, dvec=dvec, Amat=Amat, bvec=bvec, meq=meq)
}


#################################################################
# Read a template for optimization 
#
read_xlsx_template <- function(filename, sheetName)
{
  require(xlsx)

  wb <- loadWorkbook(filename)
  sheets <- getSheets(wb)
  sheet <- sheets[[sheetName]]

  # read the expected portfolio value
  pi_star <- readColumns(sheet, startColumn=5, endColumn=5,
                         startRow=2, endRow=2, header=FALSE,
                         as.data.frame=FALSE, colClasses="numeric")[[1]]
  
  # read the positions limits
  qq <- readColumns(sheet, startColumn=1, endColumn=3,
                         startRow=4, endRow=NULL, header=TRUE,
                         as.data.frame=TRUE, colClasses="numeric")
  ind <- which(is.finite(qq[,1]))
  q_min <- qq[ind,2]
  q_max <- qq[ind,3]
  stopifnot(length(q_min)==length(q_max))
  P <- length(q_min)   # number of paths

  # read the constraints mean, std, max_exposure
  aux <- readColumns(sheet, startColumn=5, endColumn=9,
                         startRow=4, endRow=NULL, header=TRUE,
                         as.data.frame=TRUE, colClasses="numeric")
  aux  <- aux[which(is.finite(aux[,1])),]
  C <- nrow(aux)  # number of constraints
  constraints <- dlply(aux, c("Constraints"), function(x) {
    constraint(x[,2], x[,3], x[,4], x[,5])
  })
  
  # read the unit payoffs constraints 
  aux <- readColumns(sheet, startColumn=12, endColumn=12+C-1,
                         startRow=4, endRow=4+P, header=TRUE,
                         as.data.frame=TRUE, colClasses="numeric")

  unit_payoff <- t(as.matrix(aux))  # solver input takes it like this ...
  colnames(unit_payoff) <- paste("path", 1:P, sep=".")
  rownames(unit_payoff) <- NULL


  inp <- .prepare_solver_input(constraints, unit_payoff,
                               q_min, q_max, pi_star)
  
  inp$unit_payoff <- unit_payoff
  inp$constraints <- constraints 
  
  inp
}



#################################################################
#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(quadprog)
  require(Matrix)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/FTR/lib.FTR.optimization.R")
  #source("S:/All/Risk/Software/R/prod/Energy/Trading/Congestion/NEPOOL/FTR/lib.FTR.optimization.R")

  #########################################################################
  # optimize a set of paths
  #
  inp <- make_input_from_paths( paths, )
  sol <- find_solution( inp )
 
  
  #########################################################################
  # optimize from a template
  #
  filename <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/FTR/optimization_template.xlsx"
  sheetName <- "P3C2"
  inp <- read_xlsx_template(filename, sheetName)

  sol <- find_solution(inp)
  


  

}
