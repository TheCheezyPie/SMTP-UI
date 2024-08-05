#ifndef MAILHISTORYUNIT_H
#define MAILHISTORYUNIT_H

#include <QWidget>

namespace Ui {
class MailHistoryUnit;
}

class MailHistoryUnit : public QWidget
{
    Q_OBJECT

public:
    explicit MailHistoryUnit(QWidget *parent = nullptr);
    MailHistoryUnit(const QString& Email, const QString& Subject, const QString& LetterBody, QWidget *parent = nullptr);
    ~MailHistoryUnit();

private:
    Ui::MailHistoryUnit *ui;
};

#endif // MAILHISTORYUNIT_H
