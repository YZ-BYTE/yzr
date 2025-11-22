#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void requestClose();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionOnlineTV_triggered();
    void on_actionLocalFile_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionRecorder_triggered();
    void on_actionLogin_triggered();
    void on_pb_login_clicked();
    void on_pb_register_clicked();
    void on_pb_uploadVideo_clicked();

private:
    Ui::MainWindow *ui;
    int m_userId;  // 当前登录的用户ID
    
    // 打开登录对话框的辅助函数
    void openLoginDialog(int tabIndex);
    
    // 处理登录成功，保存用户ID
    void handleLoginSuccess(int userId);
};

#endif // MAINWINDOW_H

