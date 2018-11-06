#include "PDialogStrong.h"

PDialogStrong::PDialogStrong(const QSqlDatabase &db,
                             const QString &number,
                             const QFont &font,
                             const QPixmap &background,
                             bool useBckgrnd,
                             QWidget *parent) :
    QDialog(parent)
{
    QWidget::resize(640, 480);
    QWidget::setMinimumSize(400, 300);
    QWidget::setWindowTitle(number);
    dbDct = &db;
    QVBoxLayout *mainVerLayout = new QVBoxLayout;
    mainVerLayout->setContentsMargins(5, 5, 5 ,5);
    m_defTextBrowser = new QTextBrowser;
    m_defTextBrowser->setFont(font);
    QObject::connect(m_defTextBrowser, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_definitionTextBrowser_anchorClicked(QUrl)));
    mainVerLayout->addWidget(m_defTextBrowser);
    QWidget::setLayout(mainVerLayout);
    if (useBckgrnd) {
        setBrowserBackground(background);
    }
    loadDefinition(number);
}

PDialogStrong::~PDialogStrong()
{

}

void PDialogStrong::on_definitionTextBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QString number = argString.mid(2, argString.size() - 1);
    QDialog::setWindowTitle(number);
    loadDefinition(number);
}

void PDialogStrong::loadDefinition(const QString &number)
{
    QSqlQuery query(*dbDct);
    QString queryString =  "SELECT data FROM dictionary WHERE word = '" + number + "'";
    if (query.exec(queryString)) {
        if (query.next()) {
            m_defTextBrowser->setHtml(query.record().value(0).toString());
        }
    }
}

void PDialogStrong::setBrowserBackground(const QPixmap &background)
{
    QPalette palette;
    palette.setBrush(m_defTextBrowser->viewport()->backgroundRole(), QBrush(background));
    m_defTextBrowser->viewport()->setPalette(palette);
}
