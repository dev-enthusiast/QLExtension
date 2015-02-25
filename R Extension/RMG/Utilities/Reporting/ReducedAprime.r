# source("S:/All/Risk/Software/R/prod/Utilities/Reporting/ReducedAprime.r")
# source("H:/user/R/RMG/Utilities/Reporting/ReducedAprime.r")

ReducedAprime <- function( Date )
{

  source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R")

  Date = as.Date( Date )

  rLog("Reading in Aprime ...")
  filepath = paste( "//ceg/cershare/Risk/CollateralAllocation/",
    format(Date, "%Y%m%d"), "/SourceData/AUTO_AllPos_TimingDiff_",
    toupper(format(Date, "%d%b%y")), ".csv", sep="")
  reducedAprime = read.csv(filepath, header = TRUE)

  #filter to columns needed plus a few useful ones
  rLog("Reducing Aprime ...")
  reducedAprime = subset( reducedAprime, T, select = c(book_name, commod_curve, 
    contractyear, contractquarter, contractmonth, policy_region, policy_subregion, 
    counterparty, Phy_Fin_flag, Commodity_grp, Commodity, Sttype, 
    credit_nettingagreement, Marginal, Margined, RiskPos, Risk_Reporting_flag, 
    CER_GROUP_2, LOAD_GEN_HEDGE, Bucket, pos_adj, mwh_pos, PQ1Pct) )

  #add portfolio stress test flags for PJM, NY and Other
  rLog( "Adding PortStressFlag ..." )
  reducedAprime$PortStressFlag = NA

  PJMrows = which( reducedAprime[["CER_GROUP_2"]] == "PGD CENTRAL" | 
    reducedAprime[["CER_GROUP_2"]] == "CSG CENTRAL" |
    ( reducedAprime[["CER_GROUP_2"]] == "CEG VIEW" & 
    reducedAprime[["book_name"]] %in% c( "CEGJVCC1", "CEGJVCC2" ) ) )
  NYrows = which( reducedAprime[["CER_GROUP_2"]] == "PGD NY" | 
    reducedAprime[["CER_GROUP_2"]] == "CSG NY" |
    ( reducedAprime[["CER_GROUP_2"]] == "CEG VIEW" & 
    reducedAprime[["book_name"]] %in% c( "CEGJVNM1", "CEGJVNM2", "CEGJVGIN" ) ) )
  
  if( length( which( PJMrows %in% NYrows ) ) != 0 ) 
    { rLog( "Error: Some rows meet requirements for both PJMstress and NYstress" ) }
   
  reducedAprime[PJMrows,"PortStressFlag"] = "PJMstress"
  reducedAprime[NYrows,"PortStressFlag"] = "NYstress"
  
  NArows = which( is.na( reducedAprime[["PortStressFlag"]] ) )
  reducedAprime[NArows,"PortStressFlag"] = "Otherstress"
  
  #add portfolio stress test flags for CER/CEG
  rLog( "Adding TotPortStressFlag ..." )
  reducedAprime$TotPortStressFlag = NA
  
  CERrows = which( substr( reducedAprime[["CER_GROUP_2"]], 1, 2 ) != "JV" &
    reducedAprime[["CER_GROUP_2"]] != "CONTROLLERS" & 
    reducedAprime[["CER_GROUP_2"]] != "DISC OPS CEP" &
    reducedAprime[["CER_GROUP_2"]] != "CEG VIEW" )
  CEGonlyrows = which ( reducedAprime[["CER_GROUP_2"]] == "CEG VIEW" )
  
  reducedAprime[CERrows,"TotPortStressFlag"] = "CERstress"
  reducedAprime[CEGonlyrows,"TotPortStressFlag"] = "CEGonlystress"
  
  NArows = which( is.na( reducedAprime[["TotPortStressFlag"]] ) )
  reducedAprime[NArows,"TotPortStressFlag"] = "NOstress"
  #remove no stress stuff from PJM, NYPP, Other split as well
  reducedAprime[NArows,"PortStressFlag"] = "NOstress" 
  
  #output reduced Aprime
  rLog("Writing reduced Aprime to file ...")
  outputFile = paste( "//ceg/cershare/Risk/CollateralAllocation/",
    format(Date, "%Y%m%d"), "/SourceData/Reduced Aprime ",
    toupper(format(Date, "%d%b%y")), ".csv", sep="" )
  write.csv(reducedAprime, outputFile, row.names = FALSE)

}