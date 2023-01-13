#ifndef ABSTRACTDB_H
#define ABSTRACTDB_H

namespace qbv {

class AbstractDb
{
public:
    explicit AbstractDb();
    explicit AbstractDb(const QString &filePath);
    ~AbstractDb();

    bool Open(const QString &filePath);
    void Close();
    QString FilePath() const;
    bool IsOpen() const;

protected:
    QSqlDatabase m_db;
};

}

#endif // ABSTRACTDB_H
