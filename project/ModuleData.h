#ifndef MODULEDATA_H
#define MODULEDATA_H

struct ModuleData
{
    QSqlDatabase database;
    QString name;
    QString filePath;
    bool hasOldTestament;
    bool hasStrong;
};

#endif // MODULEDATA_H
