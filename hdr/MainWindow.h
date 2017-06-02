#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QCompleter>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPair>
#include <QTextBrowser>
#include <QTextDocument>
#include <QtSql>
#include <tuple>

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
    void on_actionEnglish_triggered();
    void on_actionExit_triggered();
    void on_actionOpen_Bible_Module_triggered();
    void on_actionPolski_triggered();
    void on_actionPreferences_triggered();
    void on_bookListWidget_currentRowChanged(int currentRow);
    void on_chapterListWidget_currentRowChanged(int currentRow);
    void on_copyButton_clicked();
    void on_definitionTextBrowser_anchorClicked(const QUrl &arg1);
    void on_divisionComboBox_currentIndexChanged(int index);
    void on_enterLineEdit_returnPressed();
    void on_enterLineEdit_textChanged(const QString &arg1);
    void on_nextButton_clicked();
    void on_nextChapterButton_clicked();
    void on_openDictionaryButton_clicked();
    void on_openTopicModuleButton_clicked();
    void on_prevButton_clicked();
    void on_prevChapterButton_clicked();
    void on_randomVerseButton_clicked();
    void on_randomVerseTextBrowser_anchorClicked(const QUrl &arg1);
    void on_resultsTextBrowser_anchorClicked(const QUrl &arg1);
    void on_resultsTextBrowser_highlighted(const QUrl &arg1);
    void on_searchButton_clicked();
    void on_searchDictionaryButton_clicked();
    void on_searchDictionaryLineEdit_returnPressed();
    void on_searchDictionaryLineEdit_textChanged(const QString &arg1);
    void on_searchFromComboBox_currentIndexChanged(int index);
    void on_searchToComboBox_currentIndexChanged(int index);
    void on_tabWidget_currentChanged(int index);
    void on_translationTabWidget_currentChanged(int index);
    void on_verseFirstComboBox_currentIndexChanged(int index);
    void on_verseLastComboBox_currentIndexChanged(int index);
    void chapterBrowser_anchorClicked(const QUrl &arg1);
    void chapterBrowser_highlighted(const QUrl &arg1);

private:
    Ui::MainWindow *ui;
    QCompleter *completer;
    QList<QTextBrowser*> chapterBrowsers;
    QList<QHBoxLayout*> chapterLayouts;
    QList<QStringList> globalNotes;
    QList<std::tuple<QSqlDatabase, QString, bool, bool>> databases;
    QRegExp displayRegex;
    QRegExp searchRegex;
    QSqlDatabase dbDct;
    QSqlDatabase dbXRef;
    QString copyMessage;
    QString currentLanguage;
    QString currentPassage;
    QString emptyMessage;
    QString enteredText;
    QString fontFamily = "";
    QString openBblFilter;
    QString openCaption;
    QString openDctFilter;
    QString randomResult;
    QString resultsMessage;
    QString searchQueryString;
    QString unavailable;
    QString warningCaption;
    QString timeElapsed;
    QString settingsPath;
    QString verses;
    QString inTotal;
    QString verseInTotal;
    QString noMatches;
    QStringList bookNames;
    QStringList divisionNames;
    QStringList referenceList;
    QStringList verseList;
    Qt::CaseSensitivity sensitivity;
    bool languageChanging = false;
    bool sentByRandom = false;
    int currentTranslationTab = 0;
    int fontSize = 10;
    int pageNumber;
    int resultsPerPage = 25;
    QString formatText(QString text, bool hasStrong);
    QStringList getModuleNames(QString path);
    void addSingleTranslation(int index);
    void addTranslationTabs();
    void changeLanguageToEnglish();
    void changeLanguageToPolski();
    void displayResults(int startIndex, int endIndex);
    void loadBibleModule(QString moduleName);
    void loadPassage();
    void loadXReferencesAndDict(QString path);
    void lockCheckBoxes();
    void populateChapterMenuItemsEnglish();
    void populateChapterMenuItemsPolski();
    void populateDivisionMenuItemsEnglish();
    void populateDivisionMenuItemsPolski();
    void searchWithRegex(QList<QRegExp> patterns, int wordCount);
    void searchWithRegex(QRegExp pattern);
    void searchWithString(QString word, Qt::CaseSensitivity sensitivity);
    void searchWithString(QStringList words, int wordCount, Qt::CaseSensitivity sensitivity);
    void updateRangeComboBoxes();
    void loadSettings(QString path, int counter = 0);
    void saveSettings();
    void changeFont(QFont font);
};

#endif // MAINWINDOW_H
