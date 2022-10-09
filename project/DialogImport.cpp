#include "DialogImport.h"

#include "ConversionTools.h"

DialogImport::DialogImport(bool isMySword, const QString &modulePath, QWidget *parent) :
    QDialog(parent)
{
    QString srcApp = isMySword ? "MySword" : "theWord";
    QWidget::setWindowTitle("Import a Module from " + srcApp);
    QWidget::resize(600, 100);
    QWidget::setMaximumSize(800, 100);
    QWidget::setMinimumSize(500, 100);

    QLineEdit *filenameLineEdit = new QLineEdit;

    QFormLayout *mainFormLayout = new QFormLayout;

    QHBoxLayout *pickHorLayout = new QHBoxLayout;

    QPushButton *pickFileButton = new QPushButton(". . .");

    pickHorLayout->addWidget(filenameLineEdit);
    pickHorLayout->addWidget(pickFileButton);

    mainFormLayout->addRow(tr("Input File:"), pickHorLayout);

    QPushButton *convertButton = new QPushButton(tr("Import"));
    mainFormLayout->addRow(convertButton);

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    mainFormLayout->addRow(dialogButtonBox);

    setLayout(mainFormLayout);

    QObject::connect(pickFileButton, QOverload<bool>::of(&QPushButton::clicked),
                     [=] () { onPickFileButtonClicked(filenameLineEdit, convertButton); });
    QObject::connect(convertButton, QOverload<bool>::of(&QPushButton::clicked),
                     [=] () { onImportButtonClicked(filenameLineEdit->text(), isMySword, modulePath); });

    pickFileButton->setFocus();
}

void DialogImport::onPickFileButtonClicked(QLineEdit *filenameLineEdit, QPushButton *convertButton)
{
    QString filename = QFileDialog::getOpenFileName(
                this, tr("Select a MYBIBLE Module"), "/",
                tr("MYBIBLE Modules (*.bbl.mybible);;All Files (*.*)"));
    if (!filename.isEmpty()) {
        filenameLineEdit->setText(filename);
        convertButton->setFocus();
    }
}

void DialogImport::onImportButtonClicked(const QString &filename, bool isMySword, const QString &modulePath)
{
    if (QFile(filename).exists()) {
        QString importedFilename;
        if (isMySword) {
            ConversionTools::FromMySwordToQtBibleViewer(filename, modulePath, importedFilename);
        }
        m_importedFiles.append(importedFilename);
        QMessageBox::information(this, tr("Info"), tr("Import successful."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("The specified file does not exist."));
    }
}

const QStringList DialogImport::getImportedFiles() const
{
    return m_importedFiles;
}

DialogImport::~DialogImport()
{

}
