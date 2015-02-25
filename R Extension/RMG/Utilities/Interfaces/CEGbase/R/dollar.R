# Format numbers in dollar format, like Excel
#
#
# Written by Adrian Dragulescu on 16-Feb-2006

dollar <- function(x, digits=2, do.dollar=TRUE)
{
    # we take the abs here because we're going to check if it's negative
    # below. We don't want the negative sign when it's a string
    roundedNumber = abs(round(x, digits=digits))
    
    withCommas = prettyNum(roundedNumber, big.mark=",", nsmall=digits,
            preserve.width="individual")
    
    idx = which( x < 0 )
    withCommas[idx] = paste( "(", withCommas[idx], ")", sep="" )
    
    if (do.dollar) withCommas = paste( "$", withCommas, sep="" )
    
    return( withCommas )
}


dollarShortForm <- function(x, digits=2, do.dollar=TRUE)
{
    # we take the abs here because we're going to check if it's negative
    # below. We don't want the negative sign when it's a string
    ind.skip <- x %in% c(0, NA, NaN, Inf, -Inf)
    numbers = abs( x )

    pow10   <- trunc(log(numbers,10))  # get the power of 10
    mult10  <- pow10%/%3               # get the multiplier
    multMAP <- structure(c("", "k", "MM", "B", "T"), names=c(0, 1, 2, 3, 4))
    labels  <- multMAP[as.character(mult10)]
        
    # Get it down to <=3 digits prior to the decimal
    numbers[!ind.skip] <- numbers[!ind.skip]/10^(3*mult10[!ind.skip])
    numbers <- prettyNum(round(numbers, digits), nsmall=digits)
    
    numbers[!ind.skip] = paste( numbers[!ind.skip], labels, sep="" )
    
    negative = which( x < 0 )
    numbers[negative] = paste( "(", numbers[negative], ")", sep="" )
    
    if( do.dollar )
      numbers[!ind.skip] = paste( "$", numbers[!ind.skip], sep="" )
    
    return( numbers )
}



