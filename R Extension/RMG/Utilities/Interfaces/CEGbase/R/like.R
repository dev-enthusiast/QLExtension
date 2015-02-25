`%like%` <- function(vector, pattern)
{
  if (is.factor(vector)) {
    which(as.integer(vector) %in% grep(pattern,levels(vector)))
  } else {
    which(regexpr(pattern,vector)>0)
  }    
}



