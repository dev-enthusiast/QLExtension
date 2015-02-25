# R_analyses
#     Analysis tools to be used by ERCOT PM
# Written by Lori Simpson on 25-May-2011


#------------------------------------------------------------
# Do Not modify - run this code only once
# 
    ## Get source code
        # strHeader = "H:/user/"      
        strHeader = "S:/All/Portfolio Management/ERCOT/R/"  ## as variable passed into the functions
        source(paste(strHeader,"R/RMG/Energy/PM/ERCOT/desk_tools/R_analyses.lib.R",sep=""))   ## For testing
        source(paste(strHeader,"R/RMG/Energy/PM/ERCOT/dam/lib.dam.R",paste="",sep=""))   ## For testing
    ## Get data (This may take a couple minutes, but you only have to do once)
      historical = get.nodal.zonal.data (startyear=2005, endyear=2011)
            # historical = read.csv(file="S:/All/Portfolio Management/Simpson/Second Simpson/R/written docs/desk_tools_historical.csv");



#------------------------------------------------------------
# Choose options and Run function
# 

    ## Choose time variables
      Startyear=2012                  ## startyear=2006
      Endyear=2012                    ## endyear=2006
      Include_months=c(3,4)           ## include_months=c(6,7) or include_months=0 to include all months
      Exclude_years=0                 ## exclude_years=c(2006,2007) or exclude_years=0 to exclude NO years
      Bucket="5x16"                   ## bucket="5x16" or bucket="7x8" or bucket="2x16h" or bucket="7x24" or bucket="wrap"
    ## Choose other variables
      Market="RT"                     ## Market="RT"; Market="DA"; Market="OTC";
      Gas="HH"                        ## Gas="HH" for henry hub; Gas="HSC" for ship channel
      SP_analysis="HB_NORTH"        ## sp_analysi="HB_NORTH"
      Combine_years_on_graph=FALSE     ## choose FALSE if you want a separate graph by year


    ## Run function
      arrResults = do.analysis(arrHistorical=historical, startyear=Startyear, endyear=Endyear, conn=channel_zema_prod, include_months=Include_months, exclude_years=Exclude_years, bket=Bucket, sp_choice=SP_analysis, combine_years_on_graph=Combine_years_on_graph, market=Market, gas=Gas)
      print(arrResults)




   ## Additional variables you might like to set
      yaxismin=FALSE                  ## controls the axis, set equal to FALSE to give default limits, yaxismin=FALSE, yaxismin=10
      yaxismax=15                  ## controls the axis, set equal to FALSE to give default limits, yaxismin=FALSE, yaxismin=10

   ## Run function with additional variables
      arrResults = do.analysis(arrHistorical=historical, startyear=Startyear, endyear=Endyear, conn=channel_zema_prod, include_months=Include_months, exclude_years=Exclude_years, bket=Bucket, sp_choice=SP_analysis, ymin=yaxismin, ymax=yaxismax)
      print(arrResults)




#------------------------------------------------------------
# Run before closing
# 
     odbcCloseAll()
     rm(list=ls())







