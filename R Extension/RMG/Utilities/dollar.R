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


.testDollar <- function()
{
    library(RUnit)
    checkEquals("$4.50", dollar(4.5))
    checkEquals("$0.51", dollar(0.514))
    checkEquals("$1,024.52", dollar(1024.515))
    checkEquals("$(4.50)", dollar(-4.5))
    checkEquals("$(0.51)", dollar(-.514))
    checkEquals("$(1,024.52)", dollar(-1024.515))
    checkEquals("$0.00", dollar(0))
    checkEquals("$6,950,046.00", dollar(6950046))
    checkEquals("$(6,950,046.00)", dollar(-6950046))
}


dollarShortForm <- function(x, do.dollar=TRUE)
{
    # we take the abs here because we're going to check if it's negative
    # below. We don't want the negative sign when it's a string
    numbers = abs( x )
    
    # determine the k,MM,B,T label that should be used
    labels = rep( "", length( numbers ) )
    labels[ which( numbers >= 1000 & numbers < 1000000 ) ] = "k"
    labels[ which( numbers >= 1000000 & numbers < 1000000000 ) ] = "MM"
    labels[ which( numbers >= 1000000000 & numbers < 1000000000000 ) ] = "B"
    labels[ which( numbers >= 1000000000000 & numbers < 1000000000000000 ) ] = "T"
    
    # Get it down to <=3 digits prior to the decimal
    numbers[ which( numbers >= 1000 & numbers < 1000000 ) ] = numbers / 1000
    numbers[ which( numbers >= 1000000 & numbers < 1000000000 ) ] = numbers / 1000000
    numbers[ which( numbers >= 1000000000 & numbers < 1000000000000 ) ] = numbers / 1000000000
    numbers[ which( numbers >= 1000000000000 & numbers < 1000000000000000 ) ] = numbers / 1000000000000
    
    # Based on the number of digits prior to the decimal, determine the 
    # proper formatting
    rounded = trunc( numbers )
    
    lengthZero = which( nchar( rounded ) == 0 )
    if( length( lengthZero ) )
    {
        numbers[ lengthZero ] = round( numbers[ lengthZero ], 3 )
        numbers[ lengthZero ] = prettyNum( numbers[ lengthZero ], nsmall = 3, 
                preserve.width="individual" )
    }
    
    lengthOne = which( nchar( rounded ) == 1 )
    if( length( lengthOne ) )
    {
        numbers[ lengthOne ] = round( numbers[ lengthOne ], 2 )
        numbers[ lengthOne ] = prettyNum( numbers[ lengthOne ], nsmall = 2, 
                preserve.width="individual" )
    }
    
    lengthTwo = which( nchar( rounded ) == 2 )
    if( length( lengthTwo ) )
    {
        numbers[ lengthTwo ] = round( numbers[ lengthTwo ], 1 )
        numbers[ lengthTwo ] = prettyNum( numbers[ lengthTwo ], nsmall = 1, 
                preserve.width="individual" )
    }
    
    lengthThree = which( nchar( rounded ) == 3 )
    if( length( lengthThree ) )
    {    
        numbers[ lengthThree ] = round( numbers[ lengthThree ], 0 )
        numbers[ lengthThree ] = prettyNum( numbers[ lengthThree ], nsmall = 0, 
                preserve.width="individual" )
    }
    
    numbers = paste( numbers, labels, sep="" )
    
    negative = which( x < 0 )
    numbers[negative] = paste( "(", numbers[negative], ")", sep="" )
    
    if( do.dollar ) numbers = paste( "$", numbers, sep="" )
    
    return( numbers )
}


.testDollarShortForm <- function()
{
    library(RUnit)
    checkEquals("$4.50", dollarShortForm(4.5) )
    checkEquals("$0.51", dollarShortForm(0.51434) )
    checkEquals("$1.02k", dollarShortForm(1024.515, 2) )
    checkEquals("$(4.50)", dollarShortForm(-4.5) )
    checkEquals("$(0.51)", dollarShortForm(-.514) )
    checkEquals("$(1.02k)", dollarShortForm(-1024.515) )
    checkEquals("$0.00", dollarShortForm(0) )
    checkEquals("$6.95MM", dollarShortForm(6950046) )
    checkEquals("$(6.95MM)", dollarShortForm(-6950046) )
    checkEquals("6.95B", dollarShortForm(6950046000, FALSE) )
    checkEquals("6.95T", dollarShortForm(6950046000123, FALSE) )
    checkEquals("695k", dollarShortForm(695123.234, FALSE) )
    checkEquals("$695k", dollarShortForm(695123.13464, TRUE) )
    checkEquals("$(69.4k)", dollarShortForm(-69412, TRUE) )
}





