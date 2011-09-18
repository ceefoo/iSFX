import sys

from PyQt4.QtCore import QDateTime, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication
from PyQt4.QtDeclarative import QDeclarativeView


# This class will emit the current date and time as a signal when
# requested.
# NOT USED CURRENTLY... THIS IS FROM THE EXAMPLE
class Printer(QObject):

    #now = pyqtSignal(str)
    i = 0;
    def go(self):
      self.i += 1
      if (self.i % 67 == 0):
        print("update ", self.i)


app = QApplication(sys.argv)

printer = Printer()

# Create the QML user interface.
view = QDeclarativeView()
view.setSource(QUrl('resources/qml/main.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

# Get the root object of the user interface.  It defines a
# 'updateSignal' signal and JavaScript 'updateMessage' function.  Both
# can be accessed transparently from Python.
rootObject = view.rootObject()

# Provide the current date and time when requested by the user interface.
rootObject.updateSignal.connect(printer.go)
#rootObject.updateSignal.connect(now.emit_now)

# Update the user interface with the current date and time.
#now.now.connect(rootObject.updateMessage)

# Provide an initial message as a prompt.
#rootObject.updateMessage("Click to get the current date and time")

# Display the user interface and allow the user to interact with it.
view.setGeometry(0, 0, 1137, 798)
view.show()

app.exec_()