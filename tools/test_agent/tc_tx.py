########################################################################################
# TX test cases suite
########################################################################################

import agent
import ethernet
import host
import accesspoint

STA = ethernet.addr(0x00, 0x34, 0x56, 0x78, 0x9A, 0xBC)
AP = ethernet.addr(0x00, 0x12, 0x56, 0x78, 0x9A, 0xBC)

class TcTx:
    """
    Transmit test cases class.
    """
    def __init__(self, a):
        """
        Initializes the TX tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        
        # save the agent reference
        self.a = a
    
        self.a.tc_h1("TX TESTS")
    
        # create the host instance
        self.host = host.Host(a, STA)
        # create the access point instance
        self.ap = accesspoint.AccessPoint(a, AP)
    
        # reset the host without resetting the MIBs
        self.host.reset(False)
    
        # open the path for the AP and configure the mode as ESS STA
        self.host.dbg_add(self.ap.macaddr, 1)

    def run_basics(self):
        """
        Perform basic TX tests.
        """
        # TEST TX full traffic
        self.a.tc_h2("Back to back frames")
        msdus=[]
        for i in range(30):
            msdus.append(self.host.tx_msdu(da=self.ap.macaddr, length=((64+i)%1500), 
                                           prio=1))
    
        for i in range(30):
            self.ap.rx_msdu(msdus[i], "TX back2back(%d)"%(i, ))
    
        # TEST TX data path
        self.a.tc_h2("Normal frames with delay")
        # create an array of payload to hold the payload while waiting for confirm
        for i in range(100):
            # reinitialize the MSDU list
            msdus=[]
            # send data packets
            for j in range((i%9) + 1):
                msdus.append(self.host.tx_msdu(da=self.ap.macaddr, length=(64+(i*11)), 
                                               prio=1))

                # wait between the various requests to test preemption
                self.a.wait(100*i)
            
            # expect the TX transfer and confirmation for the packets
            for j in range((i%9) + 1):
                self.ap.rx_msdu(msdus[j], "TX normal with delay(%d)"%(i, ))

    def run_multiq(self):
        """
        Perform TX tests on multiple queues simultaneously.
        """
        self.a.tc_h2("Normal frames on multiple queues")
        # reinitialize the MSDU list
        msdus=[]
        for i in range(100):
            # send data packets on each queue
            for j in range(8):
                msdus.append(self.host.tx_msdu(da=self.ap.macaddr, length=(64+(i*11)), 
                                               prio=j))
        # expect the TX transfer and confirmation for the packets
        for msdu in msdus:
            self.ap.rx_msdu(msdu, "TX normal on multiple queues (iteration %d)"%(i, ))

    def run_longterm(self):
        """
        Perform TX tests on multiple queues simultaneously on all sizes and times.
        """
        self.a.tc_h2("Long term normal frames on multiple queues")
        for k in xrange(1450):
            self.a.tc_h3("Long term iteration %d"%(k, ))
            # reinitialize the MSDU list
            msdus=[]
            for i in xrange(100):
                # send data packets on each queue
                for j in xrange(8):
                    msdus.append(self.host.tx_msdu(da=self.ap.macaddr, length=(20+k), 
                                                   prio=j))
            # expect the TX transfer and confirmation for the packets
            for msdu in msdus:
                self.ap.rx_msdu(msdu, "TX normal on multiple queues (iteration %d)"%(i,))
