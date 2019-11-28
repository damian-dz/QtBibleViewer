#ifndef SEARCHOPTIONS_H
#define SEARCHOPTIONS_H

enum SearchMode
{
    exactPhrase,
    allWords,
    anyWords,
    byStrong
};

struct SearchOptions
{
    int translation;
    int bookFrom;
    int bookTo;
    bool caseSensitive;
    bool wholeWordsOnly;
    SearchMode searchMode;
};

#endif // SEARCHOPTIONS_H
