#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <adcirc_io.h>

extern int process_a2s(QString inputFile,QString outputFile);
extern int process_s2a(QString inputFile,QString outputFile);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString PreviousDirectory;
    explicit MainWindow(QWidget *parent = 0);
    void GetLeadingPath(QString Input);
    ~MainWindow();

private slots:
    void on_browse_fromAdcirc_clicked();

    void on_browse_toSha_clicked();

    void on_browse_fromSha_clicked();

    void on_browse_toAdc_clicked();

    void on_button_toSha_clicked();

    void on_button_toAdc_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
