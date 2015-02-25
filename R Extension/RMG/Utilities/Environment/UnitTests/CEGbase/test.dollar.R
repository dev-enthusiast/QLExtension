#
#
#

test.dollar <- function()
{
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





