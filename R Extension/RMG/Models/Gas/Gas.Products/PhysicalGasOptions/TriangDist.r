# Generate Random Variables from a Triangular Distribution Function
# n is the number of draws.
# min.val: the minimum value
# mode.val: the mode value
# max.val: the maximum value

#rtriang(n=10, min.val=4, mode.val=7, max.val=9)
rtriang <- function(n, min.val, mode.val, max.val){
   
    x = rep(NA, n)
    probs = runif(n)
    
    # The following function, ptriang.diff,  is simply a probabiliy value (runif(1)) minus the Triangular distribution
    # function.
    # I want to solve for x such that the value of ptriang.diff is zero.
     
    ptriang.diff <- function(x, min.val, mode.val, max.val, proba){
      if( x >= min.val & x <= mode.val){dist.val <- proba-((x-min.val)^2/((max.val - min.val)*(mode.val-min.val)))}

      if(x > mode.val & x <= max.val){dist.val <- proba-(1-(max.val-x)^2/((max.val - min.val)*(max.val-mode.val)))}
      dist.val
    }

    for( i in 1:n){
      proba = probs[i]

      x[i] <- uniroot(ptriang.diff, interval=c(min.val,max.val), min.val=min.val, mode.val=mode.val, max.val=max.val, proba=proba)$root

    }
return(x)
}


  
