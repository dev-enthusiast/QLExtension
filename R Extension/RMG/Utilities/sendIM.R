################################################################################
# sendIM.R
#
# Function to send instant messages to registered users. 
# The user key is a string supplied by the bot when a user first sends it
# an instant message.
#
# Example bot request:
#    jfs214 (3:12:26 PM): hello
#    ConstellationBot (3:12:36 PM): Your user key: 8F43C8282DF9-D8CA-FEC7-02014DE1C1E400B1
#
library(RCurl)


################################################################################
# A list of all known user keys, helpful for sending log messages.
#
# Add your user key here once you register with the bot so that you don't 
# have to store it all over your code later.
#
IMUsers = new.env( hash=TRUE )
IMUsers$JFS214 = "8F43C8282DF9-D8CA-FEC7-02014DE1C1E400B1"
IMUsers$E14600 = "AF90E877937F-0947-5AFB-BB1786CA7847DD1F"
IMUsers$ETHANHAIRECCG = "AFAC987830B4-B1C1-73E3-E567F5F24BDC0025"
IMUsers$adrianccg = "9EC888530B66-A65C-273B-30C64CA82ABA9222"


################################################################################
# Internal Constants
#
.address = 'https://www.imified.com/api/bot/'
.botkey = '233C9F50-B7C4-8A33-3C9ABCC858E41F8C'
.userpass = 'john.scillieri@constellation.com:risksystems'
.send_apimethod = 'send'


################################################################################
# Send a given user the supplied message. 
#
# Example: 
# 	sendIM( IMUsers$JFS214, "\nAnother test message! \nIt works!\n\n")
# 
# Per the IMified API:
#   botkey   	Your unique bot key.
#   apimethod 	This parameter should have a value of send.
#   userkey 	The userkey of the user you would like to send a message to.
#    	Note: 	You may pass a list of userkeys seperated by comma to 
#				send one message to multiple users.
#   msg 		The message you'd like to send. 
#
sendIM <-function( userkey, msg )
{
    msg = gsub( "\n", "<br>", msg, fixed=TRUE )
    
    formParameters = list(
            botkey = .botkey,
            apimethod = .send_apimethod,
            userkey = userkey,
            msg = msg )
    
    curlParams = curlOptions(
            userpwd = .userpass,
            httpauth = 1L, # "basic",
            ssl.verifypeer = FALSE)
    
    output = postForm( uri=.address, .params=formParameters, .opts=curlParams )

    message = strsplit( output, '"' )[[1]][4]
    
    return( message )
}
