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

    PopulateMailsHistory();
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

void MainWindow::SpawnNewHistoryUnit(const LetterStruct& Letter)
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->MailHistoryScrollArea->layout());
    MailHistoryUnit* NewHistoryUnit = nullptr;

    if (layout)
    {
        NewHistoryUnit = new MailHistoryUnit(Letter);
        layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        layout->insertWidget(0, NewHistoryUnit);
    }

    connect(NewHistoryUnit, SIGNAL(OnMouseReleased(QVector<LetterStruct>)), this, SLOT(HistoryWidgets(QVector<LetterStruct>)));
}

void MainWindow::SpawnNewHistoryUnit(const QVector<LetterStruct>& Letters)
{
    MailHistoryUnit* NewHistoryUnit = new MailHistoryUnit(Letters);
    ui->MailHistoryScrollArea->layout()->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->MailHistoryScrollArea->layout()->addWidget(NewHistoryUnit);

    connect(NewHistoryUnit, SIGNAL(OnMouseReleased(QVector<LetterStruct>)), this, SLOT(HistoryWidgets(QVector<LetterStruct>)));
}

void MainWindow::PopulateMailsHistory()
{
    QDir dir(m_temp_file_path);

    if (!dir.exists())
    {
        qDebug() << "Directory does not exist:" << m_temp_file_path;
    }

    dir.setSorting(QDir::Time);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();

    for (const QString &fileName : files)
    {
        qDebug() << fileName;
        QVector<LetterStruct> Letters = ReadLettersFromFile(m_temp_file_path + fileName);
        if (!Letters.isEmpty()) SpawnNewHistoryUnit(Letters);
    }
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

        QString FileName = m_current_user + "_" + Recipient + "_" + CurrentDate.toString("dd-MM-yyyy") + ".txt";
        QString FullFileName = m_temp_file_path + FileName;

        LetterStruct Letter(m_current_user, Recipient, CurrentDate, LetterSubject, LetterBody);
        QVector<LetterStruct> Letters {Letter};
        SpawnNewHistoryUnit(Letter);
        WriteLettersToFile(Letters, FullFileName);
    }

    CleanNewLetterFields();
}

bool MainWindow::WriteLettersToFile(const QVector<LetterStruct>& Letters, const QString& FullFileName)
{
    QFile file(FullFileName);
    if (!file.open(QIODevice::ReadWrite))
    {
        qDebug() << "Could not open file for writing";
        return false;
    }
    if (Letters.isEmpty())
    {
        qDebug() << "Trying to write 0 letters!";
        return false;
    }

    qDebug() << "Writing to " << FullFileName << " " << Letters.size() << " letters";

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_7);

    out << Letters.size();

    for (const auto& Letter : Letters)
    {
        out << Letter;
    }

    file.close();

    return true;
}

QVector<LetterStruct> MainWindow::ReadLettersFromFile(const QString& FullFileName)
{
    QFile file(FullFileName);
    QVector<LetterStruct> Letters;

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file " << FullFileName << " for reading";
        return Letters;
    }
    if (!QFile::exists(FullFileName))
    {
        qDebug() << "The file " << FullFileName << " was not found";
        return Letters;
    }

    qDebug() << "Reading from: " << FullFileName;

    QDataStream in(&file);

    qsizetype size;
    in >> size;
    qDebug() << size;

    for (qsizetype i = 0; i < size; i++)
    {
        LetterStruct Letter;
        in >> Letter;
        Letters.append(Letter);
    }

    file.close();
    return Letters;
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

void MainWindow::CleanNewLetterFields()
{
    ui->EmailLine->setText("");
    ui->SubjectLine->setText("");
    ui->LetterBodyText->setText("");
}

void MainWindow::on_NewLetterButton_released()
{
    ui->LetterTypeStack->setCurrentIndex((int)ELetterPagesIndex::NewLetterPage);

    CleanNewLetterFields();
}


void MainWindow::on_SendReplyButton_released()
{

}

