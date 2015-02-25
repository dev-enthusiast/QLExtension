#
#
#

test.cast <- function()
{
  # check basic reshaping
  x <- data.frame(group=c(1,1,2,3,3,4), value=c(2,14,3,11,13,12))  
  current <- data.frame(cast(x, group ~., mean))
  target  <- data.frame(group=c(1,2,3,4), `(all)`=c(8,3,12,12))
  checkEquals(current, target)

  # check reshaping with unbalanced data
  x <- data.frame(group=c(1,1,2,3,3,4), location=rep(c("A","B"),3), 
                  value=c(2,14,3,11,13,12))  
  current <- data.frame(cast(x, group ~ location, I, fill=NA))
  target  <- data.frame(group=c(1,2,3,4), A=c(2,3,13,NA), B=c(14, NA, 11, 12))
  checkEquals(current, target)

  # this one breaks, fill is not set! 
  x <- data.frame(group=c(1,1,2,3,3,4), location=rep(c("A","B"),3), 
                  value=c(2,14,3,11,13,12))  
  checkException(cast(x, group ~ location, I))


  # if you have a column called value, you cannot specify another column
  # as the true value column
  x <- data.frame(location=c("A","B", "C"),  value=c(1, 2, 3),
                  trueValue=c(100, 200, 300))
  res <- cast(x, location ~ ., I, fill=0, value="trueValue")
  checkEquals(res[1,2], 100)
  
  
}





