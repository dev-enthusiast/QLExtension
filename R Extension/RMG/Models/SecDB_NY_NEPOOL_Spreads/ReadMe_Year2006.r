Forward prices on illiquidly traded locations are constructed from:

1. Base curves, actively traded and marked by traders
2. Spread, regressed using historical DA prices, per bucket per month

The spread regression is implemented inside SecDB as a linear regression and validated
by RMG, Q4_Year2004.

PWC requested the validation of such regression for Year 2005, especially at Dec 30th, 2005
This directory hosts the codes used and results found for Year 2005.

For NYPP regression inside SecDB, the default start and end dates for historical DA prices are hard-coded. 
Each time this regrssion is run, the guy in charge of periodically running this
regress manually change the start and end dates, and the chosen start and end dates
are written into:

            BAL Close PWY Spreads. Regressions. [Zone].Start
            BAL Close PWY Spreads. Regressions. [Zone].End
                  
Zone could be CEN, HQ and etc.
As of Jan 09, 2006, for Central we have

Nov 1, 2000, 01:00:00AM  --->   Dec 1, 2005, 00:00:00AM

and We will run regression on:

NYPP_DAM_ZONE_LBM_61754; Zone C, Central
NYPP_DAM_GEN_LBM_23744; Nine Mile 2, NMP2

For NEPOOl regression inside SecDB, the default start and end dates for historical DA prices are hard-coded. 
Each time this regrssion is run, the guy in charge of periodically running this
regress manually change the start and end dates, and the chosen start and end dates
are written into:

            BAL Close PWX Spreads. Regressions. [Zone].Start
            BAL Close PWX Spreads. Regressions. [Zone].End
                  
Zone could be CEN, HQ and etc.  

However, BAL Close PWX Spreads.Regions. CT has not fields named as Start and End.
In fact, the Start and End dates could be found at 

            BAL Close PWX Spreads. Congif.Start Date
            BAL Close PWX Spreads. Congif.Start Date


As of Jan 09, 2006, for NEPOOL we have:

            Mar, 15, 2003 ---> Sep 30, 2005

and We will run regression on:

NEPOOL_LMP_DA_CT ; Zone CT
NEPOOL_SMD_DA_485_LMP; Milstone3

_UT Elec NEPOOL Spread Regress is the Slang script regressing, it has:

1. FiltLow = True
2. FileHigh = True
3. eps = 0.1
4. Maxprice = 200

TsdbFunction removes Zeros but not Extremely High prices above $200

Statistical Details:

1. Linear Model would give back more information about reg

  coefficients(reg) provides intercept and slope
  confint(reg) provides confidence interval for estimated parameters, by default 2.5% - 97.5%
  
2. predict(reg) provides confidence intervals for predictted values, therefore would be explictly
  published and included in the report.

Also, there is difference between the SecDB and RMG regression in technical details

SecDB will 
1. Remove hourly prices between eps and Maxprice (0.1 - 200) 
2. Fill in hourly prices (Intropolate)
3. Bucketize hourly prices into Bucket Prices 

RMG will
1. Bucketize hourly prices into Bucket Prices
2. Remove Bucket Prices between eps and Maxprice (0.1 - 200)
3. No fill the Bucket Prices

While the SecDB approach completely ignores prices outside eps and Maxprice 
RMG first see how big the impact will be and remove partial the impact.