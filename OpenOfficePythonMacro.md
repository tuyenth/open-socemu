# Introduction #

Directions to write a Python macro for OpenOffice 3 with an example.

This tutorial is based on a Windows machine, it could be different on a Linux machine specifically for the paths.

The example macro does the following:
  * Open a file browser to select an export file
  * Read the current Cal sheet starting for the selection row
  * Generate a SportTracks fitlog export file



# Details #

  * It is not possible to edit/debug a Python macro within OpenOffice, it has to be edited with an external editor and can only be executed in OpenOffice.  Debugging can be a little bit difficult
  * For the development, the script is created in `C:\Documents and Settings\<username>\Application Data\OpenOffice.org\3\user\Scripts\python`, let's say `SportTracks.py`
  * In the Calc document, go to `Tools->Macros->Macros->Python...`, under `My macros` there should be the `SportTracks/SportTracks.py`
  * The code of the macro is the following:

```
import uno
from com.sun.star.ui.dialogs.TemplateDescription import FILESAVE_SIMPLE
from com.sun.star.awt import Rectangle
from com.sun.star.awt import WindowDescriptor

from com.sun.star.awt.WindowClass import MODALTOP
from com.sun.star.awt.VclWindowPeerAttribute import OK, OK_CANCEL, YES_NO, YES_NO_CANCEL, RETRY_CANCEL, DEF_OK, DEF_CANCEL, DEF_RETRY, DEF_YES, DEF_NO

import time
from urllib import url2pathname


# Show a message box with the UNO based toolkit
def MessageBox(ParentWin, MsgText, MsgTitle, MsgType="messbox", MsgButtons=OK):
    
    MsgType = MsgType.lower()
    
    #available msg types
    MsgTypes = ("messbox", "infobox", "errorbox", "warningbox", "querybox")
    
    if not ( MsgType in MsgTypes ):
        MsgType = "messbox"
    
    #describe window properties.
    aDescriptor = WindowDescriptor()
    aDescriptor.Type = MODALTOP
    aDescriptor.WindowServiceName = MsgType
    aDescriptor.ParentIndex = -1
    aDescriptor.Parent = ParentWin
    #aDescriptor.Bounds = Rectangle()
    aDescriptor.WindowAttributes = MsgButtons
    
    tk = ParentWin.getToolkit()
    msgbox = tk.createWindow(aDescriptor)
    
    msgbox.setMessageText(MsgText)
    if MsgTitle :
        msgbox.setCaptionText(MsgTitle)

    return msgbox.execute()

def SportTracks( ):
    """exports a swimming track to a sporttracks fitlog file"""
    # get the doc from the scripting context which is made available to all scripts
    oDoc = XSCRIPTCONTEXT.getDocument()
    # retrieve the active sheet
    oSheet = oDoc.CurrentController.getActiveSheet()
    # retrieve the pointer to the parent window
    parentwin = oDoc.CurrentController.Frame.ContainerWindow
    
    ctx = XSCRIPTCONTEXT.getComponentContext()
    smgr = ctx.getServiceManager();

    # create the dialog model and set the properties 
    dialog = smgr.createInstanceWithContext("com.sun.star.ui.dialogs.FilePicker", ctx)

    # set the parameters of the dialog box
    dialog.initialize((FILESAVE_SIMPLE, ))
    dialog.appendFilter('SportTracks log(*.fitlog)', '*.fitlog')
    # run the dialog
    result = dialog.execute()

    # check that the user clicked OK
    if result != 1:
        return None

    # get the selected URL
    url = dialog.getFiles()[0]
    ## debug informative print
    #s = "fname = %s"%(url, )
    #MessageBox(parentwin, s, 'SportTracks macro', "infobox")
    
    # remove the leading "file:" since the latest version of url2pathname does not support it
    if url[0:5] != "file:":
        MessageBox(parentwin, 'Unexpected file preposition = '+url[0:5], 'SportTracks macro', "infobox")
        return None    
    url = url[5:]
    
    # retrieve the filename
    fname = url2pathname(url)
    
    # retrieve the current selection cells
    oSelection = oDoc.getCurrentSelection()
    if oSelection == None:
        return None
    # get the address range of the selection
    oArea = oSelection.getRangeAddress()

    # initialize counters
    nb_activity = 0
    nb_empty = 0

    fid = open(fname, 'w')
    fid.write('<?xml version="1.0" encoding="utf-8"?>\n')
    fid.write('<FitnessWorkbook xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://www.zonefivesoftware.com/xmlschemas/FitnessLogbook/v1">\n')
    fid.write('  <AthleteLog>\n')

    # loop on all the rows in the selection
    for row in range(oArea.StartRow, oArea.EndRow+1):
#        MessageBox(parentwin, oSheet.getCellByPosition( 0, row ).getString().upper(), 'test', "infobox")
#        MessageBox(parentwin, oSheet.getCellByPosition( 2, row ).getString().upper(), 'test', "infobox")
        if oSheet.getCellByPosition( 0, row ).getString().upper() == 'DATE' and oSheet.getCellByPosition( 2, row ).getString().upper() == 'NATATION':
            # increment the number of swimming activities
            nb_activity = nb_activity + 1

            # get the value of the date in the oOo format (float number of days since 01/01/1900)
            v = oSheet.getCellByPosition( 1, row ).getValue()
            # convert into Y/M/D format using python time module (using epoch time 01/01/1970) -> substract 25569 (70 years)
            activitydate = time.strftime("%Y-%m-%d", time.gmtime((int(v)-25569)*3600*24))
            s = '    <Activity StartTime="%sT12:00:00">\n'%(activitydate,)
            fid.write(s)
            # parse the condition
            if oSheet.getCellByPosition( 0, row + 1 ).getString().upper() == 'CONDITION':
                fid.write('      <Notes>%s</Notes>\n'%(oSheet.getCellByPosition( 1, row + 1 ).getString(),))
                splitrow = 3
            else:
                splitrow = 2

            # print the category
            fid.write('      <Category Name="Swimming" />\n')
            fid.write('      <Location Name="Piscine Antibes" />\n')

            # parse the time and distance
            fid.write('      <Track StartTime="%sT12:00:00">\n'%(activitydate,))
            # write the first initialization point
            fid.write('        <pt tm="0" dist="0" ele="0" />\n')

            # then parse the columns
            col = 1

            # parse the splittime row
            if oSheet.getCellByPosition( 0, row + splitrow ).getString().upper() == 'SPLIT 50M':
                # retrieve the split time
                splittime = oSheet.getCellByPosition( col, row + splitrow ).getValue()
                while splittime != 0:
                    # write the time and distance, convert the splittime in seconds
                    fid.write('        <pt tm="%s" dist="%s" ele="%s" />\n'%(str(int(splittime*3600*24)),str(50*col),str(50*col)))
                    col = col + 1
                    splittime = oSheet.getCellByPosition( col, row + splitrow ).getValue()
            
            # check if there was any track element
            if col == 1:
                nb_empty = nb_empty + 1
            fid.write('      </Track>\n')
            fid.write('    </Activity>\n')
            

    fid.write('  </AthleteLog>\n')
    fid.write('</FitnessWorkbook>\n')

    fid.close()

    # print a report
    s = "Number of activities = %d\nNumber of empty activities = %d"%(nb_activity, nb_empty)
    MessageBox(parentwin, s, 'SportTracks macro', "infobox")
    return None

g_exportedScripts = SportTracks,
```

  * Open the document file with a ZIP utility:
    * Create `Scripts/python` in the ZIP archive
    * Copy the `SportTracks.py` file in the `Scripts/python` directory inside the archive
    * Edit the `META-INF/manifest.xml` document in the archive and add the following lines before the tag `</manifest:manifest>`:

```
 <manifest:file-entry manifest:media-type="" manifest:full-path="Scripts/python/SportTracks.py"/>
 <manifest:file-entry manifest:media-type="application/binary" manifest:full-path="Scripts/python/"/>
 <manifest:file-entry manifest:media-type="application/binary" manifest:full-path="Scripts/"/>
```

  * Do not forget to enable the local macros execution in Options setting