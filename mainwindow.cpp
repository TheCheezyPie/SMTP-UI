#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QRegularExpression>

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
    ui->PagesWidget->setCurrentIndex((int)EPagesIndex::MainPage);
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

bool isValidEmail(const QString &email)
{
    static QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.\\w+$");
    QRegularExpressionMatch match = emailRegex.match(email);
    return match.hasMatch();
}

void MainWindow::on_EmailLine_editingFinished()
{
    QString Recipients = ui->EmailLine->text();
    QStringList EmailList = Recipients.split(QRegularExpression("\\s*,\\s*|\\s+"), Qt::SkipEmptyParts);
    QStringList InvalidEmails;

    for (const QString &email : EmailList)
    {
        if (isValidEmail(email))
        {
            qDebug() << "Invalid email:" << email;
            InvalidEmails += email;
        }
    }

    if (!InvalidEmails.isEmpty())
    {
        QString msg = R"*(Invalid emails: ")*";
        msg += InvalidEmails.join("; ");
        msg += R"*("!)*";

        QMessageBox::critical(this, "Invalid email detected!", msg);
    }
}

