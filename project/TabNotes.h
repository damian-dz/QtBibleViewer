#ifndef TABNOTES_H
#define TABNOTES_H

#include "AbstractTab.h"
#include "Module.h"
#include "SearchResultBrowser.h"

class TabNotes : public AbstractTab
{
    Q_OBJECT
public:
    explicit TabNotes(const QString &filename, const QList<qbv::Module>& modules, const QStringList &bookNames,
                      QWidget *parent = nullptr);

    virtual void ConnectSignals() override;
    virtual void AddControls() override;
    virtual void SetUiTexts() override;

    virtual void Initialize() override;

    void AddToNotes(qbv::Location loc);

    void SetPassage(const QString &bookName, int chapter, int verse, const QString &scripture);
    void SetPassage(const QString &bookName, int chapter, int verse1, int verse2, const QString &scripture);

signals:

public slots:


private:
    QLabel *ui_Label_References;
    QLabel *ui_Label_Passage;
    QLabel *ui_Label_Notes;

    QListWidget *ui_ListView_References;

    SearchResultBrowser *ui_PassageBrowser;
    QTextEdit *ui_TextEdit_Notes;


    QSqlDatabase m_db;
    const QString m_filename;
    const QList<qbv::Module> *m_pModules;
    const QStringList *m_pBookNames;

    QList<qbv::Location> m_locations;

    void CreateFavDatabase(const QString &filename);


    void CurrentIndexChanged(int idx);
    void SaveButtonClicked();
};

#endif // TABNOTES_H
