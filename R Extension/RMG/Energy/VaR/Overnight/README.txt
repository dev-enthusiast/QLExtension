# This folder should contain R files that prepare the data for VaR
# processing, stress tests and other jobs.  Keep a dependency tree in
# here. 
# 
#
# 

 - Read historical prices for last day:  
     get.one.day.prices.R from get.historical.prices.R
 - Read historical vols for the last day:
     get.one.day.vols.R from get.historical.prices.R
 - Read corrected prices for the last 71 days:
     get.corrected.marks.R
 - do the same for the vols......................

 - Do the price buddy get.price.buddy.curve.R
 
 - Do the vol buddy get.vol.buddy.curve.R

