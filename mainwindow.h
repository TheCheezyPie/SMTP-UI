#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum class EPagesIndex : uint8_t
{
    LoginPage = 0,
    MainPage = 1
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_LogInButton_released();

    void on_AttachFileButton_released();

    void on_EmailLine_editingFinished();

    void on_SendButton_released();

private:
    Ui::MainWindow *ui;
    bool isValidEmail(const QString &email);
};
#endif // MAINWINDOW_H
