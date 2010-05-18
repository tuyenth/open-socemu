########################################################################################
# Test Agent : user interface to DUT
########################################################################################

import sys
import getopt
import random
import cProfile
import agent

# all supported testcases
import tc_STA
import tc_AP

usage_doc ="""
Synopsis:
    test_agent.py [--help] [-p filename] [-v#] [AP|STA]

       --help: self explanatory
       -p: run with Python profiling turned on (filename is the output of the profiling)
       -v#: verbose level (# is a level from 0 to 9)
       [AP|STA]: indicate the testcase suite to run (defaults to STA)
"""

def usage():
    print usage_doc

def main():
    # by default, verbose is not turned on
    verbosity = 0
    # by default profiling is turned off
    profiling = False

    # parse the command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], "v:p:", ["help", ])
    except getopt.GetoptError:
        # print help information and exit:
        usage()
        sys.exit(-1)

    for o, a in opts:
        if o == "--help":
            usage()
            sys.exit(-1)
        if o == "-v":
            try:
                verbosity = int(a)
            except:
                print("Verbose level is not an integer (%s)"%(a,))
                sys.exit(-1)
        if o == "-p":
            profiling = True
            profilename = a

    # sanity check
    if len(args) > 1:
        print("Number of parameters not supported (%s)"%(args,))
        sys.exit(-1)

    if len(args) != 1:
        tcsuite = "STA"
    else:
        tcsuite = args[0].upper()

    # sanity check
    if tcsuite not in ("AP", "STA"):
        print("Testcase suite name not supported (%s)"%(tcsuite,))
        sys.exit(-1)
        

    # initialize the random number generator
    random.seed(12345)
    
    # create the agent instance to communicate
    a = agent.Agent(verbosity)
    
    if a.verbosity > 1:
        print("Testcase suite: "+tcsuite)
    
    # check if the profiling is required
    if profiling:
        if tcsuite == "AP":
            command = "tc_AP.run(a)"
        else:
            command = "tc_STA.run(a)"
        cProfile.runctx(command, globals(), locals(), filename=profilename)
    else:
        if tcsuite == "AP":
            tc_AP.run(a)
        else:
            tc_STA.run(a)

    # close the test agent
    a.close()

# main execution
if __name__ == '__main__':
    main()
