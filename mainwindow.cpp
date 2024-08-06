#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QRegularExpression>
#include "Custom/Mails/mailhistoryunit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_LogInButton_released()
{
    ui->MainPagesStack->setCurrentIndex((int)EMainPagesIndex::MainPage);
}


void MainWindow::on_AttachFileButton_released()
{
    QStringList FilePaths = QFileDialog::getOpenFileNames(nullptr, "Select Files", "", "All Files (*.*)");
    QStringList FileNames;
    if (!FilePaths.isEmpty())
    {
        for (const auto& FilePath : FilePaths)
        {
            QFileInfo FileInfo(FilePath);
            QString FileName = FileInfo.fileName();
            FileNames.append(FileName);
        }
        QString files = FileNames.join("; ");
        ui->FileNameLabel->setText(files);
    }
}

bool MainWindow::isValidEmail(const QString &email)
{
    static QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.\\w+$");

    QRegularExpressionMatch match = emailRegex.match(email);
    return match.hasMatch();
}


bool MainWindow::CheckEmails(const QLineEdit* Container)
{
    static QRegularExpression SplitRegex("\\s*,\\s*|\\s+");

    QString Recipients = Container->text();       
    QStringList EmailList = Recipients.split(SplitRegex, Qt::SkipEmptyParts);
    if (EmailList.isEmpty())
    {
        QMessageBox::critical(this, "No recipients!", "You haven't written any recipients!");
        return false;
    }

    QStringList InvalidEmails;

    for (const QString &email : EmailList)
    {
        if (!isValidEmail(email))
        {
            InvalidEmails += email;
        }
    }

    if (!InvalidEmails.isEmpty())
    {
        QString msg = R"*(Invalid emails: ")*";
        msg += InvalidEmails.join("; ");
        msg += R"*("!)*";

        QMessageBox::critical(this, "Invalid email detected!", msg);
        return false;
    }
    return true;
}

void MainWindow::on_EmailLine_editingFinished()
{
    CheckEmails(ui->EmailLine);
}


void MainWindow::on_SendButton_released()
{
    static QRegularExpression SplitRegex("\\s*,\\s*|\\s+");

    if (!CheckEmails(ui->EmailLine)) return;

    QString Recipients = ui->EmailLine->text();
    QStringList EmailList = Recipients.split(SplitRegex, Qt::SkipEmptyParts);

    QString LetterSubject = ui->SubjectLine->text();
    QString LetterBody = ui->LetterBodyText->toPlainText();

    for (const QString& Recipient : EmailList)
    {
        QDate CurrentDate = QDate::currentDate();

        QString TempFilePath = R"*(D:\SoftServe\Temp\)*";
        QString FileName = m_current_user + "_" + Recipient + "_" + CurrentDate.toString("dd-MM-yyyy");
        QString FullFileName = TempFilePath + FileName;

        QFile file(FullFileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Could not open file for writing";
            return;
        }

        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_6_7);

        LetterStruct Letter(m_current_user, Recipient, CurrentDate, LetterSubject, LetterBody);

        MailHistoryUnit* NewHistoryUnit = new MailHistoryUnit(Letter);
        ui->MailHistoryScrollArea->layout()->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        ui->MailHistoryScrollArea->layout()->addWidget(NewHistoryUnit);

        connect(NewHistoryUnit, SIGNAL(OnMouseReleased(QVector<LetterStruct>)), this, SLOT(HistoryWidgets(QVector<LetterStruct>)));

        out << Letter;

        file.close();
    }
}

void MainWindow::HistoryWidgets(QVector<LetterStruct> RelatedLetters)
{
    if (!RelatedLetters.isEmpty())
    {
        QString FullRepresentation{};
        for (int i = 0; i < RelatedLetters.size(); i++)
        {
            FullRepresentation += (QString)RelatedLetters[i];
            if (i != RelatedLetters.size() - 1)
            {
                FullRepresentation += "\n--------------------------------\n";
            }
        }
        ui->LetterHistoryText->setText(FullRepresentation);
        ui->LetterTypeStack->setCurrentIndex((int)ELetterPagesIndex::ReplyPage);
    }
}

void MainWindow::on_NewLetterButton_released()
{
    ui->LetterTypeStack->setCurrentIndex((int)ELetterPagesIndex::NewLetterPage);

    ui->EmailLine->setText("");
    ui->SubjectLine->setText("");
    ui->LetterBodyText->setText("");
}

