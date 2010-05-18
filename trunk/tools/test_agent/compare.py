########################################################################################
# Comparing incoming frames toolbox
########################################################################################

import llcsnap
import ethernet
import wifi_rx
import wifi_tx
import common.bytes

def rx_mpdu2msdu(a, mpdus, msdu, message):
    """
    This function checks that the content of the MPDU and the MSDU match.  Potentially,
    the mpdu parameter is a list (in case of fragmentation).
    
    Mandatory parameters:
    mpdus -- the list of RX MPDU objects as received from the air
    msdu -- the RX MSDU object
    """
    
    # sanity check
    assert(isinstance(mpdus, list))
    assert(isinstance(msdu, ethernet.rx_msdu))

    # loop on all the MPDUs
    mpdu_bd = ""
    mpdu_addr1 = None
    mpdu_addr2 = None
    mpdu_addr3 = None
    for m in mpdus:
        # sanity check
        assert(isinstance(m, wifi_rx.rx_mpdu))
        # unpack the reception MPDUs
        mpdu_bd += m.bd
        
        if mpdu_addr1 == None:
            mpdu_addr1 = m.addr1
        if mpdu_addr2 == None:
            mpdu_addr2 = m.addr2
        if mpdu_addr3 == None:
            mpdu_addr3 = m.addr3
            
        # sanity check:
        assert(mpdu_addr1 == m.addr1)
        assert(mpdu_addr2 == m.addr2)
        assert(mpdu_addr3 == m.addr3)

    # extract the packet information from the body of the defragmented frame
    (mpdu_ok, (mpdu_1q, mpdu_pr, mpdu_cf, mpdu_vl), mpdu_et, mpdu_pl) = llcsnap.unpack(mpdu_bd)
    
    # sanity check
    assert(mpdu_ok)

    if a.verbosity > 1:
        print(message + ": unpacked frame = %s <- %s, type = %02X, data = \n%s"%
              (common.bytes.convert_hex(msdu.da), common.bytes.convert_hex(msdu.sa), 
               msdu.etype, common.bytes.dump(msdu.payload)))
    # check if the received frame is equal to the sent one
    if msdu.da != mpdu_addr1:
        a.error(message+": DA not correct (sent=%s, rcvd=%s)"%
                (common.bytes.convert_hex(mpdu_addr2), common.bytes.convert_hex(msdu.da)))
    if msdu.sa != mpdu_addr3:
        a.error(message+": SA not correct (sent=%s, rcvd=%s)"%
                (common.bytes.convert_hex(mpdu_addr3), common.bytes.convert_hex(msdu.sa)))
    if msdu.etype != mpdu_et:
        a.error(message+": ETHERTYPE not correct (sent=%02X, rcvd=%02X)"% (mpdu_et, msdu.etype))
    if msdu.payload != mpdu_pl:
        a.error(message+": PAYLOAD not correct:\nsent:\n%srcvd:\n%s"%
                (common.bytes.dump(mpdu_pl), common.bytes.dump(msdu.payload)))

def tx_mpdu2msdu(a, mpdu, msdu, ap, message):
    """
    Check that the content of the MPDU and the MSDU match.
    
    Mandatory arguments:
    a -- test agent object to use to communicate with the reference plateform
    mpdu -- MPDU object from the air
    msdu -- MSDU object frame sent
    ap -- AccessPoint through which the frame was transported
    message -- message to print in case of error
    """
    
    # sanity check
    assert(isinstance(mpdu, wifi_tx.tx_mpdu))
    assert(isinstance(msdu, ethernet.tx_msdu))
    assert(isinstance(ap, str) and len(ap) == 6)
    
    # sanity check
    if mpdu.uPatternPT != 0xBADCAB1E:
        a.error(message+": TX policy pattern is wrong 0x%08X != 0xBADCAB1E"%
                (mpdu.uPatternPT, ))
    if mpdu.theorylen != mpdu.framelen:
        a.error(message+": TX calculated HW MPDU length (%d) different from SW MPDU length (%d)"%
                (mpdu.theorylen, mpdu.framelen))
    if mpdu.addr1 != ap:
        a.error("TX: ADDR1(%s) not equal to AP(%s)"%(common.bytes.convert_hex(mpdu.addr1), 
                                                     common.bytes.convert_hex(ap)))
    if mpdu.addr2 != msdu.sa:
        a.error("TX: ADDR2(%s) not equal to SA(%s)"%(common.bytes.convert_hex(mpdu.addr2),
                                                     common.bytes.convert_hex(msdu.sa)))
    if mpdu.addr3 != msdu.da:
        a.error("TX: ADDR3(%s) not equal to DA(%s)"%
                (common.bytes.convert_hex(mpdu.addr3),
                 common.bytes.convert_hex(msdu.da)))
    
    # extract the information from the payload
    (mpdu_ok, (mpdu_1q, mpdu_pr, mpdu_cf, mpdu_vl), mpdu_et, mpdu_pl) = llcsnap.unpack(mpdu.bd)
    
    # sanity check
    if not mpdu_ok:
        a.error(message+": LLC/SNAP malformed:\n%s"%(common.bytes.dump(mpdu_pl), ), )
    if mpdu_1q != msdu.dot1q:
        a.error(message+": LLC/SNAP: found 802.1q not expected")
    if mpdu_et != msdu.etype:
        a.error(message+": ETHERTYPE not correct (sent=%02X, rcvd=%02X)"%
                (mpdu_et, msdu.etype))
    #   - strip off the FCS from the MPDU (this had to be appended even if not used)
    if msdu.payload != mpdu_pl[0:-4]:
        a.error(message+": PAYLOAD not correct:\nsent:\n%srcvd:\n%s"%
                (common.bytes.dump(mpdu_pl[0:-4]), common.bytes.dump(msdu.payload)))

