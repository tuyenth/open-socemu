########################################################################################
# MIB accesses test cases suite
########################################################################################

import struct
import agent

class TcMib:
    """
    MIB test cases class.
    """
    def __init__(self, a):
        """
        Initializes the MIB tests.
        
        Mandatory arguments:
        a -- test agent object to use to communicate with the DUT
        """
        # sanity check
        assert(isinstance(a, agent.Agent))
        
        # save the agent reference
        self.a = a

        self.a.tc_h1("MIB TESTS")

        # write a MIB element
        self.a.nxapi_mibset_req("mib_dot11HtStationConfigTable.dot11HTOperationalMCSSet", "\7\1\2\3\4\5\6\7")
        
        # read a MIB element
        option = struct.unpack("B", a.nxapi_mibget_req("mib_commonconfigtable.dot11QosOptionImplemented"))[0]
        assert(option == 1)
    
        # readback the first MIB element
        value = self.a.nxapi_mibget_req("mib_dot11HtStationConfigTable.dot11HTOperationalMCSSet")
        assert(value[:8] == "\7\1\2\3\4\5\6\7")
    
        # read a MIB element
        option = struct.unpack("B", self.a.nxapi_mibget_req("mib_commonconfigtable[0].dot11QosOptionImplemented"))[0]
        assert(option == 1)
        
        # read a MIB element
        option = struct.unpack("B", self.a.nxapi_mibget_req("mib_commonconfigtable.dot11QosOptionImplemented[0]"))[0]
        assert(option == 1)
    
        # read a MIB element
        option = struct.unpack("B", self.a.nxapi_mibget_req("mib_commonconfigtable[0].dot11QosOptionImplemented[0]"))[0]
        assert(option == 1)
    
