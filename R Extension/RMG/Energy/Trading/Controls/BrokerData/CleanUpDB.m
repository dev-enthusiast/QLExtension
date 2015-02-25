%$Log: CleanUpDB.m,v $
%Revision 1.1  2014/11/03 20:03:09  e050092
%Moving these files to R Repository from matlab repository.
%
%Revision 1.6  2014/10/30 18:34:22  e050092
%Convert input to uppercase.
%
%Revision 1.5  2014/10/30 13:50:16  e050092
%Try again.
%
%Revision 1.4  2014/10/30 13:49:25  e050092
%Added commodity level deletion
%
%Revision 1.3  2014/10/29 22:29:12  e050092
%Error codes.
%
%Revision 1.2  2014/10/29 22:21:38  e050092
%Intial Release.
%
%
function ret = CleanUpDB(commodity)

pDate = datestr(datenum(getenv('PROCMON_RUNDATE')));
qDate = strcat(pDate(1:2),upper(pDate(3:7)), pDate(10:11));

conn = oraopen('RMGDBP','RMGDBUSER', 'n0t3bo0k');
query_activity = strcat('DELETE FROM raw_liquidity r where r.import_date = ''', qDate,'''',' and r.commodity = ''',upper(commodity), '''' );

query_deals = strcat('DELETE FROM raw_liquidity_deal r where r.import_date = ''', qDate,'''',' and r.commodity = ''',upper(commodity), '''' );

if ~isoraconn(conn)
    ret = -1;
    return;
end

v_activity = oraexec(conn, query_activity);


if (v_activity.status ~= 0)
    disp(v_activity.msg);
    ret = -1;
    return;
end

disp('Preventing double writes to database rmgdbp.[raw_liquidity]');

v_deals = oraexec(conn, query_deals);



if (v_deals.status ~= 0)
    disp(v_deals.msg);
    ret = -1;
    return;
end

disp('Preventing double writes to database rmgdbp.[raw_liquidity_deals]');

oraclose(conn);


ret = 0;
return;