#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileSystemWatcher>

#include <poppler-qt6.h>
#include <podofo/podofo.h>

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
    void saveDocToCurDir();
    void saveCheckedToCurDir();
    void reloadDirectoryContent();

signals:
    void checkboxStateChanged(Qt::CheckState state, const QString &document_name);

private:
    void updateGraphicsViewScene();
    void loadDirectoryContent();
    std::tuple<QString, int> splitItemName(const QListWidgetItem *item);
    QListWidgetItem* formatItemToCheckable(const QString &entry);

    Ui::MainWindow *ui;
    QFileSystemWatcher *watcher;
    QHash<QString, Qt::CheckState> directory_state_map_;
    QHash<QString, QImage> viewed_doc_;
    QList<QString> order_of_display_;
    QDir dir_ = QDir("/home/user/Arbeitsunterlagen");
};
#endif // MAINWINDOW_H
