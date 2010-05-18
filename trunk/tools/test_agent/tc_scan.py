########################################################################################
# Scan test cases suite
########################################################################################

import agent
import ethernet
import wifi
import host
import accesspoint

STA = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0xBC)
AP1 = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0x01)
AP2 = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0x02)

class TcScan:
    """
    Scan test cases class.
    """
    def __init__(self, a):
        """
        Initialize the Scan tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        
        # save the agent reference
        self.a = a

        self.a.tc_h1("SCAN TESTS")
    
        # create the host instance
        self.host = host.Host(a, STA)

        # create the access point instances
        self.ap1 = accesspoint.AccessPoint(a, AP1, channel=5, period=100, ssid="louis")
        self.ap2 = accesspoint.AccessPoint(a, AP2, channel=8, period=99, ssid="steven")

        # reset the host (resetting the MIBs)
        self.host.reset(True)

        # set the host MAC address
        self.host.dbg_macaddr()

    def run_basics(self):
        """
        Perform basic scanning tests.
        """
        self.a.tc_h2("Passive scanning with no results")
    
        # passive scan
        self.host.scan_passive(200)
    
        # retrieve the scan results
        (status, count, results) = self.a.nxapi_scangetresult_req()
        
        # check the results
        assert(status == 0)
        assert(count == 0)
    
    def run_oneap(self):
        """
        Performs passive scanning with 1 AP tests.
        """
        self.a.tc_h2("Passive scanning with 1 AP")
    
        # start AP
        self.ap1.start()
        
        # passive scan
        self.host.scan_passive(200)
    
        # retrieve the scan results
        (status, count, results) = self.host.scan_results()
        
        # check the results
        assert(status == 0)
        assert(count == 1)
    
        # stop the AP
        self.ap1.stop()

    def run_twoap(self):
        """
        Performs passive scanning with 2 AP tests.
        """
        self.a.tc_h2("Passive scanning with 2 AP")
    
        # start APs
        self.ap1.start()
        self.ap2.start()
        
        # passive scan
        self.host.scan_passive(200)
    
        # retrieve the scan results
        (status, count, results) = self.host.scan_results()
        
        # check the results
        assert(status == 0)
        assert(count == 2)
    
        # stop the APs
        self.ap1.stop()
        self.ap2.stop()

    def run_active(self):
        """
        Perform active scanning tests.
        """
        self.a.tc_h2("Active scanning with one AP")
    
        # dummy probe request just to check PHY TX START interrupts
        self.ap1.tx_msdu(da=self.host.macaddr, body=wifi.proberesponse(100, "toto"),
                         fctl=wifi.fctl_proberesp)
    
        # request the active scan
        self.host.scan_active(minscantime=5, maxscantime=10, wait=False)
        
        # expect a probe request on each and every channel
        for i in wifi.channels:
            # wait for a management frame
            mpdu = self.a.air_send_data(wifi.AIR_MGMT)
            
            # sanity checks
            assert(mpdu.channel == i)
            assert(mpdu.typesubtype == wifi.fctl_probereq)
            assert(wifi.find_ie(mpdu.typesubtype, mpdu.bd, wifi.ie_ssid) == "")
            
            # simulate probe response from AP
            if i == self.ap1.channel:
                self.ap1.tx_msdu(da=self.host.macaddr,
                                 body=wifi.proberesponse(100, self.ap1.ssid),
                                 fctl=wifi.fctl_proberesp)

        # wait for the confirmation
        self.host.scan_confirm()
    
        # retrieve the scan results
        (status, count, results) = self.host.scan_results()
    
        # check the results
        assert(status == 0)
        assert(count == 1)
    

