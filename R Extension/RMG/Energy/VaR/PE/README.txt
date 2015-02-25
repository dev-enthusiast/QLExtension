2008-04-10 (Plots of simulated price changes with historical changes are added as a 
way of backtesting)
==========================================================================
**MPE Problem Diagnosis**
-------------------------
Some curves showed upward trends that cannot be captured easily with OU simulation



2008-03-20 (Problems temporarily solved, there are other issues to be fixed later)
==========================================================================
**MPE Problem Diagnosis**
-------------------------
For Emerging Market CPs
--BAYANRES: This counterparty is composed all of FCO curves, which are highly correlated. 
	    Some curves show that on some occasions the simulated prices 
	    will start at a higher price than F0 on asOfDate.
	    This problem is still under investigation. 
	    Such curves include, e.g. COMMOD FCO INDOBITmED PHYSICAL, 
				  COMMOD FCO FOB INDOHSP PHYSICAL,

--KPCOAL:  This CP involves FCO curves. Similar to BAYANRES, price jumps in the FCO curves
	      may cause MPE to be very high.

--BUNGEGENEV, SK.EUROPE, TANGSHAN, SWISSMARIN: These CPs contain FRT curves.
	   On some occasions(not always, however), prices for some FRT curves are simulated much 
	   higher than the historical level. This problem is still under investigation.
               Such curves include, e.g.: COMMOD FRT ROUTE4 BCI
	   
--SWISSMARIN,CARGILLINT: These CP's contain FRC curves. Some FRC curves showed 
	   very high variations in simulated prices, although the mean level 
	   remains reasonable. Such high variations cause the upper percentiles
	   (95%, 99%) to be high. This problem is still under investigation.
	   Curves include, e.g.: COMMOD FRC PM TC PHYSICAL

--CARGILLINT: This CP has a variety of curves involved, including FRC, FRT, 
	      FCO curves. These curves have problems as stated above.
	      This CP also has PWG GER PEAK/OFFPEAK curves, with 
	      high oscillation and seasonality in their historical prices. Accordingly, there is
	      a high variation in simulated results, which also pulled up MPE.
	      GER curves are still under investigation.

**Other Notes**
----------------
--The following curves do not have valid historical prices (e.g., NA or all flat):
		COMMOD FRT USA
		COMMOD FRT VENEZUELA
		COMMOD FRT RUSSIA	
		COMMOD FRT ROUTE12 BCI
		COMMOD FRT SOUTH AFRICA
        	Simulation of these curves are not needed. The code sim.forward.R() 
		needs to be checked for such problems.




2008-02-20
==========================================================================
TODO's for PE:
- Wrote the positions in the output xls.  Make the results xls come out 
  overnight too. (DONE)



2008-02-18
==========================================================================
TODO's for PE:
- There are still problems with the FRC CAPEAVETC curve.  (DONE)

- Try synthetic PE runs with the RMG Freight Dispensation portfolio. (DONE. 
  Added a function to work with generic combination of counterparties.)

- Fix the NG curves.  It does not save the dependent curves anymore. (DONE. 
  Wrote a function to copy the filehash files after the aggregation is done. )

- Start using the template.deal.review.MPE.R more to find potential bugs. 


2008-01-10
==========================================================================
TODO's for PE: 
- have set the sPP.env <<- .getFileHashEnv(PE$CURVE_DATA_DIR) as
 global.  I should check if the user has changed the asOfDate!

- have CSV/xls files with results go out too.  All 

- Something is funny with the PE on the daily price simulation.  Why
  does it swing so much? 

- Put each run in its own folder.  It's better. 


2007-10-23
==========================================================================


Price Simulation (TO DO's): 
---------------------------
 - NOX & SO2 curves still simulate to NA (probably due to input data
   not clean) 

 - Do some more testing on the quality of simulations. Check COL, FRT
   curves.  Seemed OK at cursory look. 

 - Connect/spruce up Hao's automatic reporting code. 

 - BUG: Some of the pricing.dts are not beginning of month.  Maybe
   it's a feature, but we should change it. 


Potential Exposure TO DO's:
---------------------------
 - ~30 counterparties fail due to bad SO2 & NOX curves.	

 - Realized exposure is not yet implemented as a back test. 

 - Compare Matlab and R numbers for simple deals. 
 
 - Contact Sumit to prospect Web GUI (almost in prod) interaction to
   R.
 




2007-10-08
==========================================================================
Price Simulation Current Status: 
--------------------------------
 - The prices now simulate every night at 2am. If this interferes with
   the overnight VaR run, it can very easily be moved until 9am or
   later.

 - Memory issue with NG Curve Simulations solved by splitting the
   1000 sims into packets. (DONE)

Price Simulation (TO DO's): 
---------------------------
 - NOX & SO2 curves still simulate to NA (probably due to input data
   not clean) 

 - Do some more testing on the quality of simulations. Check COL, FRT
   curves.

 - Connect/spruce up Hao's automatic reporting code.

 - BUG: Some of the pricing.dts are not beginning of month.  Maybe
   it's a feature, but we should change it. 


Potential Exposure Current Status:
----------------------------------
 - Counterparty simulation (both indirect and direct) occurs
   immediately after the price simulation.  

 - PDF exposure graphs and corresponding R data files are currently
   saved for every night's run.

 - Interactive capability is provided (we need to iron out who will
   use this & when)

 - Initial draft of credit portfolio report is completed (pending
   Credit's approval)

 - Data aggregation has not yet been added to the Condor job. (DONE)
 
 - Prices are not currently extended past their simulation dates for
   use in the PE calculator. (DONE)


Potential Exposure TO DO's:
---------------------------
 - ~30 counterparties fail due to bad SO2 & NOX curves.	

 - Realized exposure is not yet implemented as a back test. 

 - Compare Matlab and R numbers for simple deals. 
 
 - Contact Sumit to prospect Web GUI (almost in prod) interaction to
   R.
 
 - Work with Jenny to get a function to run position reports on SecDB
   calculators.  Gave her the specs, waiting on implementation. 





15-Jun-2007
===========

Credit VaR
----------

CCG has about 30,000 risk factors (curve.name x contract.dt)
combinations.  We want to do 1000 simulations, for about 30 future
pricing dates (horizons).  The storage needed for this is: 
  30,000 x 1000 x 30 x 8 = 7.2 GB 

Unique combination of curve.names x contract.dt by year:
2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017 2018 2019 2020 2021 2022 
7643 4826 3811 3083 2471 1531  955  846  820  521  339  238  228   60   60    6

Problem to solve:
 - Generate independent random numbers for the Condor runs.  To check
 package rstream.

Store the price simulations by batches of 50 sims.  20 files, each about 80
MB.  The distribution of positions (counterparty x curve.name x
contract.dt) by year: 
 2007  2008  2009  2010  2011  2012  2013  2014  2015  2016  2017  2018  2019  2020  2021  2022 
32521 28412 16259 10007  5443  2897  1780  1442  1384   895   695   450   456   260   242     6 

Potential Exposure
------------------

The PE calculation can be done as follows.  All positions by
counterparty: counterparty x curve.name x contract.dt have 120,000
rows (about 15 MB). 

The PE runs are split by counterparties that begin with the same letter   
except for counterparties with more than 1000 curve.name + contract.dt
which are treated as a separate run. 

PE storage requirements.  There are 6661 cparty + curve.name
combinations.  Assuming 54 pricing.dts, you get 360,000 cparty +
curve.name + pricing.dt combinations.  With 1000 simulations, you need 
360,000 x 1000 x 8 = 3 GB.  Not bad!  This will be our final data set.

