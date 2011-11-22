# Author: Alexis Heloir
# Models and Textures by: Alexis Heloir
# Last Updated: 17/10/08
#
import embr
import SMRPy
import getopt
import sys


#create the Realizer
realizer = SMRPy.Realizer()

def usage():
    print (
        "\n"
        "*******************************************************************************\n"
        "* EMBR realizer startup script                                                *\n" 
        "* USAGE: python agentAnim.py                                                  *\n"
        "* This program initialize and starts the components required                  *\n"   
        "* to run the EMBR realizer                                                    *\n"
        "*                                                                             *\n"
        "* -h, --help                          displays this quick help and exit       *\n"
        "* --IK_DEBUGLEVEL <DEBUGLEVEL>        sets debug level for IK aspect          *\n"
        "* --GLOBAL_DEBUGLEVEL <DEBUGLEVEL>    sets general debug level                *\n"
        "* --SCHEDULER_DEBUGLEVEL <DEBUGLEVEL> sets scheduler debug level              *\n"
        "* --PARSER_DEBUGLEVEL <DEBUGLEVEL>    sets parser    debug level              *\n"
        "* --MODIFIER_DEBUGLEVEL <DEBUGLEVEL>  sets modifier  debug level              *\n"
        "* <DEBUGLEVEL>                        between 0 and 6: 0 is TRACE, 1 is DEBUG,*\n"
        "*                                     2 is INFO, 3 is WARN, 5 is FATAL,       *\n"
        "*                                     6 is OFF                                *\n"
        "* --ADVANCED_SHADERS                  use advanced shader effects (shadows)   *\n"
        "*******************************************************************************\n")
    sys.exit(0)


opts=[]
ikLoggerLevel=40000 #ERROR LEVEL
globalLoggerLevel=40000 #ERROR LEVEL
schedulerLoggerLevel=40000 #ERROR LEVEL
parserLoggerLevel=40000 #ERROR LEVEL
modifierLoggerLevel=40000 #ERROR LEVEL
xmlLoggerLevel=40000 #ERROR LEVEL


try:
    opts, args = getopt.getopt(sys.argv[1:], 'hi:g:s:p:m:x:', ["ADVANCED_SHADERS", "help", "IK_DEBUGLEVEL=", "GLOBAL_DEBUGLEVEL=", "SCHEDULER_DEBUGLEVEL=", "PARSER_DEBUGLEVEL=", "MODIFIER_DEBUGLEVEL=", "XML_DEBUGLEVEL="])
except getopt.GetoptError:
    usage()
    sys.exit(2)

useAdvancedShaders = 0
    
for opt,arg in opts :
    if opt in ("-h","--help"):
        usage()
        sys.exit()
    elif opt in ("--XML_DEBUGLEVEL","-x"):
        xmlLoggerLevel = int(arg)*10000;
    elif opt in ("--MODIFIER_DEBUGLEVEL","-m"):
        modifierLoggerLevel = int(arg)*10000;
    elif opt in ("--IK_DEBUGLEVEL","-i"):
        ikLoggerLevel = int(arg)*10000;
    elif opt in ("--GLOBAL_DEBUGLEVEL","-g"):
        globalLoggerLevel = int(arg)*10000;
    elif opt in ("--SCHEDULER_DEBUGLEVEL","-s"):
        schedulerLoggerLevel = int(arg)*10000;
    elif opt in ("--PARSER_DEBUGLEVEL","-p"):
        print("arg: ",arg)
        parserLoggerLevel = int(arg)*10000;
    elif opt in ("--ADVANCED_SHADERS","-a"):
        useAdvancedShaders = 1

realizer.configureLogger(5,xmlLoggerLevel)
realizer.configureLogger(4,modifierLoggerLevel)
realizer.configureLogger(3,parserLoggerLevel)
realizer.configureLogger(2,schedulerLoggerLevel)
realizer.configureLogger(1,ikLoggerLevel)
realizer.configureLogger(0,globalLoggerLevel)

w = embr.World( realizer, useAdvancedShaders )

w.setDataPath("../../../data/")

run()              #Run the realizer
