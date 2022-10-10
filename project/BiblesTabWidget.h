#ifndef BIBLESTABWIDGET_H
#define BIBLESTABWIDGET_H

#include "AppConfig.h"

class BiblesTabWidget: public QTabWidget
{
    Q_OBJECT
public:
    explicit BiblesTabWidget(AppConfig &config, QWidget *parent = nullptr);
};

#endif // BIBLESTABWIDGET_H
