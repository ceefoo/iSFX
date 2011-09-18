import sys

from PyQt4.QtCore import QDateTime, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication
from PyQt4.QtDeclarative import QDeclarativeView

i = 0
num = 0
def Update():
  global i
  global num
  global rootObject
  i+=1
  if (i%67 == 0):
    if (num < 6):
      num+=1
      rootObject.appendItem("Item " + str(num))

app = QApplication(sys.argv)

# Create the QML user interface.
view = QDeclarativeView()
view.setSource(QUrl('main.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

rootObject = view.rootObject()
rootObject.updateSignal.connect(Update)

#sound.setPositionCallback(rootObject.updateProgress)

#view.setGeometry(0, 0, rootObject.width, rootObject.h)
view.show()


app.exec_()