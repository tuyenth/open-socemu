########################################################################################
# STA test cases suite
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

    # SCAN testcases
    tc = tc_scan.TcScan(a)
    tc.run_basics()
    tc.run_oneap()
    tc.run_basics()
    tc.run_twoap()
    tc.run_basics()
    tc.run_active()
    
    # CONNECT testcases
    tc = tc_connect.TcConnect(a)
    tc.run_connect()
#    tc.run_roaming()
    
    # RX testcases
    tc = tc_rx.TcRx(a)
    tc.run_basics()
    tc.run_frag_test()

    # TX testcases
    tc = tc_tx.TcTx(a)
    tc.run_basics()
    tc.run_multiq()

    a.end()
    
#    tc.run_longterm()

#    # traffic testcases
#    tc = tc_traffic.TcTraffic(a)
#    tc.run_bidi()

