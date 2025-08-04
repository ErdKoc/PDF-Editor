#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadDirectoryContent();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadDirectoryContent() {
    QDir directory("/home/user/Arbeitsunterlagen");
    QStringList entry_list = directory.entryList(QStringList() << "*.pdf");
    for(int i=0; i<entry_list.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(entry_list[i]);
        qDebug() << item->text();
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        ui->listWidget->addItem(item);
    }
}
