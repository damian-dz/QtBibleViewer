#include "DialogInfo.h"

DialogInfo::DialogInfo(const QSqlDatabase &db,
                       QFont font,
                       const QPixmap &background,
                       bool useBckgrnd,
                       QWidget *parent)
    : QDialog(parent)
{
    QDialog::resize(720, 480);
    setWhatsThis("DialogInfo");
    QLabel *descriptionLabel = new QLabel(tr("Description:"));

    QTextBrowser *descriptionTextBrowser = new QTextBrowser;
    descriptionTextBrowser->setContextMenuPolicy(Qt::NoContextMenu);
    descriptionTextBrowser->setFont(font);
    descriptionTextBrowser->setOpenExternalLinks(false);

    QLabel *abbreviationLabel = new QLabel(tr("Abbreviation:"));

    QLineEdit *abbreviationLineEdit = new QLineEdit;
    abbreviationLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    abbreviationLineEdit->setReadOnly(true);

    QVBoxLayout *descAbbrVerLayout = new QVBoxLayout;
    descAbbrVerLayout->addWidget(descriptionLabel);
    descAbbrVerLayout->addWidget(descriptionTextBrowser);
    descAbbrVerLayout->addWidget(abbreviationLabel);
    descAbbrVerLayout->addWidget(abbreviationLineEdit);

    QLabel *commentsLabel = new QLabel(tr("Comments:"));
    
    QTextBrowser *commentsTextBrowser = new QTextBrowser;
    commentsTextBrowser->setContextMenuPolicy(Qt::NoContextMenu);
    commentsTextBrowser->setFont(font);
    commentsTextBrowser->setOpenExternalLinks(false);
    commentsTextBrowser->setOpenLinks(false);

    QVBoxLayout *commentsVerLayout = new QVBoxLayout;
    commentsVerLayout->addWidget(commentsLabel);
    commentsVerLayout->addWidget(commentsTextBrowser);

    QHBoxLayout *descAbbrCommentsHorLayout = new QHBoxLayout;
    descAbbrCommentsHorLayout->addLayout(descAbbrVerLayout);
    descAbbrCommentsHorLayout->addLayout(commentsVerLayout);
    descAbbrCommentsHorLayout->setStretchFactor(descAbbrVerLayout, 1);
    descAbbrCommentsHorLayout->setStretchFactor(commentsVerLayout, 2);

    QLabel *versionLabel = new QLabel("Version:");

    QLineEdit *versionLineEdit = new QLineEdit;
    versionLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    versionLineEdit->setReadOnly(true);

    QLabel *versionDateLabel = new QLabel(tr("Version Date:"));

    QLineEdit *versionDateLineEdit = new QLineEdit;
    versionDateLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    versionDateLineEdit->setReadOnly(true);

    QLabel *publishDateLabel = new QLabel(tr("Publish Date:"));

    QLineEdit *publishDateLineEdit = new QLineEdit;
    publishDateLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    publishDateLineEdit->setReadOnly(true);

    QGridLayout *verPubDateGridLayout = new QGridLayout;
    verPubDateGridLayout->addWidget(versionLabel, 0, 0);
    verPubDateGridLayout->addWidget(versionLineEdit, 1, 0);
    verPubDateGridLayout->addWidget(versionDateLabel, 0, 1);
    verPubDateGridLayout->addWidget(versionDateLineEdit, 1, 1);
    verPubDateGridLayout->addWidget(publishDateLabel, 0, 2);
    verPubDateGridLayout->addWidget(publishDateLineEdit, 1, 2);

    QCheckBox *rightToLeftCheckBox = new QCheckBox;
    rightToLeftCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    rightToLeftCheckBox->setFocusPolicy(Qt::NoFocus);
    rightToLeftCheckBox->setLayoutDirection(Qt::RightToLeft);
    rightToLeftCheckBox->setText(tr("Right to Left"));

    QCheckBox *oldCheckBox = new QCheckBox;
    oldCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    oldCheckBox->setFocusPolicy(Qt::NoFocus);
    oldCheckBox->setLayoutDirection(Qt::RightToLeft);
    oldCheckBox->setText(tr("Old Testament"));

    QCheckBox *newCheckBox = new QCheckBox;
    newCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    newCheckBox->setFocusPolicy(Qt::NoFocus);
    newCheckBox->setLayoutDirection(Qt::RightToLeft);
    newCheckBox->setText(tr("New Testament"));

    QCheckBox *strongCheckBox = new QCheckBox;
    strongCheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    strongCheckBox->setFocusPolicy(Qt::NoFocus);
    strongCheckBox->setLayoutDirection(Qt::RightToLeft);
    strongCheckBox->setText(tr("Strong's Numbers"));

    QHBoxLayout *rightOldNewStrongHorLayout = new QHBoxLayout;
    rightOldNewStrongHorLayout->addWidget(rightToLeftCheckBox);
    rightOldNewStrongHorLayout->addWidget(oldCheckBox);
    rightOldNewStrongHorLayout->addWidget(newCheckBox);
    rightOldNewStrongHorLayout->addWidget(strongCheckBox);

    QVBoxLayout *mainVerLayout = new QVBoxLayout(this);
    mainVerLayout->setContentsMargins(10, 10, 10, 10);
    mainVerLayout->setSpacing(5);
    mainVerLayout->addLayout(descAbbrCommentsHorLayout);
    mainVerLayout->addLayout(verPubDateGridLayout);
    mainVerLayout->addLayout(rightOldNewStrongHorLayout);

    if (useBckgrnd) {
        setBrowserBackground(*descriptionTextBrowser, background);
        setBrowserBackground(*commentsTextBrowser, background);
    }

    QString queryString = "SELECT "
                          "Description, "
                          "Abbreviation, "
                          "Comments, "
                          "Version, "
                          "VersionDate, "
                          "PublishDate, "
                          "RightToLeft, "
                          "OT, "
                          "NT, "
                          "Strong "
                          "FROM Details";
    QSqlQuery query(db);
    if (query.exec(queryString)) {
        if (query.next()) {
            QSqlRecord record = query.record();
            descriptionTextBrowser->setHtml(record.value(0).toString());
            descriptionTextBrowser->setAlignment(Qt::AlignJustify);
            abbreviationLineEdit->setText(record.value(1).toString());
            QDialog::setWindowTitle(abbreviationLineEdit->text());
            commentsTextBrowser->setHtml(record.value(2).toString());
            commentsTextBrowser->setAlignment(Qt::AlignJustify);
            versionLineEdit->setText(record.value(3).toString());
            versionDateLineEdit->setText(record.value(4).toString());
            publishDateLineEdit->setText(record.value(5).toString());
            rightToLeftCheckBox->setChecked(record.value(6).toBool());
            oldCheckBox->setChecked(record.value(7).toBool());
            newCheckBox->setChecked(record.value(8).toBool());
            strongCheckBox->setChecked(record.value(9).toBool());
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Error reading the Details table."));
    }
}

DialogInfo::~DialogInfo()
{

}

void DialogInfo::setBrowserBackground(QTextBrowser &browser, const QPixmap &background)
{
    QPalette palette;
    palette.setBrush(browser.viewport()->backgroundRole(), QBrush(background));
    browser.viewport()->setPalette(palette);
}
