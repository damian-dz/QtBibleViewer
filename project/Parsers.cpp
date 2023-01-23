#include "Parsers.h"

QList<qbv::ParsedVerseLocation> Parsers::ToVerseLocations(const QString &str)
{
    if (str.isNull() || str.isEmpty())
        throw std::invalid_argument("The input string cannot be empty.");

    const QString s = str.trimmed();

    int length = s.length();
    if (length < 3)
        throw std::invalid_argument("The input string is too short after trimming.");

    const QString num12345 = QStringLiteral("12345");
    bool isLetter = s[0].isLetter() || num12345.contains(s[0]);

    int startIdx = 0;
    int endIdx = 0;
    QString lastBookName;

    QList<qbv::ParsedVerseLocation> locations;
    bool shouldParseLocations = false;

    for (int i = 1; i < length; ++i) {
        QChar c = s[i];
        if (isLetter && !c.isLetter() && !c.isSpace()) {
           startIdx = i;
           lastBookName = str.mid(endIdx, startIdx - endIdx).trimmed();
           qDebug() << "lastBookName: " << lastBookName;
           isLetter = false;
        } else if (!isLetter && c.isLetter() || i == length - 1) {
           endIdx = i == length - 1 ? length : i;
           shouldParseLocations = true;
           isLetter = true;
        } else if (!isLetter && num12345.contains(c)
            && i > 0 && i + 1 < length && !s[i - 1].isDigit()) {
            int numberCount = CountNumbers(str.mid(startIdx, i - startIdx));
            bool canBeBookName = CanBeBookName(s, i + 1);
            if (numberCount > 1 && numberCount % 2 == 0 && canBeBookName)
            {
                endIdx = i;
                shouldParseLocations = true;
            }
        }
        if (shouldParseLocations) {
           if (endIdx > startIdx) {
               QList<int> numbers = ToNumbers(s.mid(startIdx, endIdx - startIdx));
               QList<qbv::ParsedVerseLocation> subLocations = ToSubLocations(lastBookName, numbers);
               locations.append(subLocations);
            }
           isLetter = true;
           shouldParseLocations = false;
       }
    }

    return locations;
}

int Parsers::CountNumbers(const QString &str)
{
    const QString s = str.trimmed();
    if (s.isEmpty())
        throw std::invalid_argument("The input string cannot be empty.");

    bool isDigit = s[0].isDigit();
    int count = s.count();
    if (count == 1)
        return isDigit ? 1 : 0;

    int numberCount = 0;
    for (int i = 1; i < count; ++i) {
        QChar c = s[i];
        if (isDigit && !c.isDigit()) {
            ++numberCount;
            isDigit = false;
        } else if (!isDigit && c.isDigit()) {
            isDigit = true;
        }
        if (i == count - 1 && isDigit)
            numberCount++;
    }
    return numberCount;
}

bool Parsers::CanBeBookName(const QString &str, int offset)
{
    int i = offset;
    int letterCount = 0;
    int count = str.count();
    while (i < count) {
        QChar c = str[i];
        if (c.isDigit())
           return letterCount > 0;
        else if (c.isPunct())
           return letterCount > 0;
        else if (c.isLetter())
           ++letterCount;
       ++i;
    }
    return letterCount > 0;
}

QList<int> Parsers::ToNumbers(const QString &str)
{
    const QString s = str.trimmed();
    qDebug() << "ToNumbers: " << s;
    if (s.isEmpty())
        throw std::invalid_argument("The input string cannot be empty.");

    bool isDigit = s[0].isDigit();
    int count = s.count();

    QList<int> numbers;
    if (count == 1) {
        if (isDigit) {
            int digit = QString(s[0]).toInt();
            numbers.append(digit);
        }
        return numbers;
    }

    int startIdx = 0;
    int endIdx = 0;
    for (int i = 1; i < count; ++i) {
        QChar c = s[i];
        if (isDigit && !c.isDigit()) {
            endIdx = i;
            int number = s.mid(startIdx, endIdx - startIdx).toInt();
            numbers.append(number);
            isDigit = false;
        } else if (!isDigit && c.isDigit()) {
            startIdx = i;
            isDigit = true;
        }
        if (i == count - 1)
        {
            endIdx = count;
            if (endIdx > startIdx && isDigit) {
                int number = s.mid(startIdx, endIdx - startIdx).toInt();
                numbers.append(number);
            }
        }

    }
    return numbers;

}

QList<qbv::ParsedVerseLocation> Parsers::ToSubLocations(const QString &lastBookName, const QList<int> &numbers)
{
    QList<qbv::ParsedVerseLocation> locations;
    int roundedCount = numbers.count() & ~1;
    for (int i = 0; i < roundedCount; i += 2) {
        qbv::ParsedVerseLocation location { lastBookName, numbers[i], numbers[i + 1] };
        locations.append(location);
    }
    return locations;
}
