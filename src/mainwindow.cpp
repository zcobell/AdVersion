#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QDir>
#include <QMessageBox>

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

void MainWindow::process_err(int ierr)
{
    switch(ierr){
    case ERR_NOFILE:
        QMessageBox::critical(NULL,"ERROR","The file could not be opened.");
        break;
    case ERR_CANCELED:
        QMessageBox::critical(NULL,"ERROR","The operation was cancelled.");
        break;
    default:
        QMessageBox::critical(NULL,"ERROR","An unknown error hsa occured.");
        break;
    }
    return;
}
