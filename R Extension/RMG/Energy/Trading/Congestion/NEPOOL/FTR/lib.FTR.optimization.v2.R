# Use quadratic programming to minimize variance of a portfolio of
# paths.
#
# constraint
# find_solution
# make_lambda_matrix
# make_unit_matrix
# .prepare_solver_input
# read_xlsx_template
# .suggest_constraints_input
#
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
# Call the solver 
# @param inp the inputs to the solver as a list:
#   Dmat, dvec, Amat, bvec, meq, constraints
#
find_solution <- function( inp )
{
  # try it straight
  sol <- try(solve.QP(inp$Dmat, inp$dvec, inp$Amat, inp$bvec, inp$meq,
                      factorized=FALSE), silent=TRUE)
  
  if (class(sol) == "try-error") {
    #print(sol)
    if (grepl("constraints are inconsistent", sol) ) {
      stop("Constraints are inconsistent!  Modify constraints!") 
    }
    rLog("Failed.  Quadratic matrix Rho is not positive definite.  Making it positive definite.")
    inp$Dmat_fixed <- as.matrix( nearPD(inp$Dmat)$mat ) 
    sol <- try(solve.QP(inp$Dmat_fixed, inp$dvec, inp$Amat, inp$bvec, inp$meq,
                      factorized=FALSE), silent=TRUE)
     
     if (class(sol) == "try-error") {
       rLog("Failed.  Try to calculate the Cholesky matrix directly")
       inp$R <- chol(inp$Dmat_fixed)          # R^T x R = Dmat
       inp$Dmat_factorized <- solve(inp$R)        # the inverse of the Choleski matrix
       sol <- try(solve.QP(inp$Dmat_factorized, inp$dvec,
                           inp$Amat, inp$bvec, inp$meq, factorized=TRUE), silent=TRUE)
       
       if (class(sol) == "try-error") {
         rLog("Failed.  Doubling the smallest eigenvalue.")
         aux <- eigen(inp$Dmat_fixed, symmetric=TRUE)
         aux$values[length(aux$values)] <- 2*aux$values[length(aux$values)] 
         inp$Dmat_increased <- aux$vectors %*% diag(aux$values) %*% solve(aux$vectors)
  
         sol <- try(solve.QP(inp$Dmat_increased, inp$dvec, inp$Amat, inp$bvec, inp$meq,
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
  C <- length(inp$constraints)

  payoff_path <- as.matrix(sol$solution) *
    (inp$ULambda %*% matrix(sapply(inp$constraints, function(x){x$mu_c})) - inp$clearingCost)  
  
  qty <- data.frame(path=1:P,
                    position = round(sol$solution),
                    q_min    = inp$bvec[2:(1+P)],
                    q_max    = -inp$bvec[(P+2):(2*P+1)],
                    payoff   = round(payoff_path))

    
  #browser()
  rLog("Portfolio expected value =", dollar(inp$bvec[1], digits=0))
  rLog("Portfolio stddev =", dollar(sqrt(sol$value), digits=0))
  rLog("\nVariance minimizing positions:")
  print(qty, row.names=FALSE)
  
  rLog("\nExposure by constraint:")
  mu_c <- sapply(inp$constraints, function(x){x$mu_c})
  print(data.frame(constraint=sapply(inp$constraints, function(x){x$name}),
    exposure=round( t(inp$Amat)[(2*P+C+2):(2*P+2*C+1),] %*% as.matrix(sol$solution)),
    min_exposure = sapply(inp$constraints, function(x){x$minexposure_c}),
    max_exposure = sapply(inp$constraints, function(x){x$maxexposure_c})),
        row.names=FALSE)
 
  
  list(solution=sol, qty=qty)
} 





#################################################################
# Make \Lambda matrix with the regrestion coefficients,  
# @param paths, a list of Paths
# @params constraints names of the constraints to use.  If NULL, use
#   all, if not filter them.  
# @return a matrix
#
make_lambda_matrix <- function( paths, constraints=NULL)
{
  ptids <- sort(unique(unlist(
    lapply(paths, function(path) c(path$source.ptid, path$sink.ptid)))))

  # load the regression info
  auctionName <- paths[[1]]$auction$auctionName
  fname <- paste("//CEG/CERShare/Trading/Nodal FTRs/NYISO/Auctions/",
               auctionName, "/Data/mcc_regression.RData", sep="")
  load(fname)
  reg <- merge(reg, data.frame(ptid = ptids), all.y=TRUE)
  if (any(is.na(reg$constraint.name))) {
    stop("Cannot find a regression for nodes: ",
         unique(reg$ptid[is.na(reg$constraint.name)]))
  }
   
  reg <- subset(reg, ptid %in% ptids)
  if ( !is.null(constraints) ) 
    reg <- subset(reg, constraint.name %in% constraints)
    
  aux <- cast(reg, ptid ~ constraint.name, I, fill=0, value="regression.coef")
  L <- as.matrix(aux[,-1])
  rownames(L) <- aux[,1]
  colnames(L) <- colnames(aux)[-1]
  
  L
}


#################################################################
# Make unit matrix U from a list of paths
# @param paths, a list of Paths
# @return a matrix
#
make_unit_matrix <- function( paths )
{
  ptids <- sort(unique(unlist(
    lapply(paths, function(path) c(path$source.ptid, path$sink.ptid)))))

  U <- matrix(0, nrow=length(paths), ncol=length(ptids))
  for (i in seq_along(paths)) {
    U[i, which(ptids == paths[[i]]$source.ptid)] <- -1
    U[i, which(ptids == paths[[i]]$sink.ptid)]   <-  1
  }
  colnames(U) <- ptids
  rownames(U) <- 1:length(paths)

  if (paths[[1]]$auction$region == "NYPP")
    U <- -1 * U
  
  U
}



#################################################################
# Prepare the data for optimization
# @param constraints a list of lists as returned by constraint.
#   There should be C constraints.  
# @param paths a list of paths, length P
# @param constraints a list of constraints, length C
# @param q_min minimal values for the quantities, a vector size P 
# @param q_max maximal values for the quantities, a vector size P
# @param clearingCost a vector size P, equal to ClearingPrice * Hours, cost for 1 MW
# @param pi_star the expected net portfolio payoff, a numeric value
# @return a list with (Dmat, dvec, Amat, bvec, meq) ready for the solver
#
.prepare_solver_input <- function(paths,
                                  constraints,
                                  q_min, q_max,
                                  clearingCost,    
                                  pi_star)
{
  # check dimensions
  stopifnot(length(q_min)==length(q_max))
  
  # number of constraints
  C <- length(constraints)
  
  # number of paths
  P <- length(q_min)

  # the unit matrix
  U <- make_unit_matrix( paths )

  # the regression matrix:  P = Lambda S
  cnames <- unlist(lapply(constraints, "[[", "name"), use.names=FALSE)
  Lambda <- make_lambda_matrix( paths, cnames )

  
  sigma_c <- sapply(constraints, function(x){x$sigma_c})
  mu_c <- sapply(constraints, function(x){x$mu_c})
  maxexposure_c <- sapply(constraints, function(x){x$maxexposure_c})
  minexposure_c <- sapply(constraints, function(x){x$minexposure_c})
  
  ULambda <- U %*% Lambda  # size P x C
  
  # the quadratic matrix
  Dmat <- matrix(NA, ncol=P, nrow=P)  
  for (i in seq_len(P)) {
    for (j in i:P) {
      Dmat[i,j] <- 2*sum(sigma_c^2*ULambda[i,]*ULambda[j,])
      if (j != i)  
        Dmat[j,i] <- Dmat[i,j]
    }
  }
  
  # no linear component
  dvec <- rep(0, length=P)
  
  # the constraints 
  AmatT <- matrix(NA, nrow=1+2*P+2*C, ncol=P)
  AmatT[1,] <- t(ULambda %*% matrix(mu_c)) - clearingCost # the expected payoff of the portfolio
  AmatT[2:(P+1), 1:P]       <- diag(1, P, P)      # the lower position bound, q >= q_min 
  AmatT[(P+2):(2*P+1), 1:P] <- diag(-1, P, P)     # the upper position bound, q <= q_max
  AmatT[(2*P+2):(2*P+C+1),1:P] <- -t(ULambda %*% diag(mu_c))    # the max exposure constraint, C x P
  AmatT[(2*P+C+2):(2*P+2*C+1),1:P] <- t(ULambda %*% diag(mu_c)) # the min exposure constraint, C x P

  bvec <- c(pi_star, q_min, -q_max, -maxexposure_c, minexposure_c)  # size 2*P+2*C+1
  meq  <- 1           # one equality, the expected portfolio value 

  
  list(Dmat=Dmat, dvec=dvec, Amat=t(AmatT),
       bvec=bvec, meq=meq, ULambda=ULambda)
}


#################################################################
# Read the paths used for optimization 
#
read_paths_for_optimization <- function(filename, sheetName="Optimization")
{
  require(xlsx)

  wb <- loadWorkbook(filename)
  sheets <- getSheets(wb)
  sheet <- sheets[[sheetName]]

  auctionName <- readColumns(sheet, startColumn=5, endColumn=5,
                         startRow=1, endRow=1, header=FALSE,
                         as.data.frame=FALSE, colClasses="character")[[1]]
  auction <- parse_auctionName( auctionName, region="NYPP")
 
  qq <- readColumns(sheet, startColumn=1, endColumn=3,
    startRow=4, endRow=NULL, header=TRUE,
    as.data.frame=TRUE,
    colClasses=c(rep("numeric", 3)))
  ind <- which(is.finite(qq[,1]))
  qq <- qq[ind,]

  paths <- dlply(qq, c("Path"), function(x) {
    Path(source.ptid = x$source.ptid,
         sink.ptid   = x$sink.ptid,
         bucket      = "FLAT",
         auction     = auction)
  })

  print(do.call(rbind, lapply(paths, as.data.frame))[,
    c("source.ptid", "sink.ptid", "source.name", "sink.name")])

                    
  paths
}



#################################################################
# Read a template for optimization 
#
read_xlsx_template <- function(filename, sheetName="Optimization")
{
  require(xlsx)

  wb <- loadWorkbook(filename)
  sheets <- getSheets(wb)
  sheet <- sheets[[sheetName]]

  # read the expected portfolio value
  pi_star <- readColumns(sheet, startColumn=5, endColumn=5,
                         startRow=2, endRow=2, header=FALSE,
                         as.data.frame=FALSE, colClasses="numeric")[[1]]

  auctionName <- readColumns(sheet, startColumn=5, endColumn=5,
                         startRow=1, endRow=1, header=FALSE,
                         as.data.frame=FALSE, colClasses="character")[[1]]
  auction <- parse_auctionName( auctionName, region="NYPP")
  
  # read the positions limits and clearing prices
  qq <- readColumns(sheet, startColumn=1, endColumn=8,
    startRow=4, endRow=NULL, header=TRUE,
    as.data.frame=TRUE,
    colClasses=c(rep("numeric", 3), rep("character", 2), rep("numeric", 3)))
  ind <- which(is.finite(qq[,1]))
  qq <- qq[ind,]
  q_min <- qq[ind,6]   # min allowed qty
  q_max <- qq[ind,7]   # max allowed qty
  cp    <- qq[ind,8]   # clearing price
  stopifnot(length(q_min)==length(q_max))
  P <- length(q_min)   # number of paths

  # read the constraints mean, std, max_exposure
  aux <- readColumns(sheet, startColumn=10, endColumn=14,
                     startRow=4, endRow=NULL, header=TRUE,
                     as.data.frame=TRUE,
                     colClasses=c("character", rep("numeric", 4)))
  aux  <- aux[which(is.finite(aux[,2])),]
  C <- nrow(aux)  # number of constraints


  paths <- dlply(qq, c("Path"), function(x) {
    Path(source.ptid = x$source.ptid,
         sink.ptid   = x$sink.ptid,
         bucket      = "FLAT",
         auction     = auction)
  })

  hrs <- count_hours_auction(paths[[1]]$auction$auctionName,
                             region="NYPP", buckets="FLAT")
  hours <- hrs$hours   # for NYPP only
  constraints <- dlply(aux, c("Constraints"), function(x) {
    constraint(mean = hours*x[,2],
               std  = hours*x[,3],    # it is linear in hours, no diversification
               min_exposure = x[,4],
               max_exposure = x[,5],
               name=x[,1] )
  })
  
  clearingCost <- cp*hours
  
  inp <- .prepare_solver_input(paths,
                               constraints,
                               q_min, q_max,
                               clearingCost, 
                               pi_star)

  inp$constraints <- constraints
  inp$hours <- hours
  inp$paths <- paths
  inp$clearingCost <- clearingCost
  
  inp
}



#################################################################
# Suggest some values for the clearing price based on previous
# auctions
# @param paths, a list of Paths
# @param auctionName a valid auction name
# @return a data.frame to paste on the template xlsx
#
.suggest_clearingprices_input <- function(paths,
                                       auctionName)
{
  auction <- parse_auctionName(auctionName, region=paths[[1]]$auction$region)
  
  newPaths <- lapply(paths, function(path) {
    Path(source.ptid = path$source.ptid,
         sink.ptid   = path$sink.ptid,
         bucket      = path$bucket,
         auction     = auction,
         mw          = path$mw)
  })
  do.call(rbind, lapply(newPaths, as.data.frame))
  
  data.frame(CP=get_clearing_price( newPaths ))
}



#################################################################
# Suggest constraints input for the solver
# @param paths, a list of Paths
# @bc a zoo object with binding constraints for the historical
#   period you want as returned by ..binding_constraints_as.zoo 
# @param threshold.mean, keep constraints with
#    abs(Mean) shaddowprice > threshold.mean
# @param max.mw.exposure how many mw exposure you can take 
#    on this constraint.
# @return a data.frame to paste on the template xlsx
#
.suggest_constraints_input <- function(paths,
                                       bc,
                                       threshold.mean=0.01,
                                       max.mw.exposure=50)
{
  # which constraints are important for these paths
  constraints <- get_constraints_for_paths( paths )

  ## auction <- paths[[1]]$auction
  ## interval <- Interval(paste(auction$startDt, "01:00:00"),
  ##                      paste(auction$endDt+1, "00:00:00"))
  
  ## bc <- ..binding_constraints_as.zoo( interval )  

  # some simple binding constraint stats    
  bc.stats <- t(apply(bc, 2, function(x) {
    c(summary(x), sd=sd(x)) }))
  bc.stats <- bc.stats[order(-bc.stats[,"Mean"]),]
  bc.stats <- cbind(constraint = rownames(bc.stats),
                    as.data.frame(bc.stats))
  rownames(bc.stats) <- NULL
  bc.stats <- bc.stats[order(bc.stats$constraint),]  # order them

  # keep only the ones you need
  bc.stats <- subset(bc.stats, constraint %in% constraints)
  bc.stats <- subset(bc.stats, abs(Mean) > threshold.mean)
  
  hrs <- count_hours_auction(paths[[1]]$auction$auctionName,
                             region="NYPP", buckets="FLAT")
  res <- cbind(bc.stats[, c("constraint", "Mean", "sd")],
           min_exposure = -round(max.mw.exposure*abs(bc.stats$Mean*hrs$hours), -3),
           max_exposure =  round(max.mw.exposure*abs(bc.stats$Mean*hrs$hours), -3))
  res <- subset(res, max_exposure > 1000)  # don't bother with less

  
  res
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
  require(CRR)

  load_ISO_environment(region="NYPP")
 

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.analysis.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/FTR/lib.FTR.optimization.v2.R")

  #########################################################################
  # optimize a set of paths
  #
  filename <- "S:/Trading/Nodal FTRs/NYISO/Auctions/X14-6M-R4/allBids.xlsx"
  sheetName <- "Optimization"
  
  inp <- read_xlsx_template(filename, sheetName)
  
  sol <- find_solution( inp )
 

  


  
  #########################################################################
  # optimize from a template
  #
  ## filename <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/FTR/optimization_template.xlsx"
  ## sheetName <- "P3C2"
  ## inp <- read_xlsx_template(filename, sheetName)

  ## sol <- find_solution(inp)
  


  

}
