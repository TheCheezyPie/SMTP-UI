#include "mailhistoryunit.h"
#include "Custom/Mails/ui_mailhistoryunit.h"
#include "mailhistoryunit.h"

MailHistoryUnit::MailHistoryUnit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MailHistoryUnit)
{
    ui->setupUi(this);
}

MailHistoryUnit::MailHistoryUnit(const QString& Email, const QString& Subject, const QString& LetterBody, QWidget *parent)
    : MailHistoryUnit(parent)
{
    ui->SenderEmailLabel->setText(Email);
    ui->LetterSubjectLabel->setText(Subject);
    ui->LetterBodyLabel->setText(LetterBody);
}

MailHistoryUnit::~MailHistoryUnit()
{
    delete ui;
}
