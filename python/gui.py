# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'untitled.ui'
#
# Created by: PyQt5 UI code generator 5.15.10
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1458, 1129)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout(self.centralwidget)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.frame = QtWidgets.QFrame(self.centralwidget)
        self.frame.setMinimumSize(QtCore.QSize(400, 0))
        self.frame.setMaximumSize(QtCore.QSize(400, 16777215))
        self.frame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.frame)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.scrollArea = QtWidgets.QScrollArea(self.frame)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setObjectName("scrollArea")
        self.scrollAreaWidgetContents = QtWidgets.QWidget()
        self.scrollAreaWidgetContents.setGeometry(QtCore.QRect(0, 0, 396, 1059))
        self.scrollAreaWidgetContents.setObjectName("scrollAreaWidgetContents")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.scrollAreaWidgetContents)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.textEdit = QtWidgets.QPlainTextEdit(self.scrollAreaWidgetContents)
        self.textEdit.setObjectName("textEdit")
        self.verticalLayout_2.addWidget(self.textEdit)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        self.horizontalLayout.addWidget(self.scrollArea)
        self.horizontalLayout_3.addWidget(self.frame)
        self.frame_2 = QtWidgets.QFrame(self.centralwidget)
        self.frame_2.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_2.setObjectName("frame_2")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.frame_2)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.verticalLayout_5 = QtWidgets.QVBoxLayout()
        self.verticalLayout_5.setObjectName("verticalLayout_5")
        self.frame_4 = QtWidgets.QFrame(self.frame_2)
        self.frame_4.setMaximumSize(QtCore.QSize(16777215, 30))
        self.frame_4.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.frame_4.setFrameShadow(QtWidgets.QFrame.Raised)
        self.frame_4.setObjectName("frame_4")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(self.frame_4)
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.comboBox = QtWidgets.QComboBox(self.frame_4)
        self.comboBox.setMinimumSize(QtCore.QSize(200, 0))
        self.comboBox.setObjectName("comboBox")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.horizontalLayout_2.addWidget(self.comboBox)
        self.lineEdit = QtWidgets.QLineEdit(self.frame_4)
        self.lineEdit.setInputMethodHints(QtCore.Qt.ImhDigitsOnly)
        self.lineEdit.setMaxLength(10)
        self.lineEdit.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lineEdit.setObjectName("lineEdit")
        self.horizontalLayout_2.addWidget(self.lineEdit)
        self.pushButton = QtWidgets.QPushButton(self.frame_4)
        self.pushButton.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton.setMaximumSize(QtCore.QSize(100, 16777215))
        self.pushButton.setDefault(False)
        self.pushButton.setFlat(False)
        self.pushButton.setObjectName("pushButton")
        self.horizontalLayout_2.addWidget(self.pushButton)
        self.verticalLayout_5.addWidget(self.frame_4)
        self.label = QtWidgets.QLabel(self.frame_2)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Ignored, QtWidgets.QSizePolicy.Ignored)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setInputMethodHints(QtCore.Qt.ImhDigitsOnly)
        self.label.setText("")
        self.label.setScaledContents(True)
        self.label.setObjectName("label")
        self.verticalLayout_5.addWidget(self.label)
        self.verticalLayout.addLayout(self.verticalLayout_5)
        self.horizontalLayout_3.addWidget(self.frame_2)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1458, 24))
        self.menubar.setObjectName("menubar")
        self.menuFile = QtWidgets.QMenu(self.menubar)
        self.menuFile.setObjectName("menuFile")
        self.menuAdd_Object = QtWidgets.QMenu(self.menubar)
        self.menuAdd_Object.setObjectName("menuAdd_Object")
        self.menuAdd_Material = QtWidgets.QMenu(self.menubar)
        self.menuAdd_Material.setObjectName("menuAdd_Material")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.actionOpen_Scene = QtWidgets.QAction(MainWindow)
        self.actionOpen_Scene.setObjectName("actionOpen_Scene")
        self.actionRender_Scene = QtWidgets.QAction(MainWindow)
        self.actionRender_Scene.setObjectName("actionRender_Scene")
        self.actionAdd_Sphere = QtWidgets.QAction(MainWindow)
        self.actionAdd_Sphere.setObjectName("actionAdd_Sphere")
        self.actionAdd_Rectangle = QtWidgets.QAction(MainWindow)
        self.actionAdd_Rectangle.setObjectName("actionAdd_Rectangle")
        self.actionAdd_YZ_Rectangle = QtWidgets.QAction(MainWindow)
        self.actionAdd_YZ_Rectangle.setObjectName("actionAdd_YZ_Rectangle")
        self.actionAdd_YZ_Rectangle_2 = QtWidgets.QAction(MainWindow)
        self.actionAdd_YZ_Rectangle_2.setObjectName("actionAdd_YZ_Rectangle_2")
        self.actionBox = QtWidgets.QAction(MainWindow)
        self.actionBox.setObjectName("actionBox")
        self.actionMoving_Sphere = QtWidgets.QAction(MainWindow)
        self.actionMoving_Sphere.setObjectName("actionMoving_Sphere")
        self.actionTranslation = QtWidgets.QAction(MainWindow)
        self.actionTranslation.setObjectName("actionTranslation")
        self.actionRotation = QtWidgets.QAction(MainWindow)
        self.actionRotation.setObjectName("actionRotation")
        self.actionLambertian = QtWidgets.QAction(MainWindow)
        self.actionLambertian.setObjectName("actionLambertian")
        self.actionMetak = QtWidgets.QAction(MainWindow)
        self.actionMetak.setObjectName("actionMetak")
        self.actionDielectric = QtWidgets.QAction(MainWindow)
        self.actionDielectric.setObjectName("actionDielectric")
        self.actionDiffuse_Light = QtWidgets.QAction(MainWindow)
        self.actionDiffuse_Light.setObjectName("actionDiffuse_Light")
        self.menuFile.addAction(self.actionOpen_Scene)
        self.menuFile.addAction(self.actionRender_Scene)
        self.menuAdd_Object.addAction(self.actionAdd_Sphere)
        self.menuAdd_Object.addAction(self.actionAdd_Rectangle)
        self.menuAdd_Object.addAction(self.actionAdd_YZ_Rectangle)
        self.menuAdd_Object.addAction(self.actionAdd_YZ_Rectangle_2)
        self.menuAdd_Object.addAction(self.actionBox)
        self.menuAdd_Object.addAction(self.actionMoving_Sphere)
        self.menuAdd_Object.addSeparator()
        self.menuAdd_Object.addAction(self.actionTranslation)
        self.menuAdd_Object.addAction(self.actionRotation)
        self.menuAdd_Material.addAction(self.actionLambertian)
        self.menuAdd_Material.addAction(self.actionMetak)
        self.menuAdd_Material.addAction(self.actionDielectric)
        self.menuAdd_Material.addAction(self.actionDiffuse_Light)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuAdd_Object.menuAction())
        self.menubar.addAction(self.menuAdd_Material.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.comboBox.setItemText(0, _translate("MainWindow", "Ray Tracing Shader"))
        self.comboBox.setItemText(1, _translate("MainWindow", "Normal Shader"))
        self.comboBox.setItemText(2, _translate("MainWindow", "Depth Shader"))
        self.comboBox.setItemText(3, _translate("MainWindow", "Scatter Shader"))
        self.lineEdit.setText(_translate("MainWindow", "128"))
        self.pushButton.setText(_translate("MainWindow", "Render"))
        self.menuFile.setTitle(_translate("MainWindow", "File"))
        self.menuAdd_Object.setTitle(_translate("MainWindow", "Add Object"))
        self.menuAdd_Material.setTitle(_translate("MainWindow", "Add Material"))
        self.actionOpen_Scene.setText(_translate("MainWindow", "Open Scene"))
        self.actionRender_Scene.setText(_translate("MainWindow", "Render Scene"))
        self.actionAdd_Sphere.setText(_translate("MainWindow", "Sphere"))
        self.actionAdd_Rectangle.setText(_translate("MainWindow", "XY Rectangle"))
        self.actionAdd_YZ_Rectangle.setText(_translate("MainWindow", "XZ Rectangle"))
        self.actionAdd_YZ_Rectangle_2.setText(_translate("MainWindow", "YZ Rectangle"))
        self.actionBox.setText(_translate("MainWindow", "Box"))
        self.actionMoving_Sphere.setText(_translate("MainWindow", "Moving Sphere"))
        self.actionTranslation.setText(_translate("MainWindow", "Translation"))
        self.actionRotation.setText(_translate("MainWindow", "Rotation"))
        self.actionLambertian.setText(_translate("MainWindow", "Lambertian"))
        self.actionMetak.setText(_translate("MainWindow", "Metal"))
        self.actionDielectric.setText(_translate("MainWindow", "Dielectric"))
        self.actionDiffuse_Light.setText(_translate("MainWindow", "Diffuse Light"))
