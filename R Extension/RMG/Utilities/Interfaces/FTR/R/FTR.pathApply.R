`FTR.pathApply` <-
function( Paths=NULL, pTS=NULL, ISO.env, FUN, ... )
{
  FUN <- if (!is.null(FUN)) match.fun(FUN)

  if (!is.null(Paths) & !is.null(pTS))
    stop("Only one of the two first arguments can be NULL.")
  
  if (is.null(Paths)){ 
    return( .FTR.pathApplyBatch( pTS, FUN, ... ) )
  }
  
  batch.size <- 10000
  batches <- trunc(nrow(Paths)/batch.size)+1

  res <- NULL
  for (b in seq_along(1:batches)){
    rLog(paste("Working on batch no.", b))
    ind.b <- ((b-1)*batch.size+1):min(b*batch.size, nrow(Paths))
    hSP <- FTR.get.hSP.for.paths(Paths[ind.b, ], ISO.env)
    res <- rbind(res, .FTR.pathApplyBatch(hSP, FUN, ...))
  }
  rownames(res) <- NULL
  
  return(res)
}

################################################################################
# Work function.  Makee invisible.  Apply a function to a set of paths.
#
.FTR.pathApplyBatch <- function( pTS, FUN, ... )
{
  FUN <- if (!is.null(FUN)) match.fun(FUN)

  res <- apply(pTS, 2, FUN, ...)
  if (class(res)=="numeric")
    res <- as.matrix(res)
  else
    res <- t(res)
  res <- data.frame(path.no=as.numeric(rownames(res)), res)
  rownames(res) <- NULL

  return(res)
}
