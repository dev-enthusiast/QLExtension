# Code used for obtaining AS bids for all plants within ERCOT
#
#
# Written by Chelsea Yeager on 15-Dec-2011




#------------------------------------------------------------
# FUNCTIONS
#   get.all.anc(start_date='2011-01-01', end_date='2011-12-14', connnmkt=channel_nmktp, conntesla=channel_teslap, connzema=channel_zema_prod)
#   start_date='2011-01-01'; end_date='2011-11-30'; connnmkt=channel_nmktp; conntesla=channel_teslap; connzema=channel_zema_prod
#   rm(list=ls()) --> clears memory


get.all.anc = function(start_date, end_date, connnmkt, conntesla, connzema)
  {

  ## Modify dates
      start_dt_1d=as.Date(start_date)-1
      end_dt_1d=as.Date(end_date)+2
      if(end_date>=Sys.Date()) {rLog("Silly--you can't run this report current or future days.  Decrease your end date!"); returnrLog("You can't run this report current or future days.  Decrease your end date.")}
                    

  ## Pull AS Data from ZEMA
      strQry=paste("select OPR_date, opr_hour, resource_name, resource_type, regup_awarded, regup_mcpc, regdn_awarded, regdn_mcpc, rrs_awarded, rrs_mcpc, nspin_awarded, nspin_mcpc
            from  ze_data.ercot_nodal_60ddamgrd
            where opr_date>=to_date('",start_dt_1d,"', 'YYYY-MM-DD') and opr_date<=to_date('",end_dt_1d,"', 'YYYY-MM-DD')
            and (regup_awarded<>0 or regdn_awarded<>0 or rrs_awarded<>0 or nspin_awarded<>0)
            group by resource_name, OPR_date, opr_hour, resource_type, regup_awarded, regup_mcpc, regdn_awarded, regdn_mcpc, rrs_awarded, rrs_mcpc, nspin_awarded, nspin_mcpc",sep="")  
      arrAS = sqlQuery(connzema, strQry)    
      
  ## Check that any data was returned
      if(nrow(arrAS)>0) {rLog("Successfully loaded AS data from ZEMA")}

  ## Pull in the resource category by resource used in the reserve margin calculation and label resource type to be more  descriptive
      RESOURCE_CATEGORY = data.frame(read.table(file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/rt/CVS/resource name2.csv", header = TRUE, sep = ","))
      arrAS.Type = merge(arrAS, RESOURCE_CATEGORY)       
      
      RESOURCE_TYPE_DETAIL = data.frame(read.table(file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/rt/CVS/Resource_Type.csv", header = TRUE, sep = ","))
      arrAS.All = merge(arrAS.Type, RESOURCE_TYPE_DETAIL)
                                                                 
  ## Aggregate data to be an 1.average of the mcpc and 2.sum of the MWs awarded and merge the two tables
      ASawdSum = aggregate( arrAS.All[,c("REGUP_AWARDED","REGDN_AWARDED","RRS_AWARDED","NSPIN_AWARDED") ], by=list(arrAS.All$OPR_DATE, arrAS.All$OPR_HOUR, arrAS.All$RESOURCE_CATEGORY, arrAS.All$RESOURCE_TYPE_DETAIL), sum)
      colnames(ASawdSum )[1:8]=c("date","hour", "resource category", "resource type", "regup awd" , "regdn awd" , "rrs awd" , "ns awd" )
      
      ASmcpcMean = aggregate( arrAS.All[,c("REGUP_MCPC","REGDN_MCPC","RRS_MCPC","NSPIN_MCPC") ], by=list(arrAS.All$OPR_DATE, arrAS.All$OPR_HOUR, arrAS.All$RESOURCE_CATEGORY, arrAS.All$RESOURCE_TYPE_DETAIL), mean)
      colnames(ASmcpcMean)[1:8]=c("date","hour", "resource category", "resource type", "regup mcpc" , "regdn mcpc" , "rrs mcpc" , "ns mcpc" )
      
      AncAll = merge( ASawdSum , ASmcpcMean)
                                        
  ## Write output to csv file
      write.csv(AncAll, file="S:/All/Portfolio Management/ERCOT/R/R/RMG/Energy/PM/ERCOT/rt/Output/AncAll.csv")

      
  }

