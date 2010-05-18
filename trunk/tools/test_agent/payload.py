########################################################################################
# Payload toolbox
########################################################################################

import random

def build(length):
    """
    Build a random payload.
    
    Mandatory arguments:
    length -- requested length of the generated payload
    
    return -- the generated payload
    """
    return (("%%0%dX" % ((length*2),)) % random.getrandbits(length*8)).decode("hex")
