# Document breaks in our code from 2.5.0 to 2.6.0
#
#
#
#

# memory.limit() in 2.6.0 returns size in MB, in 2.5.0 it was in Bytes.
# Code that calculates size of packets will break.
# See H:\user\R\RMG\Models\Price\ForwardCurve\Network/utils.R
#     function: get.no.packets
# There is at least another one in VaR/Base/
