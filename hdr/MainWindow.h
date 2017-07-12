#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPair>
#include <QStringBuilder>
#include <QTextBrowser>
#include <QTextDocument>
#include <QtSql>

#include "HistogramForm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString appDir, QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void chapterBrowser_actionAddToFavorites();
    void chapterBrowser_actionCopy();
    void chapterBrowser_actionCopyFormatted();
    void chapterBrowser_actionSelectAll();
    void chapterBrowser_anchorClicked(const QUrl &arg1);
    void chapterBrowser_highlighted(const QUrl &arg1);
    void compareTextBrowser_actionCopy();
    void compareTextBrowser_actionSelectAll();
    void fillDictionaryEntriesWidget();
    void on_actionAbout_Qt_triggered();
    void on_actionAbout_triggered();
    void on_actionBack_triggered();
    void on_actionCopy_triggered();
    void on_actionDecrease_Font_Size_triggered();
    void on_actionEnglish_triggered();
    void on_actionExit_triggered();
    void on_actionForward_triggered();
    void on_actionIncrease_Font_Size_triggered();
    void on_actionOpen_Bible_Module_triggered();
    void on_actionPolski_triggered();
    void on_actionPreferences_triggered();
    void on_actionSearch_triggered();
    void on_actionWord_Frequency_triggered();
    void on_bookListWidget_currentRowChanged(int currentRow);
    void on_byStrongsNumberRadioButton_toggled(bool checked);
    void on_chapterListWidget_currentRowChanged(int currentRow);
    void on_compareBookListWidget_currentRowChanged(int currentRow);
    void on_compareChapterListWidget_currentRowChanged(int currentRow);
    void on_compareNextButton_clicked();
    void on_comparePrevButton_clicked();
    void on_compareTextBrowser_anchorClicked(const QUrl &arg1);
    void on_compareVerseListWidget_currentRowChanged(int currentRow);
    void on_copyButton_clicked();
    void on_definitionTextBrowser_anchorClicked(const QUrl &arg1);
    void on_deleteButton_clicked();
    void on_divisionComboBox_currentIndexChanged(int index);
    void on_enterLineEdit_returnPressed();
    void on_enterLineEdit_textChanged(const QString &arg1);
    void on_entriesListWidget_currentTextChanged(const QString &currentText);
    void on_favoritePassagesListWidget_currentRowChanged(int currentRow);
    void on_favoritePassageTextBrowser_anchorClicked(const QUrl &arg1);
    void on_nextButton_clicked();
    void on_nextChapterButton_clicked();
    void on_openDictionaryButton_clicked();
    void on_openTopicModuleButton_clicked();
    void on_prevButton_clicked();
    void on_prevChapterButton_clicked();
    void on_randomVerseButton_clicked();
    void on_randomVerseTextBrowser_anchorClicked(const QUrl &arg1);
    void on_resultsTextBrowser_anchorClicked(const QUrl &arg1);
    void on_saveButton_clicked();
    void on_searchButton_clicked();
    void on_searchDictionaryLineEdit_textEdited(const QString &arg1);
    void on_searchFromComboBox_currentIndexChanged(int index);
    void on_searchToComboBox_currentIndexChanged(int index);
    void on_tabWidget_currentChanged(int index);
    void on_translationComboBox_currentIndexChanged(int index);
    void on_translationTabWidget_currentChanged(int index);
    void on_verseFirstComboBox_currentIndexChanged(int index);
    void on_verseLastComboBox_currentIndexChanged(int index);
    void showBibleContextMenu(const QPoint& pos);
    void showCompareContextMenu(const QPoint& pos);

private:
    Ui::MainWindow *ui;
    // auxiliary structures
    struct TranslationData
    {
        QSqlDatabase database;
        QString moduleName;
        bool hasOldTestament;
        bool hasStrong;
    };
    struct TabBookChapterVerses
    {
        uchar tab;
        uchar book;
        uchar chapter;
        uchar verseFirst;
        uchar verseLast;
        friend bool operator == (const TabBookChapterVerses &x, const TabBookChapterVerses &y) {
            return (x.tab == y.tab &&
                    x.book == y.book &&
                    x.chapter == y.chapter &&
                    x.verseFirst == y.verseFirst &&
                    x.verseLast == y.verseLast);
        }
    };
    // global variables
    bool firstLoadBible;
    bool firstLoadCompare;
    bool languageChanging = false;
    bool loadFirstChapter;
    bool loadWhenBookChanged;
    bool sentByBackForward = false;
    bool sentByRandom = false;
    HistogramForm *histogramForm;
    int currentTranslationTab = 0;
    int fontSize = 10;
    int indexHistory;
    int indexStrong;
    int maxRecentPassages = 10;
    int pageNumber;
    int resultsPerPage = 25;
    QLabel *statusLabel;
    QList<QHBoxLayout *> chapterLayouts;
    QList<QStringList> globalNotes;
    QList<QTextBrowser *> chapterBrowsers;
    QList<TabBookChapterVerses> favoriteList;
    QList<TabBookChapterVerses> historyList;
    QList<TranslationData> translations;
    QPair<uchar, uchar> range;
    QRegExp displayRegex;
    QRegExp searchRegex;
    QSqlDatabase dbCnt;
    QSqlDatabase dbDct;
    QSqlDatabase dbUsr;
    QSqlDatabase dbXRef;
    QString cntxtActCopy;
    QString cntxtActCopyFormatted;
    QString cntxtActSelectAll;
    QString cntxtActAddVerse;
    QString cntxtActAddVerses;
    QString cntxtActToFavorites;
    QString confirmDeletion;
    QString areYouSure;
    QString copyMessage;
    QString currentLanguage;
    QString currentPassage;
    QString emptyMessage;
    QString enteredText;
    QString executionPath;
    QString fontFamily = "";
    QString inTotal;
    QString noMatches;
    QString openBblFilter;
    QString openCaption;
    QString openDctFilter;
    QString randomResult;
    QString resultsMessage;
    QString replyYes;
    QString replyNo;
    QString searchQueryString;
    QString settingsPath;
    QString timeElapsed;
    QString unavailable;
    QString contextActionBack;
    QString contextActionForward;
    QString verseInTotal;
    QString verses;
    QString warningCaption;
    QString entryUpdated;
    QString titleError;
    QString entryExists;
    QString containsOnlyNT;
    QStringList bookNames;
    QStringList divisionNames;
    QStringList recentVerse;
    QStringList referenceList;
    QStringList verseList;
    Qt::CaseSensitivity sensitivity;
    // methods
    QString formatScripture(QString text, bool hasStrong);
    QString formatText(QString text, bool hasStrong);
    QStringList getModuleNames(QString path);
    void addSingleTranslation(int index);
    void insertIntoFavorites();
    void addTranslationTabs();
    void changeFont(const QFont &font);
    void changeFontSize(bool increase);
    void changeLanguageToEnglish();
    void changeLanguageToPolski();
    void displayResults(int startIndex, int endIndex);
    void fillDetails();
    void getVerseRange();
    void highlightPassage(const QStringList &indices, int dbIndex);
    void loadBibleModule(const QString &moduleName);
    void loadCompareTab();
    void loadFavoritesTab();
    void loadPassage();
    void loadSettings(const QString &path, int counter = 0);
    void loadXRefAndDict(const QString &path);
    void lockCheckBoxes();
    void populateChapterMenuItemsEnglish();
    void populateChapterMenuItemsPolski();
    void populateDivisionMenuItemsEnglish();
    void populateDivisionMenuItemsPolski();
    void saveSettings();
    void searchByStrong(const QString &number);
    void searchWithLIKE(const QList<QRegExp> &wordsRgx, const QStringList &words, QString oprtr);
    void searchWithLIKE(const QRegExp &text, const QStringList &words);
    void searchWithLIKE(const QString &word, Qt::CaseSensitivity sensitivity);
    void searchWithLIKE(const QStringList &words, Qt::CaseSensitivity sensitivity, QString oprtr);
    void searchWithRegex(const QList<QRegExp> &patterns, int wordCount);
    void searchWithRegex(const QRegExp &pattern);
    void searchWithString(const QString &phrase, Qt::CaseSensitivity sensitivity);
    void searchWithString(const QStringList &words, int wordCount, Qt::CaseSensitivity sensitivity);
    void setTabBookChapterVerses(int tab, int book, int chapter, int verseFirst, int verseLast);
    void updateRangeComboBoxes();
};

#endif // MAINWINDOW_H
