import sys
import random
import string
import iSFX

from PyQt4.QtCore import QDateTime, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication
from PyQt4.QtDeclarative import QDeclarativeView

system = iSFX.System();

#class SongDummy(object):
#  filename = ""
#  def Play(self, mod):
#    print ("Play " + self.filename + " " + mod)

app = QApplication(sys.argv)

# Create the QML user interface.
view = QDeclarativeView()
view.setSource(QUrl('main.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

rootObject = view.rootObject()
#rootObject.updateSignal.connect(Update)
#sound.setPositionCallback(rootObject.updateProgress)
rootObject.updateSignal.connect(system.Update)

view.setGeometry(0, 0, rootObject.property("width"), rootObject.property("height"))
view.show()
for i in range(36):
  rootObject.addElements()

def Print(index, mouse):
  print("Clicked: " + str(index) + " " + str(mouse.property("x")) + " " + str(mouse.property("y")))

songs = []

j = 0
for c in range(10):
  if (c >= 1 and c <= 8):
    l = rootObject.getListContents(c)
    for i in l:
      j += 1
      s = object
      if j <= 13:
        if j == 1:
          s = iSFX.Sound(system, "../sounds/mario_slide.wav")
        if j == 2 or j == 3 or j == 4:
          s = iSFX.Sound(system, "../sounds/Ronald_Jenkees.mp3")
        if j == 5:                
          s = iSFX.Sound(system, "../sounds/Applause.mp3")
        if j == 6:                
          s = iSFX.Sound(system, "../sounds/CozzaFrenzy.mp3")
        if j == 7:                
          s = iSFX.Sound(system, "../sounds/BassOverhead.mp3")
        if j == 8:                
          s = iSFX.Sound(system, "../sounds/E.T.Remix.wav")
        if j == 9:                
          s = iSFX.Sound(system, "../sounds/JamieD_vs_INTOX.mp3")
        if j == 10:               
          s = iSFX.Sound(system, "../sounds/Mike_Aaron_Eddie.mp3")
        if j == 11:               
          s = iSFX.Sound(system, "../sounds/Pound4Pound.mp3")
        if j == 12:               
          s = iSFX.Sound(system, "../sounds/lightsbassnectar.mp3")
        if j == 13:               
          s = iSFX.Sound(system, "../sounds/something.xm")
        s.setPercentCallback(i.setProgress)
        #s.setNewLengthCallback(i.setProgress)
        s.setNewNameCallback(i.setName)
        s.setInStateCallback(i.setState)
        s.setNowStoppedCallback(i.nowStopped)
        i.buttonSignal.connect(s.playStop)
        i.setText(s.getName())
        songs.append(s)
  
#rootObject.setFocusList1();
  
#l2 = rootObject.getListContents2()
#for i in l2:
#  i.clicked.connect(Print)
  
#l1[0].setProperty("text", "Hello World 0")  
#l2[10].setProperty("text", "Hello World 10")

app.exec_()