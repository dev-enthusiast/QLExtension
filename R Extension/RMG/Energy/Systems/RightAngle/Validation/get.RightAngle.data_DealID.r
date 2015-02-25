# Pull RightAngle potisions from VCentral.
# Expect Inputs:
#         AsOfDate,
#         DealID
#
# Written by Wang Yu on 28-Mar-2007


get.RightAngle.data_DealID <- function(options)
{

  if (length(options$asOfDate)==0) {options$asOfDate <- Sys.Date()-1}

  query <- paste("SELECT th.Valuation_Date, th.Deal_type, th.Parent_Deal_ID,th.ExternalDealID, ",
                        "th.SOR_Trade_ID, th.Source_Name, td.Trade_Detail_ID, th.Commodity, ",
                        "td.Product, td.Region, td.Location_NM, td.Delivery_Point, td.Transaction_Type, ",
                        "td.Service_Type, td.Buy_Sell_Type, td.Call_Put_Type, th.Internal_Company, ",
                        "th.CounterParty, th.Profit_Center_1, th.Profit_Center_2, th.Trader, ",
                        "td.Last_Modification_User, td.Last_Modification_Date, td.TimeZone, ",
                        "td.Valuation_Type, td.Frequency_Indicator, td.Detail_Side, td.Phy_Fin_Flag, ",
                        "th.Trade_Entry_Date, th.Trade_Start_Date, th.Trade_End_Date, td.Valuation_Month, ",
                        "td.Period_Start_Date, td.Period_End_Date, td.Price_Type, td.Price, td.Price_USD, ",
                        "td.Price_UOM, td.Quantity, td.Qty_UOM, td.Quantity_Bucket_Type, td.Total_Original_Quantity, ",
                        "td.Currency, td.MTM_Value_Native, td.Undisc_MTM_Value_Native, td.FX_Rate, td.MTM_Value_USD, ",
                        "td.Undisc_MTM_Value_USD, td.PNL_Value_USD, td.Discount_End_DT, td.Payment_Date, ",
                        "td.Accounting_Period, td.Interest_Rate, td.Delta, td.Undisc_Delta, td.Skew_Delta, ",
                        "td.Gamma, td.Option_Flag, td.Strike, td.Expiration_Date, td.Premium_Flag, td.Vega, ",
                        "td.Theta, td.Rho, td.Correlation, td.Volatility, td.Vol_Frequency ",
                 "FROM vcentral.Trade_Detail td JOIN vcentral.Trade_Header th on td.Trade_Header_ID=th.Trade_Header_ID ",
                 "WHERE th.valuation_date = to_date('", format(options$asOfDate, "%m/%d/%Y"), "' ,'mm/dd/yyyy') ",
                 "  AND td.valuation_date = to_date('", format(options$asOfDate, "%m/%d/%Y"), "' ,'mm/dd/yyyy') ",
                 "  AND th.Parent_Deal_ID = '", as.character(options$DealID), "' ",
                 "  AND th.source_name='SRA' ",
                 "  AND th.source_eid=12 ",
                 "  AND th.sor_indicator=th.source_eid ",
                 "  AND td.source_feed='R' ",
                 "  AND (td.valuation_type='E' OR td.valuation_type IS NULL) ",
                 "Order by td.Valuation_Month ", sep="")
# FileDSN provides more robust performance and independency of users' setup
  require(RODBC)
  RightAngleConString = paste("FileDSN=H:/user/R/RMG/Utilities/DSN/VCTRLP.dsn",";UID=vcentral_read;PWD=vcentral_read;",sep="")
  RightAngleCon <- odbcDriverConnect(RightAngleConString)
  data <- sqlQuery(RightAngleCon, query)
  odbcCloseAll()

  save(date,file=paste("C:/temp/",as.character(options$DealID),"_",
                                  format(options$asOfDate, "%m_%d_%Y"),".RDate",sep=""))
  data$PHY_FIN_FLAG <- substr(as.character(data$PHY_FIN_FLAG),1,1)
  return(data)

}
