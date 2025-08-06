#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void stateCheck(QListWidgetItem *Item);

signals:
    void checkboxChecked();

private:
    void loadDirectoryContent();

    Ui::MainWindow *ui;
    QMap<QString, Qt::CheckState> state_map;
};
#endif // MAINWINDOW_H
