#ifndef DIALOGPREFERENCESNEW_H
#define DIALOGPREFERENCESNEW_H

#include "AppConfig.h"

class DialogPreferencesNew : public QDialog
{
    Q_OBJECT
public:
    DialogPreferencesNew(AppConfig *config, QWidget *parent = nullptr);
    ~DialogPreferencesNew();

private:
    AppConfig *m_pConfig;

    QStackedWidget *ui_stackedWidget;
};

#endif // DIALOGPREFERENCESNEW_H
