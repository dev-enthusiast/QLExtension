#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <float.h>

#include "xmim_api.h"

/**
 * R DLL to LIM
 */

static char *build_version_tag = "YuXmimR_Minimal.dll 07/05/2006";

/* standard DLL start-up function */

extern int UnlockData(int);

/*** QUERY INTERFACE ****/

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

static void get_data_vals(XmimClientHandle client_handle,
			  XmimQueryResultHandle	query_handle,
			  struct request_data *request)
{
  int		num_report_blocks, num_reports, i, j, k, nc;
  XmimDate 	date;
  XmimTime  time;
	XmimBoolean   time_p = (char) 0 ;
  XmimQueryReportHandle	report_handle;
  XmimQueryReportBlockHandle report_block_handle;
  double val;

  if (XmimQueryGetNumReports(client_handle, query_handle, &num_reports) == XMIM_SUCCESS) 
      {
        /* ONLY HANDLE 1 BLOCK NOW */
        num_reports = 1;
        for (i = 0; i < num_reports; i++) 
        {
          if (XmimQueryGetReport(client_handle, query_handle, i, &report_handle) == XMIM_SUCCESS)

	       if (XmimQueryGetNumReportBlocks(client_handle, report_handle,&num_report_blocks) == XMIM_SUCCESS)
            {
	          /* ONLY HANDLE 1 REPORT BLOCK */
	          num_report_blocks = 1;
	          for (i = 0; i < num_report_blocks; i++)
             {
	             if (XmimQueryGetReportBlock(client_handle, report_handle, i, &report_block_handle) == XMIM_SUCCESS)
                   {
	                  XmimQueryHasTimes(client_handle, report_block_handle, &time_p);
	                  XmimQueryGetNumRows(client_handle, 
				            report_block_handle, &(request->num_rows));
	                  XmimQueryGetNumColumns(client_handle,report_block_handle,&(request->num_cols));

	                 /* Allocate a vector of points to store the labels. */
	                  request->labels = (char **)calloc(request->num_cols, sizeof(char *));
	                  if (request->labels != (char **)0)
                       {
		                    char *ch;
	                     	for (nc = 0; nc < request->num_cols; nc++) 
                            {
		                          XmimQueryGetColumnHeading(client_handle,report_block_handle,nc,&ch);
		                          request->labels[nc] = strdup(ch);
		                        }
	                     }

	                 request->dates = (XmimDateTime *)calloc(request->num_rows,sizeof(XmimDateTime));

	                 request->values = (double *)calloc(request->num_rows * request->num_cols, sizeof(double));
	      
	                 for (i = 0; i < request->num_rows; i++)
                    {		
		                  XmimQueryGetRowDate(client_handle,report_block_handle,i, &date);

		                  if (time_p)
                       {
		                       XmimQueryGetRowTime(client_handle, report_block_handle, i, &time);
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
	                 for (j = 0; j < request->num_cols; j++)
                    {
		                  for (i = 0; i < request->num_rows; i++) 
                      {		
		                    XmimQueryGetValue(client_handle,report_block_handle, i, j, &val);
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
  if (!(query && *query && **query && host && *host && (svr_num >= 0))) 
  {
    return;
  }

  XmimSetClientType("SPLUS"); 

  /* connect to server */
  if (XmimConnect (host, svr_num, &client_handle) != XMIM_SUCCESS)
  {
    return;
  }
  
  if (XmimVaQueryExecute
      (XMIM_CLIENT_HANDLE, client_handle,
       XMIM_QUERY, *query,
       XMIM_QUERY_HAS_CLIENT_MACROS, FALSE,
       XMIM_UNITS, (num_exec_units ? *num_exec_units : 1), (exec_units ? *exec_units : XMIM_DAILY),
       XMIM_HISTORIC_ONLY, (rtd_usage ? *rtd_usage : FALSE),
       //XMIM_FILL_OPTION,   (holiday_fill ? *holiday_fill : XMIM_FILL_NAN), 
       //                    (miss_data_fill ? *miss_data_fill : XMIM_FILL_NAN),
	     XMIM_FILL_OPTION, fill, fill, 
       XMIM_QUERY_RESULT_HANDLE,	&query_handle,
       XMIM_END_ARGS) == XMIM_SUCCESS)
  {
  } 
  else
  {
    char buffer[1024];
    sprintf(buffer, "%s", XmimStringError());
    XmimDisconnect(client_handle);
    return;
  }

  get_data_vals(client_handle, query_handle, &request); 

  *num_rows = request.num_rows;
  *num_cols = request.num_cols;

  if (*num_rows > 0 && *num_cols > 0) 
  {
	*retval = 0;
	request.query_ok = TRUE;
  }

  XmimQueryDeleteResult(client_handle, query_handle);
  XmimDisconnect(client_handle);
}

/****************************************************************/


/****************************************************************/

/**
 * xmim_query_execute
 * This one has units 
 */
int xmim_query_execute(char **query, 
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
  int Success = 0;

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
  Success = (int)(*retval);
  return Success;
}

/****************************************************************/

/**
 * Returns a NxM matrix that can be used with rbind to add data to 
 * the data set.
 */
int  xmim_query_results(long *max, 
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
		return 0;
	}

	/* total data returned from query */
	*num_recs = request.num_rows;
	*num_cols = request.num_cols;

	if (*num_cols <= 0) 
		return 0;

	for (ii = 0; ii < request.num_cols; ii++)
	  strncpy(labels[ii], request.labels[ii], 255);

	/* Determine how many rows will fit into the R allocated buffer. */
	*num_rows = *max / *num_cols;

	/* only return the number of rows that fit in the buffer */
	if (*num_rows > *num_recs-*offset)
		*num_rows = *num_recs-*offset;
 
	for (ii = 0; ii < *num_rows; ii++) 
  {
		*year++ = request.dates[ii+*offset].year;
		*month++ = request.dates[ii+*offset].month;
		*day++ = request.dates[ii+*offset].day;
		if (hour != (long*)0 ) 
    { 
		    *hour++ = request.dates[ii+*offset].hour;
		    *minute++ = request.dates[ii+*offset].minute;
		    *second++ = request.dates[ii+*offset].second;
		}
		for (jj = 0; jj < request.num_cols; jj++) 
    {
			*values = request.values[jj*request.num_rows + ii + *offset];
			values++;
		}
	}
	return 1;
}

/****************************************************************/

int  xmim_query_finish()
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
  
  return 1;
}

/****************************************************************/