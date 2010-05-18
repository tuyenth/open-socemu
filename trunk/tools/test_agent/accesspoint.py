########################################################################################
# AccessPoint class
########################################################################################

import agent
import station
import compare
import ethernet
import payload
import llcsnap
import wifi
import wifi_rx


class AccessPoint(station.Station):
    """AccessPoint class"""
    
    def __init__(self, a, macaddr, channel=1, period=100, ssid="louis", qos=True):
        """
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        macaddr -- the MAC address of the station to create
        
        Keywords arguments:
        channel -- channel on which the AP transmits beacons (default 1)
        period -- period at which the beacons are sent in ms (default 100)
        ssid -- SSID of the AP (default 'louis')
        qos -- indicate if the QoS is supported by the AP (default True)
        """
        
        # sanity check
        assert(isinstance(channel, int))
        assert(isinstance(period, int))
        assert(isinstance(ssid, str))
        assert(isinstance(a, agent.Agent))
        assert(isinstance(macaddr, str) and len(macaddr) == 6)

        # initialize the basic STA parameters
        station.Station.__init__(self, a, macaddr)
        
        # save the AP parameters
        self.channel = channel
        self.period = period
        self.ssid = ssid
        
        # initialize the internal parameter
        self.started = False

    def __del__(self):
        """
        Destructor to guarantee that the allocation for AP in the reference platform is
        removed.
        """
        # check if the AP beacon transmission was started
        if self.started:
            self.stop()

    def tx_msdu(self, da=None, sa=None, body=None, length=1000, fragsize=4096, **kwargs):
        """
        Send an MPDU in the air from the AP.
        
        Keyword arguments:
        da -- the source MAC address (default = AP MAC address)
        sa -- the source MAC address (default = AP MAC address)
        body -- the frame body of the MPDU
        length -- the length of the body generated if no body specified (default = 1000)
        fragsize -- fragment size (default = 4096)
        others -- keyword arguments passed to all building functions
        
        return -- the list of MPDU objects sent
        """
        
        # check arguments
        if da == None:
            da = self.macaddr
        if sa == None:
            sa = self.macaddr
        if body == None:
            assert(isinstance(length, int))
            p = payload.build(length)
            body = llcsnap.build(p, **kwargs)
        
        # sanity check
        assert(isinstance(sa, str) and len(sa) == 6)
        assert(isinstance(da, str) and len(da) == 6)
        assert(isinstance(body, str))

        # increment the sequence control and reinitialize the fragment number
        if "sctl" in kwargs:
            self.sctl = kwargs["sctl"]
        else:
            self.sctl = wifi.sctl_incsn(self.sctl)
        
        # generate the MPDUs
        body_len = len(body)
        mpdus = []

        # transmit all but the last fragment
        while body_len > fragsize:
            # set the sctl
            kwargs["sctl"] = self.sctl
            # transmit the next fragment
            mpdu = wifi_rx.rx_mpdu(body[:fragsize], bssid=self.macaddr, da=da, sa=sa,
                                   morefrag = True, **kwargs)
            mpdus.append(mpdu)
            self.a.air_receive_data(mpdu)

            # increment the fragment number
            self.sctl = wifi.sctl_incfn(self.sctl)

            # remove the body part that was already transmitted
            body = body[fragsize:]
            body_len = len(body)

        # set the sctl
        kwargs["sctl"] = self.sctl
        # transmit the last fragment
        mpdu = wifi_rx.rx_mpdu(body, bssid=self.macaddr, da=da, sa=sa, **kwargs)
        mpdus.append(mpdu)
        self.a.air_receive_data(mpdu)

        return mpdus

    def rx_mpdu(self, queue):
        """
        Wait for an MPDU in the air for the AP.
        
        Mandatory arguments:
        queue -- air queue on which the MPDU is expected
        
        return -- the received MPDU object
        """
        # sanity check
        assert(isinstance(queue, int) and queue <= wifi.AIR_CTRL)
        
        # wait for the data sent over the air
        mpdu = self.a.air_send_data(queue)

        # check the channel is correct
        assert(mpdu.channel == self.channel)
        assert(mpdu.addr1 == self.macaddr or mpdu.addr1 == ethernet.bcast)

        return mpdu

    def rx_msdu(self, msdu, message):
        """
        Wait for an MPDU in the air for the AP.
        
        Mandatory arguments:
        msdu -- the pushed MSDU object
        message -- the message to print in case of error
        """
        # sanity check
        assert(isinstance(msdu, ethernet.tx_msdu))
        
        # wait for an MPDU from the air
        mpdu = self.rx_mpdu(msdu.prio)

        # compare the received MPDUs to the MSDU
        compare.tx_mpdu2msdu(self.a, mpdu, msdu, self.macaddr, message)

        # wait for data send confirmation
        self.a.host_send_data_cfm(msdu)

    def start(self):
        """
        Start the AP beacon transmission in the air.
        """
        mpdu = wifi_rx.rx_mpdu(wifi.beacon(self.period, self.ssid), bssid = self.macaddr,
                               da = ethernet.bcast, sa = self.macaddr, 
                               fctl = wifi.fctl_beacon, fromds = False, sctl = 0)

        self.a.ap_start(self.macaddr, self.channel, self.period, mpdu)
        
        self.started = True

    def stop(self):
        """
        Stop the AP beacon transmission in the air.
        """
        assert(self.started)
        
        self.a.ap_stop(self.macaddr)
        
        self.started = False