########################################################################################
# Ethernet framing tool box
########################################################################################

import struct
import payload

etype_ipv4 = 0x0800
etype_arp = 0x0806
etype_rarp = 0x8035
etype_etalk = 0x809B
etype_dot1q = 0x8100
etype_ipx = 0x8137
etype_eapoll = 0x888E
etype_ipv6 = 0x86DD

bcast = "\xFF\xFF\xFF\xFF\xFF\xFF"

min_length = 1
max_length = 1500

def addr(a,b,c,d,e,f):
    """"Create a MAC address string"""
    a = struct.pack("BBBBBB", a, b, c, d, e, f)
    return a

def unpack(msdu):
    """
    This function unpacks an entire Ethernet msdu, extracting the following tuple:
    (ok = bool, da = str, sa = str, 
     (802.1q = bool, priority = int, cfi = bool, vlanid = int), 
     ethertype, payload = str)
    """
    # initialize all the elements
    ok = True
    da = ""
    sa = ""
    dot1qfound = False
    priority = 0
    cfi = False
    vlanid = 0
    ethertype = 0xFFFF
    payload = msdu
    
    while True:
        # check the first LLC/SNAP header
        if len(msdu) < 14:
            ok = False
            break
    
        # extract the DA, the SA and the first ethertype
        (da, sa, ethertype) = struct.unpack("!6s6sH", msdu[0:14])
        
        # check if it is a not a 1Q msdu
        if ethertype != etype_dot1q:
            payload = msdu[14:]
            break

        # check the size of the msdu
        if len(msdu) < 18:
            ok = False
            break
    
        # mark a 1Q msdu found
        dot1qfound = True
        
        # extract the TCI
        tci = struct.unpack("!H", msdu[14:16])[0]
        
        priority = (tci & 0xE000) >> 13
        cfi = ((tci & 0x1000) != 0)
        vlanid = tci & 0x0FFF
    
        # extract the second ethertype
        ethertype = struct.unpack("!H", msdu[16:18])[0]
        
        payload = msdu[18:]
        
        break

    return (ok, da, sa, (dot1qfound, priority, cfi, vlanid), ethertype, payload)

class tx_msdu:
    """
    Ethernet frame for transmission class.
    """
    def __init__(self, da, sa, length, prio, **kwargs):
        """
        Create the Ethernet frame for transmission.
        
        Mandatory arguments:
        da -- destination MAC address
        sa -- source MAC address
        length -- length of the body
        prio -- user priority on which the frame must be sent
        
        Keyword arguments:
        dot1q -- use this tuple to build an 802.1q encapsulated payload 
            (cfi = bool, vlanid = int)
        etype -- ethernet type field.  If not specified, use 0x0800 (IPv4 content)
        """
        # sanity check
        assert(isinstance(da, str) and len(da) == 6)
        assert(isinstance(sa, str) and len(sa) == 6)
        assert(isinstance(prio, int))
        assert(prio < 8)
        assert(length > min_length)
        assert(length <= max_length)

        # save all the parameters of the frame
        self.da = da
        self.sa = sa
        self.pl_length = length
        self.prio = prio

        # generate a random frame
        self.payload = payload.build(self.pl_length)
        
        # every Ethernet frames start with destination and source
        self.msdu = da + sa
        
        # check if an 802.1q encapsulated frame is requested
        if "dot1q" in kwargs:
            self.dot1q = True
            # extract the information from the tuple
            (self.cfi, self.vlanid) = kwargs["dot1q"]
            # append the 802.1q ethertype
            self.msdu += struct.pack("!H", etype_dot1q)
            # sanity check
            assert(isinstance(self.cfi, bool))
            assert(isinstance(self.vlanid, int))
            assert(self.vlanid < 8192)
            
            tci = self.prio << 13
            if cfi:
                tci |= 0x1000
            tci |= vlanid
            
            # append the TCI
            self.msdu += struct.pack("!H", tci)
        else:
            self.dot1q = False
            self.cfi = False
            self.vlanid = 0

        # check if there is a specific ethertype specified
        if "etype" in kwargs:
            self.etype = kwargs["etype"]
            assert(isinstance(self.etype, int))
        else:
            # default is standard IPv4
            self.etype = etype_ipv4
        # add the Ethernet type to the frame
        self.msdu += struct.pack("!H", self.etype)
        
        # append the payload
        self.msdu += self.payload

        # save the global length
        self.length = len(self.msdu)

    def __str__(self):
        return self.msdu

class rx_msdu:
    """
    Ethernet frame class as received.
    """
    def __init__(self, msdu):
        """
        Create the Ethernet frame from received MSDU.
        
        Mandatory arguments:
        msdu -- received MSDU
        """
        # sanity check
        assert(isinstance(msdu, str))

        # save the received MSDU
        self.msdu = msdu

        (ok, self.da, self.sa, (self.dot1q, self.prio, self.cfi, self.vlanid), self.etype,
         self.payload) = unpack(self.msdu)
        
        # sanity check
        assert(ok)

    def __str__(self):
        return self.msdu
