########################################################################################
# Station class
########################################################################################

import agent
import ethernet

class Station:
    """Station class"""
    def __init__(self, a, macaddr):
        """
        Initialize the station.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        macaddr -- the MAC address of the station to create
        """
        
        # sanity check
        assert(isinstance(a, agent.Agent))
        assert(isinstance(macaddr, str) and len(macaddr) == 6)

        # save the agent
        self.a = a
        # save the MAC address
        self.macaddr = macaddr
        
        # initialize the sequence counter
        self.sctl = 0

    def rx_mpdu(self, sa):
        """Send an MPDU from the STA in the air"""
        pass
    
    def tx_mpdu(self, msdu, priority = 0, sa = "\0\0\0\0\0\0"):
        """
        Wait for an MPDU from the host in the air for the STA.
        """
        pass

