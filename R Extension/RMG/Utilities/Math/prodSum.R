################################################################################
#function to take two vectors, multiply across elements and then
#sum the resultsing vector
#
#author: Sean McWilliams
#
################################################################################

Math = new.env(hash = TRUE)

################################################################################

Math$prodSum <- function(vectorOne, vectorTwo)
{
    newVector <- vectorOne * vectorTwo
    sumVector <- sum(newVector)
    return(sumVector)
    
}