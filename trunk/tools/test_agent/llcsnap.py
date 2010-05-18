########################################################################################
# LLC/SNAP frame build tool
########################################################################################

import struct
import ethernet

llc_dsap = 0xAA
llc_ssap = 0xAA
llc_ctrl = 0x03
snap_oui0 = 0x00
snap_oui1 = 0x00
snap_oui2 = 0x00
llcsnap_header = struct.pack("BBBBBB", llc_dsap, llc_ssap, llc_ctrl, snap_oui0, 
                             snap_oui1, snap_oui2)

snap_ethtype_ip = struct.pack("!H", ethernet.etype_ipv4)
snap_ethtype_dot1q = struct.pack("!H", ethernet.etype_dot1q)

def build(payload, **kwargs):
    """
    This function encapsulates a payload in an LLC/SNAP frame.
    
    Mandatory arguments:
    payload -- encapsulate this payload
    
    Keyword arguments:
    ethtype -- use this value as ethertype in the SNAP header, if not specified, the 
            IP ethertype will be used
    dot1q -- use this tuple to build an 802.1q encapsulated payload 
            (priority = int, cfi = bool, vlanid = int)
            
    return -- the generated encapsulated payload
    
    Note that all LLC/SNAP elements are stored in big endian.
    """
    # initialize the first LLC/SNAP header
    f  = llcsnap_header
    
    # check if an 802.1q encapsulated frame is requested
    if "dot1q" in kwargs:
        # extract the information from the tuple
        (prio, cfi, vlanid) = kwargs["dot1q"]
        # append the 802.1q ethertype
        f += snap_ethtype_dot1q
        # sanity check
        assert(isinstance(prio, int))
        assert(prio < 8)
        assert(isinstance(cfi, bool))
        assert(isinstance(vlanid, int))
        assert(vlanid < 8192)
        
        tci = prio << 13
        if cfi:
            tci |= 0x1000
        tci |= vlanid
        
        # append the TCI
        f += struct.pack("!H", tci)
    
        # append the second LLC/SNAP header
        f += llcsnap_header

    # ethertype field (stored in big endian format)
    if "ethtype" in kwargs:
        ethtype = struct.pack("!H", kwargs["ethtype"])
    else:
        ethtype = snap_ethtype_ip
    f += ethtype
    
    # append the payload
    f += payload
    
    return f

def unpack(frame):
    """
    This function decapsulates an LLC/SNAP frame.
    It returns the following tuple:
    (ok = bool, (802.1q = bool, priority = int, cfi = bool, vlanid = int), ethertype, 
     payload = str)
    """
    
    # initialize all the element
    ok = True
    dot1qfound = False
    priority = 0
    cfi = False
    vlanid = 0
    ethertype = 0xFFFF
    payload = frame
    
    while True:
        # check the first LLC/SNAP header
        if (len(frame) < 8) or (frame[0:6] != llcsnap_header):
            ok = False
            break
    
        # extract the first LLC/SNAP ethertype
        ethertype = struct.unpack("!H", frame[6:8])[0]
        
        # check if it is a not a 1Q frame
        if ethertype != ethernet.etype_dot1q:
            payload = frame[8:]
            break

        # check the second LLC/SNAP header
        if (len(frame) < 18) or (frame[10:16] != llcsnap_header):
            ok = False
            break
    
        # mark a 1Q frame found
        dot1qfound = True
        
        # extract the TCI
        tci = struct.unpack("!H", frame[8:10])[0]
        
        priority = (tci & 0xE000) >> 13
        cfi = ((tci & 0x1000) != 0)
        vlanid = tci & 0x0FFF
    
        # extract the second LLC/SNAP ethertype
        ethertype = struct.unpack("!H", frame[16:18])[0]
        
        payload = frame[18:]
        
        break

    return (ok, (dot1qfound, priority, cfi, vlanid), ethertype, payload)
