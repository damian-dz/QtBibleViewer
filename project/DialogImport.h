#ifndef DIALOGIMPORT_H
#define DIALOGIMPORT_H

#include "precomp.h"

class DialogImport : public QDialog
{
    Q_OBJECT
public:
    DialogImport(bool isMySword, const QString &modulePath, QWidget *parent = nullptr);
    ~DialogImport();

    const QStringList getImportedFiles() const;

private:
    void onImportButtonClicked(const QString &filename, bool isMySword, const QString &modulePath);
    void onPickFileButtonClicked(QLineEdit *filenameLineEdit, QPushButton *convertButton);

    QStringList m_importedFiles;
};


#endif // DIALOGIMPORT_H
