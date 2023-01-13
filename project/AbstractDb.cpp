#include "AbstractDb.h"

namespace qbv {

AbstractDb::AbstractDb()
{

}

AbstractDb::AbstractDb(const QString &filePath)
{
    Open(filePath);
}

AbstractDb::~AbstractDb()
{
    qDebug() << "~AbstactDb()";
    Close();
}

bool AbstractDb::Open(const QString &filePath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", filePath);
    m_db.setDatabaseName(filePath);
    return m_db.open();
}

void AbstractDb::Close()
{
    if (m_db.isOpen()) {
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(m_db.connectionName());
    }
}

QString AbstractDb::FilePath() const
{
    return m_db.databaseName();
}

bool AbstractDb::IsOpen() const
{
    return m_db.isOpen();
}

}
