#ifndef MODULE_H
#define MODULE_H

/*!
 * \brief The Module struct
 */
struct Module
{
    QSqlDatabase database;
    QString name;
    QString filePath;
    bool hasOldTestament;
    bool hasStrong;
};

#endif // MODULE_H
