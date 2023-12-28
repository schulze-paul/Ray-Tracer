import sys
import subprocess

from PyQt5.QtWidgets import QApplication, QMainWindow, QDialog, QFileDialog
from PyQt5.QtCore import QFile
from PyQt5.QtGui import QStandardItemModel, QStandardItem, QPixmap

from gui import Ui_MainWindow
from add_sphere import Ui_AddSphereDialog

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.pushButton.clicked.connect(self.handleRenderButton)

        self.ui.model = QStandardItemModel()
        self.ui.listView.setModel(self.ui.model)
        self.ui.listView.setObjectName("listView-1")
        self.ui.model.appendRow(QStandardItem("one"))
        self.ui.model.appendRow(QStandardItem("two"))
        self.ui.model.appendRow(QStandardItem("three"))

        self.ui.actionAdd_Sphere.triggered.connect(self.handleAddSphere)
        self.ui.actionOpen_Scene.triggered.connect(self.handleOpenScene)


    def handleOpenScene(self):
        self.in_file, _ = QFileDialog.getOpenFileName(self,
            "Open Image", "./scenes", "*.yaml")
        
    def handleRenderButton(self):
        shaderIdx = self.ui.comboBox.currentIndex()
        shader = ["ray_tracing", "normal", "depth", "scatter"][shaderIdx]
        number_iterations = self.ui.lineEdit.text()
        
        arguments = ["./ray_tracer", "-i", self.in_file, "-o", "./tmp/out.ppm", "-n", number_iterations]
        if shaderIdx != 0: arguments += ["-s", shader]
        subprocess.call(arguments)

        pixmap = QPixmap("./tmp/out.ppm")
        self.ui.label.setPixmap(pixmap)

    def handleAddSphere(self):
        dialog = QDialog(self)
        AddSphereDialog(dialog)
        dialog.exec()


class AddSphereDialog(Ui_AddSphereDialog):
    """Employee dialog."""
    def __init__(self, parent=None):
        super().__init__()
        # Create an instance of the GUI
        self.ui = Ui_AddSphereDialog()
        # Run the .setupUi() method to show the GUI
        self.ui.setupUi(parent)
        self.ui.buttonBox.accepted.connect(self.accepted)


    def accepted(self):
        print("Accept data")



if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    sys.exit(app.exec_())
