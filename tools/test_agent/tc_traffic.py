########################################################################################
# Traffic test cases suite
########################################################################################

import agent
import ethernet
import wifi
import host
import accesspoint

STA = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0x01)
STA2 = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0x02)
AP = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0xBC)

class TcTraffic:
    """
    Traffic condition test cases.
    """
    def __init__(self, a):
        """
        Initializes the traffic tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        
        # save the agent reference
        self.a = a
    
        self.a.tc_h1("TRAFFIC TESTS")
    
        # create the host instance
        self.host = host.Host(a, STA)
        # create the access point instance
        self.ap = accesspoint.AccessPoint(a, AP)
    
        # reset the host without resetting the MIBs
        self.host.reset(False)

        # set the host MAC address
        self.host.dbg_macaddr()

        # start the AP (start sending beacons)
        self.ap.start()

        # scan passively now that the AP has started
        self.host.scan_passive(200)
        
        # connect to AP
        self.host.sta_connect(self.ap.macaddr)

        # expect a management frame for the AP
        mpdu = self.ap.rx_mpdu(wifi.AIR_MGMT)
        
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_probereq)
        assert(wifi.find_ie(mpdu.typesubtype, mpdu.bd, wifi.ie_ssid) == "louis")
        
        # send a probe response (optional if the join timeout is long enough)
        
        # expect an authentication request
        mpdu = self.ap.rx_mpdu(wifi.AIR_MGMT)
        
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_authentication)
    
        # send an authenticate response
        self.ap.tx_msdu(da=self.host.macaddr, body=wifi.authenticate(), 
                        fctl=wifi.fctl_authentication)

        # expect an association request
        mpdu = self.ap.rx_mpdu(wifi.AIR_MGMT)
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_associationreq)
        
        # send an association response
        self.ap.tx_msdu(da=self.host.macaddr, body=wifi.associationresponse(), 
                        fctl=wifi.fctl_associationresp)
    
        # expect a STA connection confirmation with a correct status
        assert(self.host.sta_connect_confirm() == True)
    
    def run_bidi(self):
        """
        Perform bidirectional transfer tests.
        """
        self.a.tc_h2("Bidirectional tests")
        for i in xrange(10):
            # reinitialize the MSDUs lists
            tx_msdus=[]
            rx_msdus=[]
            # send data packets
            for j in xrange(i):
                # send MSDUs from host
                tx_msdus.append(self.host.tx_msdu(da=STA2, length=1300, prio=1))
            for j in xrange(i):
                # send MSDUs from AP
                rx_msdus.append(self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=100))
                
            # check the correct reception
            for tx_msdu in tx_msdus:
                # wait for the transmitted frame
                self.ap.rx_msdu(tx_msdu, "TX bidi")
    
            # check the correct reception
            for rx_msdu in rx_msdus:
                # wait for the received frame
                self.host.rx_msdu(rx_msdu, "RX bidi")
