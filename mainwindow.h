#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QDir>

#include "poppler-qt6.h"

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
    void addToViewedItemList(Qt::CheckState state,
                             const QString &document_name);

signals:
    void checkboxStateChanged(Qt::CheckState state, const QString &document_name);

private:
    void updateGraphicsViewScene();
    void loadDirectoryContent();
    QListWidgetItem* formatItemToCheckable(const QString &entry);

    Ui::MainWindow *ui;
    QMap<QString, Qt::CheckState> directory_state_map_;
    QMap<QString, QImage> viewed_doc_;
    QDir dir_ = QDir("/home/user/Arbeitsunterlagen");
};
#endif // MAINWINDOW_H
