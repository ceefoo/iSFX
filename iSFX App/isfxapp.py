import sys

from PyQt4.QtCore import QDateTime, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication
from PyQt4.QtDeclarative import QDeclarativeView

import iSFX

def Foo():
  print("Click!")

class Printer(object):
  def go(self, o, i):
    o.clicked.connect(Foo)
    #print("New Sound: " + o.height + " " + i)

printer = Printer()
system = iSFX.System()
sound = iSFX.Sound(system, "../sounds/Bassnectar-CozzaFrenzy.mp3")

app = QApplication(sys.argv)

# Create the QML user interface.
view = QDeclarativeView()
view.setSource(QUrl('resources/qml/main.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

# Get the root object of the user interface.  It defines a
# 'updateSignal' signal and JavaScript 'updateMessage' function.  Both
# can be accessed transparently from Python.
rootObject = view.rootObject()

# Provide the current date and time when requested by the user interface.
rootObject.updateSignal.connect(system.Update)
rootObject.newSound.connect(printer.go)
#rootObject.updateSignal.connect(now.emit_now)

sound.setPositionCallback(rootObject.updateProgress)


# Update the user interface with the current date and time.
#now.now.connect(rootObject.updateMessage)

# Provide an initial message as a prompt.
#rootObject.updateMessage("Click to get the current date and time")

# Display the user interface and allow the user to interact with it.
view.setGeometry(0, 0, 1137, 798)
view.show()

rootObject.addSound(1, 2, "+", "NEW.mp3")
sound.play()

app.exec_()