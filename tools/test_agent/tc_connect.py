########################################################################################
# Connect test case suite
########################################################################################

import agent
import ethernet
import wifi
import accesspoint
import host


STA = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0x00)
STA1 = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0x01)
AP1 = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0xBC)
AP2 = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0xBD)

class TcConnect:
    """
    Connection test cases class.
    """
    def __init__(self, a):
        """
        Initialize the connection tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        
        # save the agent reference
        self.a = a
        
        self.a.tc_h1("CONNECT TESTS")
    
        # create the host instance
        self.host = host.Host(a, STA)
        # create the access point instances
        self.ap1 = accesspoint.AccessPoint(a, AP1, channel=5, period=100, ssid="louis")
        self.ap2 = accesspoint.AccessPoint(a, AP2, channel=11, period=100, ssid="louis")

        # reset the host (resetting the MIBs)
        self.host.reset(True)

        # set the host MAC address
        self.host.dbg_macaddr()
    
    def run_connect(self):
        """
        Perform the connection tests
        """
        self.a.tc_h2("Connection tests")
    
        self.a.tc_h3("Connection without any AP in scan result")
        
        # try to connect
        self.connect_noap()
    
        self.a.tc_h3("Connection with AP")

        # start the AP (start sending beacons)
        self.ap1.start()
        
        self.connect_ap()
    
        # make sure that the MIB element contains the AP name
        ssid = self.a.nxapi_mibget_req("mib_dot11StationConfigTable.dot11DesiredSSID")
        assert(ssid[0] == '\x05')
        assert(ssid[1:6] == "louis")
        
    
        # test TX
        for i in range(30):
            # generate a random frame
            msdu = self.host.tx_msdu(da=STA1, length=((64+i) % 1500), prio=1)
    
            # wait for the data sent over the air
            self.ap1.rx_msdu(msdu, "connect TX(%d)"%(i, ))

        # test RX
        for i in range(30):
            # transmit an MSDU
            mpdus = self.ap1.tx_msdu(da=self.host.macaddr, sa=STA1, length=64)

            # wait for the MSDU on host
            self.host.rx_msdu(mpdus, "RX push(%d)"%(i,))

        self.a.tc_h3("Deauthentication initiated by user")
        self.deauthentication_from_user()
        
        self.a.tc_h3("Deauthentication initiated by AP")
        self.connect_ap()
        self.deauthentication_from_ap()
        
        self.a.tc_h3("Disassociation initiated by AP")
        self.connect_ap()
        self.disassociation_from_ap()
    
        self.a.tc_h3("Synchronization loss")
        self.connect_ap()

        # wait for a second
        self.a.wait(1000000000)
        
        # stop the AP
        self.ap1.stop()
        
        # test the synchronization loss
        self.syncloss()

    def run_roaming(self):
        # start the two APs (start sending beacons)
        self.ap1.start()
        self.ap2.start()

        self.a.tc_h3("Roaming between two APs")
        self.connect_ap()

        # stop the AP
        self.ap1.stop()

        self.roam()

        # stop the AP
        self.ap2.stop()


    def connect_noap(self):
        """
        Perform a connection test without any AP in scan results (should fail).
        """
        
        # try to connect to AP which has not yet started
        self.host.sta_connect(self.ap1.macaddr)
        
        # wait immediately for the confirmation which should fail
        assert(self.host.sta_connect_confirm() == False)

    def connect_ap(self):
        """
        Perform a scan and a connection test.
        """

        # scan passively now that the AP has started
        self.host.scan_passive(200)
        
        # connect to AP
        self.host.sta_connect(self.ap1.macaddr)

        # expect a management frame for the AP
        mpdu = self.ap1.rx_mpdu(wifi.AIR_MGMT)
        
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_probereq)
        assert(wifi.find_ie(mpdu.typesubtype, mpdu.bd, wifi.ie_ssid) == "louis")
        
        # send a probe response (optional if the join timeout is long enough)
        
        # expect an authentication request
        mpdu = self.ap1.rx_mpdu(wifi.AIR_MGMT)
        
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_authentication)
    
        # send an authenticate response
        self.ap1.tx_msdu(da=self.host.macaddr, body=wifi.authenticate(), 
                        fctl=wifi.fctl_authentication)

        # expect an association request
        mpdu = self.ap1.rx_mpdu(wifi.AIR_MGMT)
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_associationreq)
        
        # send an association response
        self.ap1.tx_msdu(da=self.host.macaddr, body=wifi.associationresponse(), 
                        fctl=wifi.fctl_associationresp)
    
        # expect a STA connection confirmation with a correct status
        assert(self.host.sta_connect_confirm() == True)
    
    def deauthentication_from_user(self):
        """
        Send a user disassociation request.
        """
        # disassociate
        self.a.nxapi_disassociate_req(self.ap1.macaddr)
        
        # expect a deauth frame
        mpdu = self.ap1.rx_mpdu(wifi.AIR_MGMT)
        
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_deauthentication)
        
        # expect a disassociation confirmation with a correct status
        assert(self.a.nxapi_disassociate_cfm() == True)
    
        # generate a random frame
        msdu = self.host.tx_msdu(da=self.ap1.macaddr, length=1000, prio=1)
    
        # wait for data send confirmation (not in the air)
        self.a.host_send_data_cfm(msdu)

    def disassociation_from_ap(self):
        """
        Perform a disassociation request from AP test.
        """
        # send disassociation
        self.ap1.tx_msdu(da=self.host.macaddr, body=wifi.disassociation(), 
                        fctl=wifi.fctl_disassociation)
    
        # expect a deauth frame
        mpdu = self.ap1.rx_mpdu(wifi.AIR_MGMT)
        
        # sanity checks
        assert(mpdu.typesubtype == wifi.fctl_deauthentication)
    
        # expect a disassociation indication with a correct status
        assert(self.a.nxapi_disassociate_ind() == True)
    
        # generate a random frame
        msdu = self.host.tx_msdu(da=self.ap1.macaddr, length=1000, prio=1)
    
        # wait for data send confirmation (not in the air)
        self.a.host_send_data_cfm(msdu)

    def deauthentication_from_ap(self):
        """
        Perform a deauthentication request from AP test.
        """
        # send deauthentication
        self.ap1.tx_msdu(da=self.host.macaddr, body=wifi.deauthenticate(), 
                        fctl=wifi.fctl_deauthentication)

        # expect a disassociation indication with a correct status
        assert(self.a.nxapi_disassociate_ind() == True)
    
        # generate a random frame
        msdu = self.host.tx_msdu(da=self.ap1.macaddr, length=1000, prio=1)
    
        # wait for data send confirmation (not in the air)
        self.a.host_send_data_cfm(msdu)
    
    def syncloss(self):
        """
        Perform a synchronization lost test.
        """
        # expect a disassociation indication with a correct status
        assert(self.a.nxapi_disassociate_ind() == True)
    
        # generate a random frame
        msdu = self.host.tx_msdu(da=self.ap1.macaddr, length=1000, prio=1)
    
        # wait for data send confirmation (not in the air)
        self.a.host_send_data_cfm(msdu)

    def roam(self):
        """
        Perform a roaming test.
        """

        # make sure that the MIB element contains the AP1 BSSID
        bssid = self.a.nxapi_mibget_req("mib_dot11StationConfigTable.dot11BSSID")
        assert(len(bssid) == 6)
        assert(bssid == self.ap1.macaddr)

        # wait for 4 seconds
        self.a.wait(4000000000)

        # make sure that the MIB element contains the AP2 BSSID
        bssid = self.a.nxapi_mibget_req("mib_dot11StationConfigTable.dot11BSSID")
        assert(len(bssid) == 6)
        assert(bssid == self.ap2.macaddr)

