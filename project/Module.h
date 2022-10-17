#ifndef MODULE_H
#define MODULE_H

#include "Formatting.h"
#include "Location.h"
#include "PassageWithRef.h"

namespace qbv {
/*!
 * \brief The Module struct provides a convenience wrapper around the Bible module database.
 */
struct Module
{
    QSqlDatabase translation;

    /*!
     * \brief Checks if the module contains the New Testament.
     * \return <code>true</code> if the module has the New Testament.
     */
    bool hasNT() const
    {
        bool result = false;
        QSqlQuery query(translation);
        if (query.exec("SELECT Scripture FROM Bible WHERE Book = 40 AND Chapter = 1 AND Verse = 1")) {
            if (query.next()) {
                result = true;
            }
        }
        return result;
    }

    /*!
     * \brief Checks if the module contains the Old Testament.
     * \return <code>true</code> if the module has the Old Testament.
     */
    bool hasOT() const
    {
        bool result = false;
        QSqlQuery query(translation);
        if (query.exec("SELECT Scripture FROM Bible WHERE Book = 1 AND Chapter = 1 AND Verse = 1") && query.next()) {
            result = true;
        }
        return result;
    }

    /*!
     * \brief Checks if the module contains Strong's numbers.
     * \return <code>true</code> if the module has Strong's numbers.
     */
    bool hasStrong() const
    {
        bool result = false;
        QSqlQuery query(translation);
        int book = hasOT() ? 1 : 40;
        if (query.exec(QString("SELECT Scripture FROM Bible WHERE Book = %1 AND Chapter = 1 AND Verse = 1").arg(book))
                && query.next()) {
            result = query.record().value(0).toString().contains(QRegExp("<W[HG][0-9]{1,4}>"));
        }
        return result;
    }

    /*!
     * \brief Returns the file path to the original file, which is also the name of the internal database.
     * \return The file path to the original file.
     */
    QString filePath() const
    {
        return translation.databaseName();
    }

    /*!
     * \brief Returns the short name (abbreviation) for the current translation, such as KJV etc.
     * \return The abbreviated name of the translation.
     */
    QString shortName() const
    {
        QString result = "";
        QSqlQuery query(translation);
        if (query.exec("SELECT Abbreviation FROM Info") && query.next()) {
            result = query.record().value(0).toString();
        }
        return result;
    }

    QStringList GetPassage(qbv::Location loc) const
    {
        QStringList result;
        QSqlQuery query(translation);
        query.prepare("SELECT Scripture From Bible "
                      "WHERE Book=? AND Chapter=? AND Verse>=? AND Verse<=?");
        query.addBindValue(loc.book, QSql::Out);
        query.addBindValue(loc.chapter, QSql::Out);
        query.addBindValue(loc.verse1, QSql::Out);
        query.addBindValue(loc.verse2, QSql::Out);
        if (query.exec()) {
            while (query.next()) {
                QString scripture = query.record().value(0).toString();
                Formatting::FormatScriptureAndRemoveNotes(scripture);
                result << scripture;
            }
        }
        return result;
    }

    QString info() const
    {
        return "File Path: " + filePath() + "\n"
               "Short Name: " + shortName() + "\n"
               "Has Old Testament: " + (hasOT() ? "Yes" : "No" ) + "\n"
               "Has New Testament: " + (hasNT() ? "Yes" : "No" ) + "\n"
               "Has Strong: " + (hasStrong() ? "Yes" : "No" ) + "\n";
    }
};
}

#endif // MODULE_H
