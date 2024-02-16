import sys
import subprocess

from PyQt5.QtWidgets import QApplication, QMainWindow, QDialog, QFileDialog
from PyQt5.QtCore import QRegExp, pyqtSlot
from PyQt5.QtGui import QPixmap, QSyntaxHighlighter, QTextDocument, QTextCharFormat, QColor
from PyQt5 import Qsci 
from gui import Ui_MainWindow
from add_sphere import Ui_AddSphereDialog
class YAMLEditor(Qsci.QsciScintilla):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setLexer(Qsci.QsciLexerYAML(self))
        
class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.pushButton.clicked.connect(self.handleRenderButton)
        self.ui.actionOpen_Scene.triggered.connect(self.handleOpenScene)
        self.ui.actionSave_Scene.triggered.connect(self.handleSaveScene)
        self.ui.actionRender_Scene_2.triggered.connect(self.handleRenderButton)
        # self.ui.textEdit.textChanged.connect(self.onTextChanged)
        # self.ui.textEdit = YAMLEditor()

        # self.highlighter = ConfigHighlighter(self.ui.textEdit.document())
        
    def handleOpenScene(self):
        self.in_file_path, _ = QFileDialog.getOpenFileName(self,
            "Open Image", "./scenes", "*.yaml")
        with open(self.in_file_path, "r") as in_file:
            self.ui.textEdit.setPlainText(in_file.read())
        
    def handleRenderButton(self):

        self.handleSaveScene()
        shaderIdx = self.ui.comboBox.currentIndex()
        shader = ["ray_tracing", "normal", "depth", "scatter"][shaderIdx]
        number_iterations = self.ui.lineEdit.text()
        
        arguments = ["./ray_tracer", "-i", self.in_file_path, "-o", "./tmp/out.ppm", "-n", number_iterations]
        if shaderIdx != 0: arguments += ["-s", shader]
        subprocess.call(arguments)

        pixmap = QPixmap("./tmp/out.ppm")
        self.ui.label.setPixmap(pixmap)

    def handleAddSphere(self):
        dialog = QDialog(self)
        AddSphereDialog(dialog)
        dialog.exec()

    def handleSaveScene(self):
        with open(self.in_file_path, "w") as in_file:
            in_file.write(self.ui.textEdit.toPlainText())
        

    @pyqtSlot(str)
    def onTextChanged(self, text):
        fmt = QTextCharFormat()
        fmt.setBackground(QColor("yellow"))
        self.highlighter.clear_highlight()
        for e in text.split():
            line = int(e)
            self.highlighter.highlight_line(line, fmt)

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

STYLES = {
    'keyword': format('blue'),
    'operator': format('red'),
    'brace': format('darkGray'),
    'defclass': format('black'),
    'string': format('magenta'),
    'string2': format('darkMagenta'),
    'comment': format('darkGreen'),
    'self': format('black'),
    'numbers': format('brown'),
}

class ConfigHighlighter(QSyntaxHighlighter):
    # config keywords:
    keywords = [
        "scene", "camera", "objects",
        "look_from", "look_at",
        "vup", "vfov", "aperture",
        "aspect_ratio", "focal_length",
        "image_width", "time0", "time1",
        "type", "x0", "x1", "y0", "y1", "z0", "z1",
        "material", "color", "center", "radius", 
        "albedo", "fuzz", "refraction_index", 
        "box_min", "box_max"
    ]
    strings = [
        "xz_rectangle", "xy_rectangle", "yz_rectangle", "sphere", "box"
        "diffuse", "diffuse_light", "metal", "dielectric"
    ]
    braces = ["[", "]"]
    operators = []

    def __init__(self, parent: QTextDocument) -> None:
        super().__init__(parent)

        # Multi-line strings (expression, flag, style)
        self.tri_single = (QRegExp("'''"), 1, STYLES['string2'])
        self.tri_double = (QRegExp('"""'), 2, STYLES['string2'])

        rules = []

        # Keyword, operator, and brace rules
        rules += [(r'\b%s\b' % w, 0, STYLES['keyword'])
            for w in ConfigHighlighter.keywords]
        rules += [(r'%s' % o, 0, STYLES['operator'])
            for o in ConfigHighlighter.operators]
        rules += [(r'%s' % b, 0, STYLES['brace'])
            for b in ConfigHighlighter.braces]

        # All other rules
        rules += [
            # 'self'
            (r'\bself\b', 0, STYLES['self']),

            # 'def' followed by an identifier
            (r'\bdef\b\s*(\w+)', 1, STYLES['defclass']),
            # 'class' followed by an identifier
            (r'\bclass\b\s*(\w+)', 1, STYLES['defclass']),

            # Numeric literals
            (r'\b[+-]?[0-9]+[lL]?\b', 0, STYLES['numbers']),
            (r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b', 0, STYLES['numbers']),
            (r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b', 0, STYLES['numbers']),

            # Double-quoted string, possibly containing escape sequences
            (r'"[^"\\]*(\\.[^"\\]*)*"', 0, STYLES['string']),
            # Single-quoted string, possibly containing escape sequences
            (r"'[^'\\]*(\\.[^'\\]*)*'", 0, STYLES['string']),

            # From '#' until a newline
            (r'#[^\n]*', 0, STYLES['comment']),
        ]

        # Build a QRegExp for each pattern
        self.rules = [(QRegExp(pat), index, fmt)
            for (pat, index, fmt) in rules]

    def highlightBlock(self, text):
        """Apply syntax highlighting to the given block of text.
        """
        self.tripleQuoutesWithinStrings = []
        # Do other syntax formatting
        for expression, nth, format in self.rules:
            index = expression.indexIn(text, 0)
            if index >= 0:
                # if there is a string we check
                # if there are some triple quotes within the string
                # they will be ignored if they are matched again
                if expression.pattern() in [r'"[^"\\]*(\\.[^"\\]*)*"', r"'[^'\\]*(\\.[^'\\]*)*'"]:
                    innerIndex = self.tri_single[0].indexIn(text, index + 1)
                    if innerIndex == -1:
                        innerIndex = self.tri_double[0].indexIn(text, index + 1)

                    if innerIndex != -1:
                        tripleQuoteIndexes = range(innerIndex, innerIndex + 3)
                        self.tripleQuoutesWithinStrings.extend(tripleQuoteIndexes)

            while index >= 0:
                # skipping triple quotes within strings
                if index in self.tripleQuoutesWithinStrings:
                    index += 1
                    expression.indexIn(text, index)
                    continue

                # We actually want the index of the nth match
                index = expression.pos(nth)
                length = len(expression.cap(nth))
                self.setFormat(index, length, format)
                index = expression.indexIn(text, index + length)

        self.setCurrentBlockState(0)

        # Do multi-line strings
        in_multiline = self.match_multiline(text, *self.tri_single)
        if not in_multiline:
            in_multiline = self.match_multiline(text, *self.tri_double)

    def match_multiline(self, text, delimiter, in_state, style):
        """Do highlighting of multi-line strings. ``delimiter`` should be a
        ``QRegExp`` for triple-single-quotes or triple-double-quotes, and
        ``in_state`` should be a unique integer to represent the corresponding
        state changes when inside those strings. Returns True if we're still
        inside a multi-line string when this function is finished.
        """
        # If inside triple-single quotes, start at 0
        if self.previousBlockState() == in_state:
            start = 0
            add = 0
        # Otherwise, look for the delimiter on this line
        else:
            start = delimiter.indexIn(text)
            # skipping triple quotes within strings
            if start in self.tripleQuoutesWithinStrings:
                return False
            # Move past this match
            add = delimiter.matchedLength()

        # As long as there's a delimiter match on this line...
        while start >= 0:
            # Look for the ending delimiter
            end = delimiter.indexIn(text, start + add)
            # Ending delimiter on this line?
            if end >= add:
                length = end - start + add + delimiter.matchedLength()
                self.setCurrentBlockState(0)
            # No; multi-line string
            else:
                self.setCurrentBlockState(in_state)
                length = len(text) - start + add
            # Apply formatting
            self.setFormat(start, length, style)
            # Look for the next match
            start = delimiter.indexIn(text, start + length)

        # Return True if still inside a multi-line string, False otherwise
        if self.currentBlockState() == in_state:
            return True
        else:
            return False


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    sys.exit(app.exec_())
