#include "DialogStrong.h"

DialogStrong::DialogStrong(const QSqlDatabase &db,
                             const QString &number,
                             const QFont &font,
                             const QPixmap &background,
                             bool useBckgrnd,
                             QWidget *parent)
    : QDialog(parent),
      m_dbDct(&db)
{
    m_defTextBrowser = new QTextBrowser;
    m_defTextBrowser->setFont(font);
    m_defTextBrowser->setOpenLinks(false);

    QVBoxLayout *mainVerLayout = new QVBoxLayout;
    mainVerLayout->setContentsMargins(5, 5, 5 ,5);
    mainVerLayout->addWidget(m_defTextBrowser);

    if (useBckgrnd) {
        setBrowserBackground(background);
    }
    loadDefinition(number);

    QObject::connect(m_defTextBrowser, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(on_definitionTextBrowser_anchorClicked(QUrl)));

    QWidget::resize(640, 480);
    QWidget::setLayout(mainVerLayout);
    QWidget::setMinimumSize(400, 300);
    QWidget::setWindowTitle(number);
}

DialogStrong::~DialogStrong()
{

}

void DialogStrong::on_definitionTextBrowser_anchorClicked(const QUrl &arg1)
{
    QString argString = arg1.toString();
    QChar scndChar = argString[1];
    if (scndChar != 'b') {

        QString number = argString.mid(2, argString.size() - 1);
        QDialog::setWindowTitle(number);
        loadDefinition(number);
   }
}

void DialogStrong::loadDefinition(const QString &number)
{
    QSqlQuery query(*m_dbDct);
    QString queryString =  "SELECT data FROM dictionary WHERE word = '" + number + "'";
    if (query.exec(queryString)) {
        if (query.next()) {
            m_defTextBrowser->setHtml(query.record().value(0).toString());
        }
    }
}

void DialogStrong::setBrowserBackground(const QPixmap &background)
{
    QPalette palette;
    palette.setBrush(m_defTextBrowser->viewport()->backgroundRole(), QBrush(background));
    m_defTextBrowser->viewport()->setPalette(palette);
}
