#ifndef CONVERSIONTOOLS_H
#define CONVERSIONTOOLS_H

class ConversionTools
{
    static const inline QRegularExpression m_PIMySwordRgx= QRegularExpression("<PI[0-7]?>");
    static const inline QRegularExpression m_TSMySwordRgx= QRegularExpression("<TS[0-7]?>");
public:
    static void FromMySwordToQtBibleViewer(const QString &srcName, const QString &modulePath, QString &destName);
    static void ReplaceMySwordTags(QString &text);
    static bool SaveOrLoadMemoryDb(const QSqlDatabase &memDb, const QString &filename, bool save);
};

#endif // CONVERSIONTOOLS_H
