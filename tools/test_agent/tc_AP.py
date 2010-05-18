########################################################################################
# AP test cases suite
########################################################################################

import tc_mib
import tc_scan
import tc_connect
import tc_rx
import tc_tx
import tc_traffic

def run(a):
    # MIB testcases
    tc = tc_mib.TcMib(a)


