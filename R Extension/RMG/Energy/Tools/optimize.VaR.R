################################################################################
# Function to optimize VaR along several dimensions.  Do optimization only 
# on groups of variables (you can have groups of one variable).  This way, 
# you impose realistic trading constraints, for example, trading a calendar 
# strip only, or Jan-Feb months together.  See below for an example of the
# limits.xls file.  It needs to contain a sheet called "Limit".  The column
# "group" will show which curvemonths get optimized together.  It is assumed
# that the "Limit" amount is the same within a group.
#
# curve.name	          contract.dt    vol.type   Limit       group
# COMMOD NG EXCHANGE	  2007-10-01	   NA			
# COMMOD NG EXCHANGE	  2007-11-01	   NA			
# COMMOD NG EXCHANGE	  2007-12-01	   NA			
# COMMOD NG EXCHANGE	  2008-01-01	   NA	        1,000,000 	1
# COMMOD NG EXCHANGE	  2008-02-01	   NA	        1,000,000 	1
#
# source("H:/user/R/RMG/Energy/Tools/optimize.VaR.R")


################################################################################
# EDIT THESE SETTINGS BELOW FOR YOUR SPECIFIC RUN
IVAR_FILE = "//ceg/cershare/All/Risk/Reports/VaR/prod/2008-02-21/east.power.trading.portfolio/IVaR.east.power.trading.portfolio.xls"
LIMITS_FILE = "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/overnight/Optimizer/limits/east.power.trading.portfolio.xls"

# Note: Live positions won't work for synthetic portfolios!
USE_LIVE_POSITIONS = FALSE

# Change to a number if you want to set a VaR value from SAS
SAS_VAR = NULL  


################################################################################
# DO NOT EDIT BELOW THIS LINE!
#
source("H:/user/R/RMG/Energy/Tools/OptimizerEngine.R")
start = Sys.time()
OptimizerEngine$main( IVAR_FILE, LIMITS_FILE, USE_LIVE_POSITIONS, SAS_VAR )
print(Sys.time()-start)

