# Get positions for an interactive run.
# If the run is interactive with archive data, check to see if the positions
# file with this run name already exists.
#  - For portfolios or books, use the nightly positions file
#    all.positions.yyyy-mm-dd and the portfolio hierarchy to subset the
#    positions.
#  - For trades, get the data directly from VCentral.
#
# If the run is intraday, check to see if the positions file with this run name
#   already exists.  If not, get the overnight positions first, then append
#   the intraday trades. 
#
# Written by Adrian Dragulescu on 2-Dec-2006

get.interactive.positions <- function(run, options){

  


}
