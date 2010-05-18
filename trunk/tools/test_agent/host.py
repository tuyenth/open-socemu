########################################################################################
# Host class
########################################################################################

import agent
import compare
import ethernet

class Host:
    """
    Host class.
    """
    def __init__(self, a, macaddr):
        """
        This function initializes the TX tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        assert(isinstance(macaddr, str) and len(macaddr) == 6)
        
        # save the agent reference
        self.a = a
        
        # save the MAC address
        self.macaddr = macaddr
        
    def reset(self, resetmib=True):
        """
        Send a command to reset the host MAC SW stack.
        
        Keyword arguments:
        resetmib -- indicate if the MIB shall also be reset (default True)
        """
        
        # sanity check
        assert(isinstance(resetmib, bool))
        
        # wait for any potential on-going transfer to complete before sending RESET
        self.a.wait(1000000)
    
        # reset the SW stack
        if resetmib:
            self.a.nxapi_reset_req(1)
        else:
            self.a.nxapi_reset_req(0)

    def dbg_add(self, macaddr, mode):
        """
        Use debug command to open the data path for a specific STA.
        
        Mandatory arguments:
        macaddr -- MAC address of the STA to open the data path for
        mode -- operating mode of the DUT (0=IBSS STA(default), 1=ESS STA, 2=AP)
        """
        
        # sanity check
        assert(isinstance(macaddr, str) and len(macaddr) == 6)
        
        # send debug command to DUT agent
        self.a.nxapi_dbg_staadd_req(macaddr, mode)
        # this message is not acknowledged so we wait some
        # time to insure that it has been taken into account
        self.a.wait(20000000)

    def dbg_macaddr(self):
        """
        Use debug command to set the MAC address of the STA.
        """
        # set the MAC address of the STA
        self.a.nxapi_dbg_macaddr_set_req(self.macaddr)
        
    def scan_passive(self, scantime=200):
        """
        Send a request to passively scan all the channels to the host.  Waits for the
        confirmation of the scan.
        
        Keyword arguments:
        scantime -- time to wait for beacons on each channel in ms (default 200)
        """
        # sanity check
        assert(isinstance(scantime, int))
        
        # request the passive scan
        self.a.nxapi_scan_req(0, scantime, scantime)

        # wait for the confirmation
        self.a.nxapi_scan_cfm()

    def scan_active(self, minscantime=10, maxscantime=50, wait=True):
        """
        Send a request to actively scan all the channels to the host
        
        Keyword arguments:
        minscantime -- minimum time to wait for probe responses/beacons on each channel in 
                       ms (default 10)
        maxscantime -- maximum time to wait for probe responses/beacons on each channel in 
                       ms (default 50)
        wait -- indicate if it should wait for the confirmation of the scan (default True)
        """
        # sanity check
        assert(isinstance(minscantime, int))
        assert(isinstance(maxscantime, int))
        
        # request the active scan
        self.a.nxapi_scan_req(1, minscantime, maxscantime)

        if wait:
            # wait for the confirmation
            self.a.nxapi_scan_cfm()

    def scan_confirm(self):
        """
        Wait for the scan end confirmation.
        """
        # wait for the confirmation
        self.a.nxapi_scan_cfm()

    def scan_results(self):
        """
        Get the scan results.
        
        return -- the tuple (status, count, results)
        """
        return self.a.nxapi_scangetresult_req()

    def sta_connect(self, bssid):
        """
        Try to connect the host to a BSSID.
        
        Mandatory parameters:
        bssid -- BSSID to connect to
        """
        # sanity check
        assert(isinstance(bssid, str) and len(bssid) == 6)
        
        # start the connection process
        self.a.nxapi_sta_connect_req(bssid)
        
    def sta_connect_confirm(self):
        """
        Wait for the confirmation of the connection.
        
        return -- True if the connection was successful, False otherwise
        """
        # wait for connection confirmation
        return self.a.nxapi_sta_connect_cfm()


    def tx_msdu(self, da=None, sa=None, length=1000, prio=0, **kwargs):
        """
        Push an MSDU in the host.
        
        Keyword arguments:
        da -- destination address of the MSDU (default = own STA address)
        sa -- source address of the MSDU (default = own STA address)
        length -- length of the MSDU payload (default = 1000 bytes)
        prio -- user priority as in IEEE 802.1p (default = Best Effort)
        others -- passed to called functions for information

        return -- the MSDU object
        """
        if sa == None:
            sa = self.macaddr
        if da == None:
            da = self.macaddr

        # generate a MSDU frame
        msdu = ethernet.tx_msdu(da, sa, length, prio, **kwargs)
        
        # send a request to the DUT for data send
        self.a.host_send_data(msdu)

        return msdu

    def rx_msdu(self, mpdus, message):
        """
        Pop a received MSDU from the host and compare it with the sent MDPUs.
        
        Mandatory arguments:
        mpdus -- MPDU object list that was received from the air
        message -- the message to print in case of error
        """
        # wait for the received frame
        frame = self.a.host_receive_data()

        # check frames correspond
        compare.rx_mpdu2msdu(self.a, mpdus, frame, message)

        