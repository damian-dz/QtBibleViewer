#include "ConversionTools.h"

void ConversionTools::FromMySwordToQtBibleViewer(const QString &srcName, const QString &modulePath, QString &destName)
{
    QString inMemName = "inMemDb";
    {
        QSqlDatabase dbSrc = QSqlDatabase::addDatabase("QSQLITE", srcName);
        dbSrc.setDatabaseName(srcName);
        dbSrc.open();

        QSqlDatabase inMemDb = QSqlDatabase::addDatabase("QSQLITE", inMemName);
        inMemDb.setDatabaseName(":memory:");
        inMemDb.open();

        QMap<QString, QString> fieldMap;
        fieldMap.insert("Title", "Title NVARCHAR(255)");
        fieldMap.insert("Description", "Description TEXT");
        fieldMap.insert("Abbreviation", "Abbreviation NVARCHAR(50)");
        fieldMap.insert("Comments", "Comments TEXT");
        fieldMap.insert("Version", "Version TEXT");
        fieldMap.insert("VersionDate", "VersionDate DATETIME");
        fieldMap.insert("PublishDate", "PublishDate DATETIME");
        fieldMap.insert("Publisher", "Publisher TEXT");
        fieldMap.insert("Publisher", "Publisher TEXT");
        fieldMap.insert("Author", "Author TEXT");
        fieldMap.insert("Creator", "Creator TEXT");
        fieldMap.insert("Source", "Source TEXT");
        fieldMap.insert("EditorialComments", "EditorialComments TEXT");
        fieldMap.insert("Language", "Language NVARCHAR(3)");
        fieldMap.insert("RightToLeft", "RightToLeft BOOL");
        fieldMap.insert("OT", "OldTestament BOOL");
        fieldMap.insert("NT", "NewTestament BOOL");
        fieldMap.insert("Strong", "Strong BOOL");
        fieldMap.insert("VerseRules", "VerseRules TEXT");

        QStringList colNames;
        colNames << "Title" << "Description" << "Abbreviation" << "Comments" << "Version"
                 << "VersionDate" << "PublishDate" << "Publisher" << "Author" << "Creator"
                 << "Source" << "EditorialComments" << "Language" << "RightToLeft"
                 << "OT" << "NT" << "Strong" << "VerseRules";

        QStringList arguments;
        for (const QString &colName : colNames) {
            arguments << fieldMap[colName];
        }

        QSqlQuery queryDest(inMemDb);
        QString createInfoTable = QString("CREATE TABLE Info (%1)").arg(arguments.join(", "));
        queryDest.exec(createInfoTable);

        //QString destName;

        QSqlQuery querySrc(dbSrc);
        QVariantList fieldValues;
        for (const QString &colName : colNames) {
            querySrc.exec(QString("SELECT %1 FROM Details").arg(colName));
            if (querySrc.next()) {
                QVariant fieldValue = querySrc.record().value(colName);
                fieldValues << fieldValue;
                if (colName == "Abbreviation") {
                    destName = modulePath + "/" + fieldValue.toString() + ".qbv";
                }
            } else {
                fieldValues << QVariant();
            }
        }

        QStringList placeholders(QVector<QString>(fieldValues.count(), "?").toList());
        QString insertIntoInfoTable = QString("INSERT INTO Info VALUES (%1);").arg(placeholders.join(", "));
        queryDest.prepare(insertIntoInfoTable);
        for (const QVariant &fieldValue : fieldValues) {
            queryDest.addBindValue(fieldValue);
        }
        queryDest.exec();

        queryDest.exec("CREATE TABLE Bible (Id INTEGER PRIMARY KEY,"
                       "Book INTEGER,"
                       "Chapter INTEGER,"
                       "Verse INTEGER,"
                       "Scripture TEXT)");

        querySrc.exec("SELECT * FROM Bible");

        QVariantList bookNumbers;
        QVariantList chapterNumbers;
        QVariantList verseNumbers;
        QVariantList scriptures;
        while (querySrc.next()) {
            QSqlRecord record = querySrc.record();
            bookNumbers << record.value(0).toInt();
            chapterNumbers << record.value(1).toInt();
            verseNumbers << record.value(2).toInt();
            QString scripture = record.value(3).toString();
            ReplaceMySwordTags(scripture);
            scriptures << scripture;
        }

        queryDest.prepare("INSERT INTO Bible VALUES (NULL, ?, ?, ?, ?)");
        queryDest.addBindValue(bookNumbers);
        queryDest.addBindValue(chapterNumbers);
        queryDest.addBindValue(verseNumbers);
        queryDest.addBindValue(scriptures);

        queryDest.execBatch();

        queryDest.exec("CREATE UNIQUE INDEX bible_key ON Bible (Book ASC, Chapter ASC, Verse ASC)");

        if (destName.isEmpty()) {
            destName = modulePath + "/" + QFileInfo(srcName).fileName().remove(".bbl.mybible").toUpper() + ".qbv";
        }

        SaveOrLoadMemoryDb(inMemDb, destName, true);

        dbSrc.close();
        inMemDb.close();
    }
    QSqlDatabase::removeDatabase(srcName);
    QSqlDatabase::removeDatabase(inMemName);
}

void ConversionTools::ReplaceMySwordTags(QString &text)
{
    text.replace("<CM>", "");
    text.replace("<FI>", "<i>").replace("<Fi>", "</i>");
    text.replace("<FO>", "{OT}").replace("<Fo>", "{ot}");
    text.replace("<FR>", "{JW}").replace("<Fr>", "{jw}");
    //text.replace("<PI1>", "").replace("<PI2>", "").replace("<PI3>", "");
    text.replace(m_TSMySwordRgx, "{H}").replace("<Ts>", "{h}");
    text.replace(m_PIMySwordRgx, "");
    text.replace("<RF>", "{TN}").replace("<Rf>", "{tn}");
    text.replace("<CI>", "");
    text.replace("--", "—");
}

/*!
 * \brief Saves or loads an in-memory database.
 * \details Based on https://www.qtcentre.org/threads/36131-Attempting-to-use-Sqlite-backup-api-from-driver-handle-fails?p=168022#post168022
 * \param inMemDb the in-memory database
 * \param filename the destination file name
 * \param save indicates whether the database should be saved or loaded
 * \return <code>true</code> if there were no errors
 */
bool ConversionTools::SaveOrLoadMemoryDb(const QSqlDatabase &inMemDb, const QString &filename, bool save)
{
    bool isSuccess = false;
    QVariant qHandle = inMemDb.driver()->handle();
    if (qHandle.isValid() && qstrcmp(qHandle.typeName(), "sqlite3*") == 0) {
        sqlite3 *handle = *static_cast<sqlite3 **>(qHandle.data());
        if (handle != nullptr) {
            sqlite3 *pInMemory = handle;
            const char *zFilename = filename.toLocal8Bit().data();
            int retCode;              /* Function return code */
            sqlite3 *pFile;           /* Database connection opened on zFilename */
            sqlite3_backup *pBackup;  /* Backup object used to copy data */
            sqlite3 *pDbTo;           /* Database to copy to (pFile or pInMemory) */
            sqlite3 *pDbFrom;         /* Database to copy from (pFile or pInMemory) */

            /* Open the database file identified by zFilename. Exit early if this fails
            ** for any reason. */
            retCode = sqlite3_open(zFilename, &pFile);
            if (retCode == SQLITE_OK) {
                /* If this is a 'load' operation (isSave==0), then data is copied
                ** from the database file just opened to database pInMemory.
                ** Otherwise, if this is a 'save' operation (isSave==1), then data
                ** is copied from pInMemory to pFile.  Set the variables pFrom and
                ** pTo accordingly. */
                pDbFrom = save ? pInMemory : pFile;
                pDbTo = save ? pFile : pInMemory;

                /* Set up the backup procedure to copy from the "main" database of
                ** connection pFile to the main database of connection pInMemory.
                ** If something goes wrong, pBackup will be set to NULL and an error
                ** code and  message left in connection pTo.
                **
                ** If the backup object is successfully created, call backup_step()
                ** to copy data from pFile to pInMemory. Then call backup_finish()
                ** to release resources associated with the pBackup object.  If an
                ** error occurred, then  an error code and message will be left in
                ** connection pTo. If no error occurred, then the error code belonging
                ** to pTo is set to SQLITE_OK.
                */
                pBackup = sqlite3_backup_init(pDbTo, "main", pDbFrom, "main");
                if (pBackup){
                    (void)sqlite3_backup_step(pBackup, -1);
                    (void)sqlite3_backup_finish(pBackup);
                }
                retCode = sqlite3_errcode(pDbTo);
            }

            /* Close the database connection opened on database file zFilename
            ** and return the result of this function. */
            (void)sqlite3_close(pFile);

            if (retCode == SQLITE_OK) {
                isSuccess = true;
            }
        }
    }
    return isSuccess;
}
