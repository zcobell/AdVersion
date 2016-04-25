#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include "AdVersion.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_button_browseInMesh_clicked();

    void on_button_browseInMesh_2_clicked();

    void on_groupBox_partition_clicked(bool checked);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString previousDirectory;
    AdVersion *versioning;

};

#endif // MAINWINDOW_H
