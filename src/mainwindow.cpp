#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PreviousDirectory = QDir::homePath();
#ifdef Q_OS_WIN
    PreviousDirectory = PreviousDirectory+"/Desktop";
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}
