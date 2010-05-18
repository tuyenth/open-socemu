########################################################################################
# RX test cases suite
########################################################################################

import agent
import ethernet
import wifi
import accesspoint
import host

STA = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0xBC)
STA2 = ethernet.addr(0x00, 0x56, 0x56, 0x78, 0x9A, 0xBC)
AP = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0xBC)

class TcRx:
    """
    RX test cases class.
    """

    def __init__(self, a):
        """
        Initialize the RX tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        
        # save the agent reference
        self.a = a

        self.a.tc_h1("RX TESTS")

        # create the host instance
        self.host = host.Host(a, STA)
        # create the access point instance
        self.ap = accesspoint.AccessPoint(a, AP, channel=5, period=100, ssid="louis")

        # reset the host (resetting the MIBs)
        self.host.reset(True)

        # set the host MAC address
        self.host.dbg_macaddr()
        
        # open the data path for the AP and set the STA in ESS
        self.host.dbg_add(self.ap.macaddr, 1)

    def run_basics(self):
        """
        Perform basic RX tests.
        """
        self.a.tc_h2("Basic tests")
    
        # TEST RX of a frame that has a sequence control equal to 0
        self.a.tc_h3("Null sequence control field test")
        mpdus = self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=64, sctl=0)
        self.host.rx_msdu(mpdus, "RX check 0 sequence control frame")

        # TEST RX normal frames (and check that too large frames are discarded)
        self.a.tc_h3("Normal frames + size discard")
        for i in range(0, 200):
            length = 46 + (i*11)
            # receive the frames from the air
            mpdus = self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=length)

            # check if the frame should have been received correctly or not
            if length <= 1500:
                # wait for the received frame
                self.host.rx_msdu(mpdus, "RX normal(%d)"%(i, ))
            else:
                # check with a small packet that previous was not filtered out
                mpdus = self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=46)
                self.host.rx_msdu(mpdus, "RX filtered out checking")

        # TEST RX duplicate frames
        self.a.tc_h3("Duplicate frames")
        for i in range(0, 100):
            # receive the frames from the air
            mpdus = self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=(46+(i*9)))

            # wait for the received frame
            self.host.rx_msdu(mpdus, "RX duplicate(%d)"%(i, ))
            
            # send again the same frame (with retry bit set) -> should not be received
            mpdus[0].add_retry()
            self.a.air_receive_data(mpdus[0])

        # TEST RX 802.1q frames
        self.a.tc_h3("802.1q frames")
        for i in range(0, 100):
            # receive the frames from the air
            mpdus = self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=(46+(i*11)),
                                    dot1q=(3,False,0x100))
            
            # wait for the received frame
            self.host.rx_msdu(mpdus, "RX 802.1q(%d)"%(i, ))
    
    
        # TEST several reception back to back
        self.a.tc_h3("Back to back frames")
        for i in range(0, 100):
            # reinitialize the entire list
            mpdus = []
            for j in range((i%8) + 1):
                # receive the frames from the air
                mpdus.append(self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=46))
    
            for j in range(0, (i%8) + 1):
                # wait for the received frame
                self.host.rx_msdu(mpdus[j], "RX back2back(%d,%d)"%(i,j))

    def run_frag_test(self):
        """
        Perform defragmentation test.
        """
        self.a.tc_h2("Fragmented frames")
    
        # TEST the fragmented reception
        for i in range(400, 600):
            # receive the frames from the air
            mpdus = self.ap.tx_msdu(da=self.host.macaddr, sa=STA2, length=1500, fragsize=i)

            # wait for the received frame
            self.host.rx_msdu(mpdus, "RX fragmented(%d)"%(i,))
