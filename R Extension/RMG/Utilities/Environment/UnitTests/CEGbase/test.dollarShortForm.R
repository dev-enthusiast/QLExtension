

test.dollarShortForm <- function()
{
    checkEquals("$4.50", dollarShortForm(4.5) )
    checkEquals("$0.51", dollarShortForm(0.514) )
    checkEquals("$1.02k", dollarShortForm(1024.515, 2) )
    checkEquals("$(4.50)", dollarShortForm(-4.5) )
    checkEquals("$(0.51)", dollarShortForm(-.514) )
    checkEquals("$(1.02k)", dollarShortForm(-1024.515, 2) )
    checkEquals("$0.00", dollarShortForm(0))
    checkEquals("$6.95MM", dollarShortForm(6950046) )
    checkEquals("$(6.95MM)", dollarShortForm(-6950046) )
    checkEquals("$7MM", dollarShortForm(6950046, 0) )
    checkEquals("7.0MM", dollarShortForm(6950046, 1, FALSE) )
    checkEquals("6.95B", dollarShortForm(6950046000, 2, FALSE) )
    checkEquals("6.95T", dollarShortForm(6950046000123, 2, FALSE) )
    checkEquals("695k", dollarShortForm(695123, 0, FALSE) )
    checkEquals("$695k", dollarShortForm(695123, 0, TRUE) )
    checkEquals("$(69k)", dollarShortForm(-69412, 0, TRUE) )
}  
