#include <windows.h>  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <float.h>

#include "xmim_api.h"
/* #include "R_FPU.h" */

/**
 * R DLL to LIM
 */

static char *build_version_tag = "xmimR.dll 04/26/2006";

/* standard DLL start-up function */

extern int UnlockData(int);

int  LibMain (HANDLE hInstance, WORD wDataSeg,
                        WORD wHeapSize, LPSTR lpszCmdLine)
{
  return (1);
}

/* traditional DLL exit function */

int 
WEP (int nParam)
{
  return (1);
}

/*** QUERY INTERFACE ****/

#define EXIT \
        XmimPrintError(NULL); XmimDisconnect(client_handle); return 1

static char* prog = NULL;

static struct request_data {
  int query_ok;
  int num_records;
  int current_col;
  int num_rows;
  int num_cols;
  double *values;
  XmimDateTime *dates;
  char **labels;
} request = { 0, 0, 0, 0, 0, 0, 0, 0 };

/****************************************************************/

static void
PrintQueryReport(XmimClientHandle		             client_handle,
		             XmimQueryReportHandle		       report_handle,
		             FILE	                    			 *stream);

static void
PrintQueryReportBlock(XmimClientHandle			       client_handle,
		                  XmimQueryReportBlockHandle	 reportBlockHandle,
		                  FILE				                 *stream);

static void
PrintSummary(XmimClientHandle			                client_handle,
	           XmimQueryReportBlockHandle		        reportBlockHandle,
	           FILE				                          *stream);

static void
PrintQueryReportBlock(XmimClientHandle		client_handle,
		      XmimQueryReportBlockHandle	report_block_handle,
		      FILE				*stream);

static void
PrintSummary(XmimClientHandle			client_handle,
	     XmimQueryReportBlockHandle		report_block_handle,
	     FILE				*stream);

static void
PrintPLReport(XmimClientHandle			client_handle,
	      XmimQueryPLReportHandle		pl_report_handle,
	      FILE				*stream);

static void
PrintPLStatRecord(XmimClientHandle		client_handle,
		  XmimQueryPLStatRecord		*stat_record,
		  FILE				*stream);

static void
PrintTradeRecord(XmimClientHandle		client_handle,
		 XmimQueryPLTradeRecord		*trade_record,
		 FILE				*stream);

static void
PrintPLEquityRecord(XmimClientHandle	 	client_handle,
		    XmimQueryPLEquityRecord 	*equity_record,
		    FILE			*stream);

/****************************************************************/

static void PrintQueryReport(XmimClientHandle		client_handle,
		      XmimQueryReportHandle	report_handle,
		      FILE			*stream)
{
  XmimString			title;
  int				num_report_blocks, i;
  XmimQueryReportHandle	final_report_handle;
  XmimQueryPLReportHandle	plReportHandle;
    
  if (XmimQueryGetReportTitle(client_handle, report_handle,
			      &title) == XMIM_SUCCESS) {
    fprintf(stream, "\n%s\n", title);
  }

  if (XmimQueryGetNumReportBlocks(client_handle, report_handle,
				  &num_report_blocks) == XMIM_SUCCESS) {
    for (i = 0; i < num_report_blocks; i++) {
      XmimQueryReportBlockHandle	report_block_handle;
      if (XmimQueryGetReportBlock(client_handle, report_handle, i,
				  &report_block_handle) == XMIM_SUCCESS)
	PrintQueryReportBlock(client_handle,
			      report_block_handle,
			      stream);
    }
  }

  if (XmimQueryGetPLReport(client_handle, report_handle, &plReportHandle)
      == XMIM_SUCCESS)
    PrintPLReport(client_handle, plReportHandle, stream);

  if (XmimQueryGetFinalReport(client_handle, report_handle,
			      &final_report_handle) == XMIM_SUCCESS)
    PrintQueryReport(client_handle, final_report_handle, stream);
}

/****************************************************************/

static void PrintQueryReportBlock(XmimClientHandle		client_handle,
			   XmimQueryReportBlockHandle	report_block_handle,
			   FILE				*stream)
{
  XmimBoolean		time_p;
  int			i, j;
  int			num_rows;
  char		       *col_heading;
  int			num_cols;

  fprintf(stream, "   Date   ");

  XmimQueryHasTimes(client_handle, report_block_handle, &time_p);

  if(time_p)
    fprintf(stream, "   Time   ");
	
  XmimQueryGetNumColumns(client_handle, report_block_handle, &num_cols);
	
  for (i = 0; i < num_cols; i++) {
    XmimQueryGetColumnHeading(client_handle,
			      report_block_handle,
			      i,
			      &col_heading);
    fprintf(stream, "    %s   ", col_heading);
  }

  fprintf(stream, "\n");

  XmimQueryGetNumRows(client_handle, report_block_handle, &num_rows);

  for (i = 0; i < num_rows; i++) {
    int		num_cols;
    XmimDate 	date;
    XmimTime	time;

    XmimQueryGetRowDate(client_handle,
			report_block_handle,
			i, &date);
    fprintf(stream, "%s  ", XmimStringDate(date));

    if (time_p) {
      XmimQueryGetRowTime(client_handle,
			  report_block_handle,
			  i, &time);
      fprintf(stream, "%s  ", XmimStringTime(time, True));
    }
	
    XmimQueryGetNumColumns(client_handle,
			   report_block_handle,
			   &num_cols);

    for (j = 0; j < num_cols; j++) {
      double val;
      XmimQueryGetValue(client_handle, report_block_handle, i, j, &val);
      fprintf(stream, "%10.2f  ", val);
    }
    fprintf(stream, "\n");
  }

  fprintf(stream, "\n");

  PrintSummary(client_handle, report_block_handle, stream);
}

/****************************************************************/

void PrintSummary(XmimClientHandle			client_handle,
		  XmimQueryReportBlockHandle		report_block_handle,
		  FILE					*stream)
{
  int 			i, num_summary_blocks;
  XmimQuerySummaryRecord 	*summary_block_ptrs;
    
  if (XmimQueryGetNumSummaryRecords(client_handle,
				    report_block_handle,
				    &num_summary_blocks) == XMIM_SUCCESS &&
      num_summary_blocks > 0) {

    summary_block_ptrs = (XmimQuerySummaryRecord*)
      malloc(num_summary_blocks * sizeof(XmimQuerySummaryRecord));
	
    for (i = 0; i < num_summary_blocks; i++)
      XmimQueryGetSummaryRecord(client_handle, report_block_handle,
				i, &summary_block_ptrs[i]);

    fprintf(stream, "%s", "Sum        ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].sum);
    fprintf(stream, "\n");
	
    fprintf(stream, "%s", "Average    ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].average);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "AveragePos ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].averagePositive);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "AverageNeg ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].averageNegative);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "PercentPos ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].percentPositive);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "PercentNeg ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].percentNegative);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "Highest    ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].highest);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "Lowest     ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].lowest);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "Stddev     ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].standardDeviation);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "Zstat      ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].zstat);
    fprintf(stream, "\n");

    fprintf(stream, "%s", "Variance   ");
    for (i= 0; i < num_summary_blocks; i++)
      fprintf(stream, "%10.2f   ",
	      summary_block_ptrs[i].variance);
    fprintf(stream, "\n");

    free(summary_block_ptrs);
  }
}

/****************************************************************/

void PrintPLReport(XmimClientHandle		client_handle,
		   XmimQueryPLReportHandle	pl_report_handle,
		   FILE				*stream)
{
  int				num_trades, num_equities, i;
  XmimQueryPLTradeRecord	trade_record;
  XmimQueryPLStatRecord	stat_record;
  XmimQueryPLEquityRecord	equity_record;
    
  fprintf(stream, "P&L Summary Statistics\n");

  fprintf(stream, "Statistic");

  fprintf(stream, "\t\t%s\t\t%s\t\t%s\n", "Combined", "Long", "Short");

  for (i = (int)XMIM_PL_NUM_PERIODS; i < (int)XMIM_PL_SLIPPAGE; i++) {
    if (XmimQueryGetPLStatRecord(client_handle, pl_report_handle,
				 (XmimQueryPLStatItem)i, &stat_record)
	== XMIM_SUCCESS)
      PrintPLStatRecord(client_handle, &stat_record, stream);
  }

  XmimQueryGetPLNumTrades(client_handle, pl_report_handle, &num_trades);

  if (num_trades > 0) {
    fprintf(stream, "\n\nP&L Trades\n");
	
    fprintf(stream, "Tr Type\tInDate");
    fprintf(stream, "\tInTime");
    fprintf(stream, "\tInReason");
    fprintf(stream, "\tOutDate");
    fprintf(stream, "\tOutTime");
    fprintf(stream, "\tOutReason");
    fprintf(stream,
	    "\tNum\tInPrice\tOutPrice\tMove\t%%Move\t"
	    "Profit\tCumProfit\n");
    for (i = 0; i < num_trades; i++) {
      if (XmimQueryGetPLTradeRecord(client_handle, pl_report_handle, i,
				    &trade_record) == XMIM_SUCCESS)
	PrintTradeRecord(client_handle, &trade_record, stream);
    }	
  }
    
  XmimQueryGetPLNumEquityRecords(client_handle, pl_report_handle,
				 &num_equities);

  if (num_equities > 0) {
    fprintf(stream, "P&L Equity Report\n\n");
    fprintf(stream, "      Date     Combined         Open       Closed");
    for (i = 0; i < num_equities; i++) {
      if (XmimQueryGetPLEquityRecord(client_handle, pl_report_handle,
				     i, &equity_record)
	  == XMIM_SUCCESS)
	PrintPLEquityRecord(client_handle, &equity_record, stream);
    }
  }
  fprintf(stream, "\n");
}

/****************************************************************/

void PrintPLStatRecord(XmimClientHandle		client_handle,
		       XmimQueryPLStatRecord	*stat_record,
		       FILE			*stream)
{
  if (stat_record->label[0] != '\0') {
    fprintf(stream, "\n%s: ", stat_record->label);
	
    if (XmimTestNaN(client_handle, (float)stat_record->combinedValue))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%10.2f\t", stat_record->combinedValue);

    if (!XmimValidDate(stat_record->combinedDate))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%s\t", XmimStringDate(stat_record->combinedDate));
	    
    if (!XmimValidTime(stat_record->combinedTime))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%s\t",
	      XmimStringTime(stat_record->combinedTime, True));
	    
    if (XmimTestNaN(client_handle, (float)stat_record->longValue))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%10.2f\t", stat_record->longValue);

    if (!XmimValidDate(stat_record->longDate))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%s\t", XmimStringDate(stat_record->longDate));
	    
    if (!XmimValidTime(stat_record->longTime))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%s\t",
	      XmimStringTime(stat_record->longTime, True));
	    
    if (XmimTestNaN(client_handle, (float)stat_record->shortValue))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%10.2f\t", stat_record->shortValue);
	
    if (!XmimValidDate(stat_record->shortDate))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%s\t", XmimStringDate(stat_record->shortDate));
	    
    if (!XmimValidTime(stat_record->shortTime))
      fprintf(stream, "\t");
    else
      fprintf(stream, "%s\t",
	      XmimStringTime(stat_record->shortTime, True));
  }
}

/****************************************************************/

void PrintTradeRecord(XmimClientHandle		client_handle,
		      XmimQueryPLTradeRecord	*trade_record,
		      FILE			*stream)
{
  fprintf(stream, "\n");

  switch (trade_record->type) {
  case XMIM_PL_TRADE_BUY:
    fprintf(stream, "Buy\t");
    break;
  case XMIM_PL_TRADE_SELL:
    fprintf(stream, "Sell\t");
    break;
  case XMIM_PL_TRADE_LONG:
    fprintf(stream, "Lg\t");
    break;
  case XMIM_PL_TRADE_SHORT:
    fprintf(stream, "Sh\t");
    break;
  }
    
  if (XmimValidDate(trade_record->entryDate))
    fprintf(stream, "%s\t", XmimStringDate(trade_record->entryDate));
  else
    fprintf(stream, "\t");

  if (XmimValidTime(trade_record->entryTime))
    fprintf(stream, "%s\t", XmimStringTime(trade_record->entryTime, True));
  else
    fprintf(stream, "\t");

  if (trade_record->entryReasonLabel[0] != '\0')
    fprintf(stream, "%s\t", trade_record->entryReasonLabel);
  else
    fprintf(stream, "\t");

  if (XmimValidDate(trade_record->exitDate))
    fprintf(stream, "%s\t", XmimStringDate(trade_record->exitDate));
  else
    fprintf(stream, "\t");

  if (XmimValidTime(trade_record->exitTime))
    fprintf(stream, "%s\t", XmimStringTime(trade_record->exitTime, True));
  else
    fprintf(stream, "\t");

  if (trade_record->exitReasonLabel[0] != '\0')
    fprintf(stream, "%s\t", trade_record->exitReasonLabel);
  else
    fprintf(stream, "\t");

  fprintf(stream, "%d\t", (int)trade_record->numContracts);

  if (XmimTestNaN(client_handle, (float)trade_record->entryPrice))
    fprintf(stream, "\t");
  else
    fprintf(stream, "%10.2f\t\t", trade_record->entryPrice);

  if (XmimTestNaN(client_handle, (float)trade_record->exitPrice))
    fprintf(stream, "\t");
  else
    fprintf(stream, "%10.2f\t\t", trade_record->exitPrice);

  if (XmimTestNaN(client_handle, (float)trade_record->move))
    fprintf(stream, "\t");
  else
    fprintf(stream, "%10.2f\t\t", trade_record->move);

  if (XmimTestNaN(client_handle, (float)trade_record->percentMove))
    fprintf(stream, "\t");
  else
    fprintf(stream, "%10.2f\t\t", trade_record->percentMove);

  if (XmimTestNaN(client_handle, (float)trade_record->profit))
    fprintf(stream, "\t");
  else
    fprintf(stream, "%10.2f\t\t", trade_record->profit);

  if (XmimTestNaN(client_handle, (float)trade_record->cumProfit))
    fprintf(stream, "\t");
  else
    fprintf(stream, "%10.2f\t\t", trade_record->cumProfit);
}

/****************************************************************/

void PrintPLEquityRecord(XmimClientHandle	 client_handle,
			 XmimQueryPLEquityRecord *equity_record,
			 FILE			 *stream)
{
  fprintf(stream, "\n%s", XmimStringDate(equity_record->date));

  if (XmimValidTime(equity_record->time))
    fprintf(stream, "\t%s", XmimStringTime(equity_record->time, True));

  if (XmimTestNaN(client_handle, (float)equity_record->combined))
    fprintf(stream, "\t%10.2f", equity_record->combined);
  else
    fprintf(stream, "\t");

  if (XmimTestNaN(client_handle, (float)equity_record->open))
    fprintf(stream, "\t%10.2f", equity_record->open);
  else
    fprintf(stream, "\t");

  if (XmimTestNaN(client_handle, (float)equity_record->close))
    fprintf(stream, "\t%10.2f", equity_record->close);
  else
    fprintf(stream, "\t");

  fprintf(stream, "\n");
}

/****************************************************************/

static void get_data_vals(XmimClientHandle client_handle,
			  XmimQueryResultHandle	query_handle,
			  struct request_data *request)
{
  int		num_report_blocks, num_reports, i, j, k, nc;
  XmimDate 	date;
  XmimTime      time;
  /* _WangYU_ int           time_p = 0; */
	XmimBoolean   time_p = (char) 0 ;
  XmimQueryReportHandle	report_handle;
  XmimQueryReportBlockHandle report_block_handle;
  double val;

  if (XmimQueryGetNumReports(client_handle, query_handle, &num_reports)
      == XMIM_SUCCESS) {
    /* ONLY HANDLE 1 BLOCK NOW */
    num_reports = 1;
    for (i = 0; i < num_reports; i++) {
      if (XmimQueryGetReport(client_handle, query_handle,
			     i, &report_handle) == XMIM_SUCCESS)

	if (XmimQueryGetNumReportBlocks(client_handle, report_handle,
					&num_report_blocks) == XMIM_SUCCESS) {
	  /* ONLY HANDLE 1 REPORT BLOCK */
	  num_report_blocks = 1;
	  for (i = 0; i < num_report_blocks; i++) {
	    if (XmimQueryGetReportBlock(client_handle, report_handle, i,
					&report_block_handle) == XMIM_SUCCESS) {
	      XmimQueryHasTimes(client_handle, report_block_handle, &time_p);
	      XmimQueryGetNumRows(client_handle, 
				  report_block_handle, &(request->num_rows));
	      XmimQueryGetNumColumns(client_handle,
				     report_block_handle, 
				     &(request->num_cols));

	      /* Allocate a vector of points to store the labels. */
	      request->labels = (char **)calloc(request->num_cols, sizeof(char *));
	      if (request->labels != (char **)0) {
		char *ch;
		for (nc = 0; nc < request->num_cols; nc++) {
		  XmimQueryGetColumnHeading(client_handle,
					    report_block_handle,
					    nc,
					    &ch);
		  request->labels[nc] = strdup(ch);
		}
	      }

	      request->dates = (XmimDateTime *)calloc(request->num_rows, 
						      sizeof(XmimDateTime));

	      request->values = (double *)calloc(request->num_rows * 
						 request->num_cols,
						 sizeof(double));
	      
	      for (i = 0; i < request->num_rows; i++) {		
		XmimQueryGetRowDate(client_handle,
				    report_block_handle,
				    i, &date);

		if (time_p) {
		  XmimQueryGetRowTime(client_handle,
				      report_block_handle,
				      i, &time);
		  ((request->dates)[i]).hour = time.hour;
		  ((request->dates)[i]).minute = time.minute;
		  ((request->dates)[i]).second = time.second;		  
		}
		
		((request->dates)[i]).year = date.year;
		((request->dates)[i]).month = date.month;
		((request->dates)[i]).day = date.day;
	      }

	      /* Extract the data */
	      k = 0;
	      for (j = 0; j < request->num_cols; j++) {
		for (i = 0; i < request->num_rows; i++) {		
		  XmimQueryGetValue(client_handle, 
				    report_block_handle, i, j, &val);
		  request->values[k++] = val;
		}
	      }
	    }
	  }
	}
    }
  }
}

/****************************************************************/

XmimFillOption parseFillOption(char *query)
{
  // parse fill from m_pQueryText
  const char *p = strstr(query, "%exec.missingdatananfill:");
  if (p != NULL) {
	  p = strstr(query, "%exec.missingdatananfill: Fill_Forward");
	  if (p != NULL)
		return XMIM_FILL_FORWARD;
	  p = strstr(query, "%exec.missingdatananfill: Fill_Backward");
	  if (p != NULL)
		return XMIM_FILL_BACKWARD;
	  p = strstr(query, "%exec.missingdatananfill: Fill_Interpolate_Linear");
	  if (p != NULL)
		return XMIM_FILL_INTERP_LIN;
	  p = strstr(query, "%exec.missingdatananfill: Fill_Interpolate_Geometric");
	  if (p != NULL)
		return XMIM_FILL_INTERP_GEO;
	  p = strstr(query, "%exec.missingdatananfill: Fill_Interpolate_Logarithmic");
	  if (p != NULL)
		return XMIM_FILL_INTERP_LOG;
	  p = strstr(query, "%exec.missingdatananfill: Fill_Nearest");
	  if (p != NULL)
		return XMIM_FILL_NEAREST;
  }
  return XMIM_FILL_NAN;
}

/****************************************************************/

void  xmim_query_execute_reshape(char **query, 
					   char **host_arg, 
					   long *port,
					   long *retval, 
					   long *num_rows, 
					   long *num_cols,
					   long *num_exec_units, 
					   long *exec_units,
					   long *rtd_usage,
					   long *holiday_fill,
					   long *miss_data_fill)
{
  XmimClientHandle client_handle;
  XmimQueryResultHandle	query_handle;
  XmimFillOption fill;

  int svr_num = (int)(port ? *port : -1);
  char *host = (host_arg ? *host_arg: 0);
  
  *retval = -1;
  request.query_ok = FALSE;
  
  fill = parseFillOption(*query);

  /* check arguments */
  if (!(query && *query && **query && 
	host && *host && 
	(svr_num >= 0))) {
    return;
  }

  XmimSetClientType("SPLUS"); 

  /* connect to server */
  if (XmimConnect (host, svr_num, &client_handle) != XMIM_SUCCESS) {
    return;
  }
  
  if (XmimVaQueryExecute
      (XMIM_CLIENT_HANDLE, 		client_handle,
       XMIM_QUERY, 		*query,
       XMIM_QUERY_HAS_CLIENT_MACROS, 	FALSE,
       XMIM_UNITS, (num_exec_units ? *num_exec_units : 1),
                   (exec_units ? *exec_units : XMIM_DAILY),
      
       XMIM_HISTORIC_ONLY, (rtd_usage ? *rtd_usage : FALSE),
       //XMIM_FILL_OPTION,   (holiday_fill ? *holiday_fill : XMIM_FILL_NAN), 
       //                    (miss_data_fill ? *miss_data_fill : XMIM_FILL_NAN),
	   XMIM_FILL_OPTION, fill, fill, 

       XMIM_QUERY_RESULT_HANDLE,	&query_handle,
       XMIM_END_ARGS) == XMIM_SUCCESS) {
  } else {
    char buffer[1024];
    sprintf(buffer, "%s", XmimStringError());
    XmimDisconnect(client_handle);
    return;
  }

  get_data_vals(client_handle, query_handle, &request); 

  *num_rows = request.num_rows;
  *num_cols = request.num_cols;

  if (*num_rows > 0 && *num_cols > 0) {
	*retval = 0;
	request.query_ok = TRUE;
  }

  XmimQueryDeleteResult(client_handle, query_handle);
  XmimDisconnect(client_handle);
}

/****************************************************************/

/*  This function is phased out by a new function.
    The new function is named the same
    but with the capability to set frequency 
    Wang Yu

void  xmim_query_execute(char **query, 
				   char **host_arg, 
				   long *port,
				   long *retval, 
				   long *num_rows, 
				   long *num_cols)
{
  long num_exec_units = 1;
  long exec_units = XMIM_DAYS;
  long rtd_usage = FALSE;
  long holiday_fill = XMIM_FILL_NAN;
  long miss_data_fill = XMIM_FILL_NAN;

    xmim_query_execute_reshape(query, 
							   host_arg, 
							   port, 
							   retval,
							   num_rows, 
							   num_cols,
							   &num_exec_units, 
							   &exec_units,
							   &rtd_usage,
							   &holiday_fill, 
							   &miss_data_fill);
}

*/

/****************************************************************/

/**
 * xmim_query_execute
 * This one has units 
 */
void  xmim_query_execute(char **query, 
			 char **host_arg, 
			 long *port,
			 long *retval, 
				    long *num_rows, 
				    long *num_cols,
				    long *num_exec_units, 
				    long *exec_units)
{
  long rtd_usage = FALSE;
  long holiday_fill = XMIM_FILL_NAN;
  long miss_data_fill = XMIM_FILL_NAN;

	if (*exec_units < 1 && *exec_units > 8)
		*exec_units = XMIM_DAYS;

    xmim_query_execute_reshape(query, 
							   host_arg, 
							   port, 
							   retval,
							   num_rows, 
							   num_cols,
							   num_exec_units, 
							   exec_units,
							   &rtd_usage,
							   &holiday_fill, 
							   &miss_data_fill);
}

/****************************************************************/

/**
 * Returns a NxM matrix that can be used with rbind to add data to 
 * the data set.
 */
void  xmim_query_results(long *max, 
				    long *offset,
				    long *num_recs, 
				    long *year, 
				    long *month, 
				    long *day, 
				    long *hour, 
				    long *minute, 
				    long *second,
				    double *values,
				    long *num_rows,
				    long *num_cols,
				    char **labels)
{
  long ii, jj;
  long numRows = 0; 

  if (!request.query_ok) 
  {
		*num_recs = 0;
		return;
	}

	/* total data returned from query */
	*num_recs = request.num_rows;
	*num_cols = request.num_cols;

	if (*num_cols <= 0) 
		return;

	for (ii = 0; ii < request.num_cols; ii++)
	  strncpy(labels[ii], request.labels[ii], 255);

	/* Determine how many rows will fit into the R allocated buffer. */
	*num_rows = *max / *num_cols;

	/* only return the number of rows that fit in the buffer */
	if (*num_rows > *num_recs-*offset)
		*num_rows = *num_recs-*offset;
 
	for (ii = 0; ii < *num_rows; ii++) {
		*year++ = request.dates[ii+*offset].year;
		*month++ = request.dates[ii+*offset].month;
		*day++ = request.dates[ii+*offset].day;
		if (hour != (long*)0 ) { 
		    *hour++ = request.dates[ii+*offset].hour;
		    *minute++ = request.dates[ii+*offset].minute;
		    *second++ = request.dates[ii+*offset].second;
		}
		for (jj = 0; jj < request.num_cols; jj++) {
			*values = request.values[jj*request.num_rows + ii + *offset];
			values++;
		}
	}
}

/****************************************************************/

/*  This function is phased out by a new function
    The new function is named the same 
    but with the capabitlity to get Units back from Query
    Wang Yu 

void  xmim_query_results(long *max, 
				   long *offset,
				   long *num_recs, 
				   long *year, 
				   long *month, 
				   long *day, 
				   double *values,
				   long *num_rows,
				   long *num_cols)
{
	xmim_query_results2(max, offset, num_recs, year, month, day, 
			    (long*)0, (long*)0, (long*)0, values, 
			    num_rows, num_cols, NULL); 
} 

*/

/****************************************************************/

void  xmim_query_finish()
{
  int ii;

  if (request.values != NULL)
    free(request.values);
  if (request.dates != NULL)
    free(request.dates);
  if (request.labels != NULL) {
    for (ii = 0; ii < request.num_cols; ii++)
      if (request.labels[ii] != NULL)
	free(request.labels[ii]);
    free(request.labels);
  }
  memset(&request, 0, sizeof(request));
}

/****************************************************************/

#if 0
	int QueryDebug()
	{
	  char *query = "SHOW NaturalGas: NG WHEN Date is 2005";
	  char *host_arg = "limprod";
	  long port = 0;
	  long retval;
	  long num_recs;
	  long num_rows;
	  long num_cols;
	  
	  long num_exec_units = 1;
	  long exec_units = XMIM_DAYS;
  
	  long max = 2000;
	  long offset = 0;
	  long year[2000], month[2000], day[2000],hour[2000],minute[2000],second[2000];
	  double values[2000];
	  char lables[2000]	  
  
	  xmim_query_execute(&query, &host_arg, &port, &retval, &num_rows, &num_cols, &num_exec_units, &exec_units);
	  xmim_query_results(NULL, NULL, &num_recs, year, month, day, hour, minute,second, values, &num_rows, &num_cols, &lables);
	  xmim_query_finish(); 

	  return 0;
	}
#endif

/****************************************************************/

#if 0

void usage()
{
  fprintf (stderr, "Usage: %s %s\n", prog,
	   "[ -n num_units units ] [ -h ] [ -s host ] [ -m svr_num ]"
	   "[-f holiday miss-data] [-r rtd_usage] [-c] query");
}

static void
PrintQueryResult(XmimClientHandle 		           client_handle,
		             XmimQueryResultHandle 		       query_handle,
		             FILE				                     *stream)		             

{
  int		num_reports;
  int i;

  if (XmimQueryGetNumReports(client_handle, query_handle, &num_reports)
      == XMIM_SUCCESS) {
    for (i = 0; i < num_reports; i++) {
      XmimQueryReportHandle	report_handle;
      if (XmimQueryGetReport(client_handle, query_handle,
			     i, &report_handle) == XMIM_SUCCESS)
	PrintQueryReport(client_handle, report_handle, stream);
    }
  }
}

/****************************************************************/

int debug_main (int argc, char **argv)
{
  int             		i, k, idx = 0;
  char			*host = NULL;
  char			*query_file;
  char			*output = NULL;
  FILE			*stream;
  int				svr_num = 0;
  XmimUnits	        	units = XMIM_DAYS;
  int				multiple = 1;
  XmimFillOption		holiday_fill = XMIM_FILL_INVALID;
  XmimFillOption		miss_data_fill = XMIM_FILL_INVALID;
  int				rtd_usage = XMIM_APPEND_TO_NONE;
  int				clnt_macro_p = 0;
  extern			char *optarg;
  extern			int optind;
  char			c;
  XmimClientHandle		client_handle;
  XmimQueryResultHandle 	query_handle;

  prog = argv[0];
  while ((c = getopt(argc, argv, "hr:s:m:n:f:o:c")) != -1)
    switch (c) {
    case 'h':
      usage();
      return 1;
    case 'r':
      rtd_usage = atoi(optarg);
      break;
    case 's':
      host = optarg;
      break;
    case 'm':
      svr_num = atoi(optarg);
      break;
    case 'n':
      multiple = atoi(optarg);
      units = (XmimUnits)atoi(argv[optind]);
      optind++;
      break;
    case 'f':
      holiday_fill = (XmimFillOption)atoi(optarg);
      miss_data_fill = (XmimFillOption)atoi(argv[optind]);
      optind += 1;
      break;
    case 'o':
      output = optarg;
      break;
    case 'c':
      clnt_macro_p = 1;
      break;
    default:
      usage();
      return 1;
    }

  if (optind > argc - 1) {
    usage();
    return 1;
  }
  else {
    if (XmimConnect (host, svr_num, &client_handle) != XMIM_SUCCESS) {
      EXIT;
    }

    if (output != NULL) {
      stream = fopen(output, "w");
      if (stream == NULL) {
	perror("fopen");
	exit(1);
      }
    }
    else
      stream = stdout;
	
    while (optind <= argc - 1) {
      query_file = argv[optind];
      optind++;
	
      if (XmimVaQueryExecute
	  (XMIM_CLIENT_HANDLE, 		client_handle,
	   XMIM_QUERY_FILE_NAME, 		query_file,
	   XMIM_QUERY_HAS_CLIENT_MACROS, 	clnt_macro_p,
	   XMIM_UNITS,	 		multiple, units,
	   XMIM_HISTORIC_ONLY, 		rtd_usage,
	   XMIM_FILL_OPTION, 		holiday_fill, miss_data_fill,
	   XMIM_QUERY_RESULT_HANDLE,	&query_handle,
	   XMIM_END_ARGS) != XMIM_SUCCESS) {
	EXIT;
      }

      PrintQueryResult(client_handle, query_handle, stream);
      XmimQueryDeleteResult(client_handle, query_handle);
    }
  }
  XmimDisconnect(client_handle);
  return 0;
}
#endif
/****************************************************************/
