########################################################################################
# WiFi tool box
########################################################################################

import struct
import ethernet
import common.bytes

# define the MAC HW queues
Q_BCN = 0
Q_BK = 1
Q_BE = 2
Q_VI = 3
Q_VO = 4

# define the air queues in which the transmitted frames are pushed
AIR_DATA_TID0 = 0
AIR_DATA_TID1 = 1
AIR_DATA_TID2 = 2
AIR_DATA_TID3 = 3
AIR_DATA_TID4 = 4
AIR_DATA_TID5 = 5
AIR_DATA_TID6 = 6
AIR_DATA_TID7 = 7
AIR_DATA_NQOS = 8
AIR_MGMT = 9
AIR_CTRL = 10

AIR_NAME = ("DATA_TID0", "DATA_TID1", "DATA_TID2", "DATA_TID3", "DATA_TID4",
            "DATA_TID5", "DATA_TID6", "DATA_TID7", "DATA_NQOS", "MGMT", "CTRL")



# define the priority to queue mapping
Q_PRIO = (Q_BE, Q_BK, Q_BK, Q_BE, Q_VI, Q_VI, Q_VO, Q_VO)

# define the MAC HQ queue names
Q_NAME = ("Q_BCN", "Q_BK", "Q_BE", "Q_VI", "Q_VO")

# define the tuple of available channels
channels = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 36, 40, 44, 48, 52, 56, 60, 64, 
            100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165)

# define default values for frame control
fctl_protocolversion = 0x3
fctl_type = 0xC
fctl_type_mgt = 0
fctl_type_ctrl = 4
fctl_type_data = 8
fctl_type_reserved = 0xC
fctl_subtype = 0xF0
fctl_associationreq = fctl_type_mgt | 0x00
fctl_associationresp = fctl_type_mgt | 0x10
fctl_reassociationreq = fctl_type_mgt | 0x20
fctl_reassociationresp = fctl_type_mgt | 0x30
fctl_probereq = fctl_type_mgt | 0x40
fctl_proberesp = fctl_type_mgt | 0x50
fctl_beacon = fctl_type_mgt | 0x80
fctl_atim = fctl_type_mgt | 0x90
fctl_disassociation = fctl_type_mgt | 0xa0
fctl_authentication = fctl_type_mgt | 0xb0
fctl_deauthentication = fctl_type_mgt | 0xc0
fctl_action = fctl_type_mgt | 0xd0
fctl_actionnoack = fctl_type_mgt | 0xe0
fctl_controlwrapper = fctl_type_ctrl | 0x70
fctl_bar = fctl_type_ctrl | 0x80
fctl_ba = fctl_type_ctrl | 0x90
fctl_pspoll = fctl_type_ctrl | 0xa0
fctl_rts = fctl_type_ctrl | 0xb0
fctl_cts = fctl_type_ctrl | 0xc0
fctl_ack = fctl_type_ctrl | 0xd0
fctl_cfend = fctl_type_ctrl | 0xe0
fctl_cfendcfack = fctl_type_ctrl | 0xf0
fctl_data = fctl_type_data | 0x00
fctl_datacfack = fctl_type_data | 0x10
fctl_datacfpoll = fctl_type_data | 0x20
fctl_datacfackcfpoll = fctl_type_data | 0x30
fctl_null = fctl_type_data | 0x40
fctl_cfack = fctl_type_data | 0x50
fctl_cfpoll = fctl_type_data | 0x60
fctl_cfackcfpoll = fctl_type_data | 0x70
fctl_qosdata = fctl_type_data | 0x80
fctl_qosdatacfack = fctl_type_data | 0x90
fctl_qosdatacfpoll = fctl_type_data | 0xa0
fctl_qosdatacfackcfpoll = fctl_type_data | 0xb0
fctl_qosnull = fctl_type_data | 0xc0
fctl_qoscfpoll = fctl_type_data | 0xe0
fctl_qoscfackcfpoll = fctl_type_data | 0xf0
fctl_tods = 0x100
fctl_fromds = 0x200
fctl_morefrag = 0x400
fctl_retry = 0x800
fctl_pwrmgt = 0x1000
fctl_moredata = 0x2000
fctl_protected = 0x4000
fctl_order = 0x8000

capa_ess = 0x1
capa_ibss = 0x2
capa_cfpollable = 0x4
capa_cfpollreq = 0x8
capa_privacy = 0x10
capa_shortpreamble = 0x20
capa_qos = 0x200

# define the bit masks in the sequence control field
sctl_seqnum = 0xFFF0
sctl_fragnum = 0x000F

# define the IE identifier
ie_ssid = 0
ie_supportedrates = 1
ie_fh = 2
ie_ds = 3
ie_cf = 4
ie_tim = 5
ie_ibss = 6
ie_country = 7
ie_hopparam = 8
ie_hoptable = 9
ie_request = 10
ie_bssload = 11
ie_edca = 12
ie_tspec = 13
ie_tclas = 14
ie_schedule = 15
ie_challenge = 16
ie_powerconstraint = 32
ie_powercapa = 33
ie_tpcreq = 34
ie_tpcrep = 35
ie_supchannels = 36
ie_chswitch = 37
ie_measreq = 38
ie_mesrep = 39
ie_quiet = 40
ie_ibssdfs = 41
ie_erpinfo = 42
ie_tsdelay = 43
ie_tclasproc = 44
ie_htcapa = 45
ie_qoscapa = 46
ie_rsn = 48
ie_extendedrates = 50
ie_htoperation = 61
ie_secondarych = 62
ie_2040coex = 72
ie_2040intolerant = 73
ie_overlapping = 74
ie_extendedcapa = 127
ie_vs = 221

# define the name of the frame per typesubtype
framename = {fctl_associationreq:"ASSOCIATION_REQUEST",
             fctl_associationresp:"ASSOCIATION_RESPONSE",
             fctl_reassociationreq:"REASSOCIATION_REQUEST",
             fctl_reassociationresp:"REASSOCIATION_RESPONSE",
             fctl_probereq:"PROBE_REQUEST",
             fctl_proberesp:"PROBE_RESPONSE",
             fctl_beacon:"BEACON",
             fctl_atim:"ATIM",
             fctl_disassociation:"DISASSOCIATION",
             fctl_authentication:"AUTHENTICATION",
             fctl_deauthentication:"DEAUTHENTICATION",
             fctl_action:"ACTION",
             fctl_actionnoack:"ACTIONNOACK",
             fctl_controlwrapper:"CONTROLWRAPPER",
             fctl_bar:"BLOCKACK_REQUEST",
             fctl_ba:"BLOCKACK",
             fctl_pspoll:"PS_POLL",
             fctl_rts:"RTS",
             fctl_cts:"CTS",
             fctl_rts:"RTS",
             fctl_ack:"ACK",
             fctl_cfend:"CFEND",
             fctl_cfendcfack:"CFENDCFACK",
             fctl_data:"DATA",
             fctl_datacfack:"DATACFACK",
             fctl_datacfpoll:"DATACFPOLL",
             fctl_datacfackcfpoll:"DATACFACKCFPOLL",
             fctl_null:"NULL",
             fctl_cfack:"CFACK",
             fctl_cfpoll:"CFPOLL",
             fctl_cfackcfpoll:"CFACKCFPOLL",
             fctl_qosdata:"QOSDATA",
             fctl_qosdatacfack:"QOSDATACFACK",
             fctl_qosdatacfpoll:"QOSDATACFPOLL",
             fctl_qosdatacfackcfpoll:"QOSDATACFACKCFPOLL",
             fctl_qosnull:"QOSNULL",
             fctl_qoscfpoll:"QOSCFPOLL",
             fctl_qoscfackcfpoll:"QOSCFACKCFPOLL"
             }

def fctl_unpack(fctl):
    """
    Unpack all the subfields from the frame control.
    
    return -- following tuple:
      (type : int, subtype : int, typesubtype : int, to DS : bool, from DS : bool, 
       more frag: bool, retry : bool, ps : bool, more data : bool, protected : bool, 
       order : bool)
    """
    return ((fctl & fctl_type), 
            (fctl & fctl_subtype), 
            (fctl & (fctl_type|fctl_subtype)), 
            (fctl & fctl_tods) != 0, (fctl & fctl_fromds) != 0, 
            (fctl & fctl_morefrag) != 0, (fctl & fctl_retry) != 0, 
            (fctl & fctl_pwrmgt) != 0, (fctl & fctl_moredata) != 0, 
            (fctl & fctl_protected) != 0, (fctl & fctl_order) != 0)

def fctl_pack(type, tds, fds, mf, r, ps, md, p, o):
    """
    Packs all the subfields of the frame control field.
    
    Mandatory arguments:
    type -- type + subtype of the frame
    tds -- to DS boolean
    fds -- from DS boolean
    mf -- more frag boolean
    r -- retry boolean
    md -- more data boolean
    p -- protected boolean
    o -- order boolean

    return -- the formatted frame control
    """
    
    fctl = type
    
    if tds:
        fctl |= fctl_tods
    if fds:
        fctl |= fctl_fromds
    if mf:
        fctl |= fctl_morefrag
    if r:
        fctl |= fctl_retry
    if md:
        fctl |= fctl_moredata
    if p:
        fctl |= fctl_protected
    if o:
        fctl |= fctl_order
    
    return fctl

def sctl_incsn(sctl):
    """
    Increment the sequence number field and reinitialize the current fragment number.
    
    Mandatory parameters:
    sctl -- current sequence control
    
    return -- the updated sequence control
    """
    return (sctl + (1 << 4)) & sctl_seqnum

def sctl_incfn(sctl):
    """
    Increment the fragment number field.
    
    Mandatory parameters:
    sctl -- current sequence control
    
    return -- the updated sequence control
    """
    return sctl + 1

def find_ie(typesubtype, body, ie):
    """
    Returns the content of an IE inside a frame body.  None if not found.
    """
    
    length = len(body)
    
    if typesubtype == fctl_beacon:
        start = 8 + 2 + 2
    elif typesubtype == fctl_probereq:
        start = 0
    elif typesubtype == fctl_authentication:
        start = 2 + 2 + 2
    elif typesubtype == fctl_associationreq:
        start = 2 + 2
    else:
        assert(false)
    
    # sanity check
    assert(start < length)
    
    while start < length:
        (ie_id, ie_len) = struct.unpack("BB", body[start:start+2])
        if ie_id == ie:
            return body[start+2:start+2+ie_len]
        else:
            start = start+2+ie_len
        assert(start <= length)

def beacon(interval, ssid, qos=True):
    """
    Build a beacon body.
    
    Mandatory arguments:
    interval -- beacon interval
    ssid -- SSID in field of the beacon
    
    Keyword arguments:
    qos -- indicate if QoS is supported (default True)
    """
    # timestamp
    body =  struct.pack("<Q", 0)
    # beacon interval
    body += struct.pack("<H", interval)
    # capability
    capability = capa_ess
    if qos:
        capability |= capa_qos
    body += struct.pack("<H", capability)
    # SSID
    assert(len(ssid) <= 34)
    assert(len(ssid) >= 1)
    body += struct.pack("BB"+str(len(ssid))+"s", 0, len(ssid), ssid)
    # supported rates
    body += struct.pack("BBB", 1, 1, 0x8C)
    # FH -> not implemented
    #body += struct.pack("BBHBBB", 2, 5, 0, 0, 0, 0)
    # DS
    #body += struct.pack("BBB", 3, 1, 0)
    # CF -> not implemented
    # IBSS -> not implemented
    # TIM -> not implemented
    # Country information element
    body += struct.pack("BB3sBBB", 7, 6, "US ", 0, 0, 0)
    
    return body

def proberesponse(interval, ssid):
    # timestamp
    body =  struct.pack("<Q", 0)
    # beacon interval
    body += struct.pack("<H", interval)
    # capability
    body += struct.pack("<H", 0)
    # SSID
    assert(len(ssid) <= 34)
    assert(len(ssid) >= 1)
    body += struct.pack("BB"+str(len(ssid))+"s", 0, len(ssid), ssid)
    # supported rates
    body += struct.pack("BBB", 1, 1, 0x96)
    # FH -> not implemented
    #body += struct.pack("BBHBBB", 2, 5, 0, 0, 0, 0)
    # DS
    #body += struct.pack("BBB", 3, 1, 0)
    # CF -> not implemented
    # IBSS -> not implemented
    # TIM -> not implemented
    # Country information element
    body += struct.pack("BB3sBBB", 7, 6, "US ", 0, 0, 0)
    
    return body

def authenticate():
    # authentication algorithm
    body =  struct.pack("<H", 0)
    # authentication transaction sequence number
    body += struct.pack("<H", 2)
    # status (OK)
    body += struct.pack("<H", 0)
    
    return body

def deauthenticate():
    # reason (unspecified)
    body =  struct.pack("<H", 1)
    
    return body

def disassociation():
    # reason (unspecified)
    body =  struct.pack("<H", 1)
    
    return body

def associationresponse():
    # capability
    body = struct.pack("<H", capa_ess)
    # status (OK)
    body += struct.pack("<H", 0)
    # AID
    body += struct.pack("<H", 0x0123)
    # supported rates
    body += struct.pack("BBB", 1, 1, 0x96)
    
    return body
