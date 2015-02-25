# Round to a given number
#
#

round_to <- function(x, to=25)
{
  xMin <- to*(x %/% to)
  xMax <- to*(x %/% to + 1)
  ifelse(x - xMin > to/2, xMax, xMin)
}


.test.round_to <- function()
{
  round_to(c(1412.1, 1430, 1445, 1455, -1412, -1430), to=25)
}
