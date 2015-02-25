###############################################################################
# OptimizerEngine.R
#
#


################################################################################
# Required library files
library(RODBC)
library(reshape)
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")


################################################################################
# File Namespace
#
OptimizerEngine = new.env(hash=TRUE)


################################################################################
# read the spreadsheet with limits 
OptimizerEngine$getLimitsFromFile <- function( ivarData, limitsFile )
{  
    if( !file.exists(limitsFile) )
    {
        stop(paste("The file:", limitsFile, "does not exist.", 
                        "\nCheck your path and try again.\n\n"))
    }
    
    con  <- odbcConnectExcel(limitsFile)
    LL   <- sqlFetch(con, "Limit")
    odbcCloseAll()

    LL <- subset(LL, toupper(LL$include)=="YES")  # pick only the ones you want to include
    LL$include <- NULL
    
    ind <- which(apply(LL, 1, function(x){all(is.na(x))}))
    if (length(ind)>0){LL <- LL[-ind,]}   # remove empty lines
    names(LL) <- gsub("#", ".", names(LL))
    LL$ratio <- ifelse(is.na(LL$ratio), 1, LL$ratio)
    LL$limit.left  <- ifelse(is.na(LL$limit.left), 0, LL$limit.left)
    LL$limit.right <- ifelse(is.na(LL$limit.right), 0, LL$limit.right)
    LL$curve.name  <- toupper(LL$curve.name)
    LL$comments    <- as.character(LL$comments)
   
    return(LL)
}

################################################################################
# read the spreadsheet with limits 
OptimizerEngine$getExtendedLimits <- function( LL )
{
    LL.ext <- NULL     # explode the limits into monthly ones 
    for (r in 1:nrow(LL)){
        startDate = as.Date(LL$contract.dt.start[r])
        endDate = as.Date(LL$contract.dt.end[r])
        months <- seq( startDate, endDate, by="month" )
        LL.ext <- rbind(LL.ext, data.frame(
                        curve.name  = toupper(LL$curve.name[r]),
                        contract.dt = as.character(months),
                        limit.left  = LL$limit.left[r],
                        limit.right = LL$limit.right[r],
                        ratio       = LL$ratio[r],                                 
                        comments    = LL$comments[r],
                        vol.type    = NA_character_))   # don't modify vega positions too
    }
    LL.ext$contract.dt <- as.Date(LL.ext$contract.dt)
    LL.ext$ratio  <- ifelse(is.na(LL.ext$ratio), 1, LL.ext$ratio)
    LL.ext$group  <- as.character(LL.ext$group)

    # make calendar positions monthly for the optimizer
    count <- aggregate(rep(1, nrow(LL.ext)), as.list(LL.ext[, c("curve.name", "comments")]), sum)
    colnames(count)[3] <- "count"

    LL <- merge(LL, count, sort=F)
    LL$limit.left.monthly  <- LL$limit.left/LL$count
    LL$limit.right.monthly <- LL$limit.right/LL$count
    
    return( list(LL.ext, LL) )
}



################################################################################
# read the spreadsheet with ivar data 
OptimizerEngine$getIVaRFromFile <- function( ivarFile )
{  
    if( !file.exists(ivarFile) )
    {
        cat("\nThe file:", ivarFile, "does not exist.", 
                "\nCheck your path and try again.\n\n")
        return(NULL)
    }
    
    require(RODBC)
    con    <- odbcConnectExcel(ivarFile)
    data   <- sqlFetch(con, "Changes")
    odbcCloseAll()
    
    data$Limit <- NULL
    names(data) <- gsub("#", ".", names(data)) 
    return(data)
}


################################################################################
# 
OptimizerEngine$main <- function( ivarFile, limitsFile, useLivePositions=FALSE, sasVaR=NULL )
{
    # read the ivar info from spreadsheet
    ivarData = OptimizerEngine$getIVaRFromFile( ivarFile ) 
    
    if( useLivePositions )
    {
        asOfDate = overnightUtils.getAsOfDate()
        
        portfolio = basename(ivarFile)
        portfolio = gsub("(IVaR\\.|\\.xls)", "" , portfolio)
        portfolio = gsub("\\.", " ", portfolio)
        
        ivarData = OptimizerEngine$getLiveIVaRData( portfolio, asOfDate, ivarData ) 
    }
    
    # read the limits info from spreadsheet
    LL    <- OptimizerEngine$getLimitsFromFile( ivarData, limitsFile )
    
    aux <- OptimizerEngine$getExtendedLimits( LL )
    LL.ext <- aux[[1]]
    LL     <- aux[[2]]

    data <- merge(LL.ext, ivarData, sort=FALSE, all.y=TRUE)  
    
    OptimizerEngine$optimizeDataSet( data, LL, sasVaR )
}


################################################################################
# read the live positions from CPSPROD and join it with the ivar file
OptimizerEngine$getLiveIVaRData <- function( portfolio, asOfDate, ivarData )
{
    positions = LiveData$cpsprodPositions()
    
    options = NULL
    options$asOfDate = asOfDate
    options$run.name = portfolio
    portfolioBooks = as.character(get.portfolio.book(options)$BOOK)
    
    livePositions = subset( positions, positions$book %in% portfolioBooks )
    livePositions$book = NULL
    names(livePositions) = c("curve.name", "contract.dt", "value")
    livePositions = cast(livePositions, curve.name+contract.dt~., sum)
    names(livePositions) = c("curve.name", "contract.dt", "position")
    
    volRows = which( !is.na(ivarData$vol.type) )
    volData = ivarData[volRows,]
    
    deltaPositions = ivarData[-volRows,]
    deltaPositions$curve.name = as.character(deltaPositions$curve.name)
    deltaPositions$contract.dt = as.Date(deltaPositions$contract.dt)
    deltaPositions$position = NULL
    
    newDeltas = merge( livePositions, deltaPositions, 
            by=c("curve.name", "contract.dt") )
    
    newPositions = rbind(newDeltas, volData)
    newPositions[,1:4] = newPositions[, c(1,2,4,3)]
    names(newPositions)[1:4] = c("curve.name", "contract.dt", "vol.type", "position")
    
    return(newPositions)
}

################################################################################
# substitute the solution back into the IVaR to see if you recover the
# optimization result
#
OptimizerEngine$check.solution <- function( data, LL, res )
{  
  indDates <- grep("-", names(data))
  aux <- merge(data, LL[,c("curve.name", "comments", "monthly.change")], all=T, sort=F)
  aux$monthly.change[is.na(aux$monthly.change)] <- 0
  aux$position <- aux$position + aux$monthly.change   # add the proposed changes
  CM <- cov(t(aux[,indDates]))        # covariance matrix

  VaR.new <- as.numeric(4*sqrt(aux$position %*% CM %*% aux$position))
  min.VaR <- 4*sqrt(res$value)

  if (round(VaR.new) != round(min.VaR))
    stop("Cannot reproduce the optimized VaR by substituting back into the IVaR!")
}


################################################################################
OptimizerEngine$optimizeDataSet <- function( data, LL, sasVaR = NULL )
{
    indDates <- grep("-", names(data))
    CM <- cov(t(data[,indDates]))        # covariance matrix
    Q0.CM <- data$position %*% CM      
    
    # current (initial) VaR
    if ( is.null(sasVaR) )
    {
        VaR.0 <- (4*sqrt(data$position %*% CM %*% data$position))   
    } else 
    {
        VaR.0 <- sasVaR
    }
    
    ind <- which(!is.na(data$comments))
    ind.G <- split(ind, data$comments[ind])  # index for all groups

    groups <- unique(LL$comments)
    d <- rep(NA, length=length(groups))
    DD <- matrix(NA, nrow=length(groups), ncol=length(groups))
    Limits <- LL[!duplicated(LL$comments), c("limit.left.monthly", "limit.right.monthly")]
    rownames(Limits) <- LL$comments[!duplicated(LL$comments)]

    for (g in 1:length(groups)){
      group.g <- groups[g]
      pg <- matrix(rep(1, length(ind.G[[group.g]])))   # unit position for group "g"
      pg <- pg * data$ratio[ind.G[[group.g]]]       # != 1 only for gas in HRs
      d[g] <- 2*Q0.CM[ind.G[[group.g]]] %*% pg
      for (f in g:length(groups)){
        group.f <- groups[f]
        pf <- matrix(rep(1, length(ind.G[[group.f]]))) # unit position for group "f"
        pf <- pf * data$ratio[ind.G[[group.f]]]        # != 1 only for gas in HRs
        CM.gf   <- CM[ind.G[[group.g]], ind.G[[group.f]], drop=F] # covariance matrix for the two blocks
            
        DD[g,f] <- t(pg) %*% CM.gf %*% pf
        DD[f,g] <- DD[g,f]
      }
    }
    VV.0 <- (VaR.0/4)^2  # original variance
    
    objective.Fun <- function(q, d, DD, VV.0){
        as.numeric( d %*% q + q %*% DD %*% q + VV.0)}

    gradient.Fun <- function(q, d, DD, VV.0){
      as.numeric(2*DD %*% q + d)
    }

    q.0  <- rep(0, length(groups))     # the starting quantity increments
    q.LB <- Limits$limit.left          # has to be between (-\infty, 0] 
    q.UB <- Limits$limit.right         # has to be between [0, \infty)
    cat("Starting optimization ... ") 
    res  <- optim(q.0, objective.Fun, gradient.Fun, method="L-BFGS-B", lower=q.LB,
            upper=q.UB, control=list(trace=3, factr=1, maxit=1000), hessian=F, d, DD, VV.0)
    cat("Done.\n")
    
    LL <- merge(LL, data.frame(comments=groups, monthly.change=res$par), sort=F)
    LL$monthly.change <- LL$monthly.change * LL$ratio      # add the ratio
    LL$change <- LL$monthly.change * LL$count

    #LL <- subset(LL, abs(LL$change)>1)
    print(LL)    
    
    min.VaR <- 4*sqrt(res$value)
    cat("\n\nOriginal VaR:          ", prettyNum(as.numeric(VaR.0), big.mark=","), "\n")
    cat("VaR after optimization:", prettyNum(as.numeric(min.VaR), big.mark=","), "\n")

    OptimizerEngine$check.solution(data, LL, res)
    
    returnedData = NULL
    returnedData$original = as.numeric(VaR.0)
    returnedData$optimized = as.numeric(min.VaR)
    returnedData$difference = returnedData$original - returnedData$optimized
    returnedData$data = LL
    
    return(returnedData)
}
