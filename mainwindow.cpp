#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadDirectoryContent();

    QObject::connect(ui->dirContentListWidget, &QListWidget::itemClicked,
                     this, &MainWindow::stateCheck);
    QObject::connect(this, &MainWindow::checkboxStateChanged,
                     this, &MainWindow::addToViewedItemList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadDirectoryContent() {
    QStringList entry_list = dir_.entryList(QStringList() << "*.pdf");
    for(int i=0; i<entry_list.count(); i++) {
        QListWidgetItem *item = formatItemToCheckable(entry_list[i]);
        directory_state_map_.insert(item->text(), item->checkState());
        ui->dirContentListWidget->addItem(item);
    }
}

void MainWindow::stateCheck(QListWidgetItem *item) {
    if(item->checkState() != directory_state_map_.value(item->text())) {
        directory_state_map_.insert(item->text(), item->checkState());
        emit checkboxStateChanged(item->checkState(), item->text());
    }
}

QListWidgetItem* MainWindow::formatItemToCheckable(const QString &entry) {
    QListWidgetItem *item = new QListWidgetItem(entry);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    return item;
}

void MainWindow::addToViewedItemList(Qt::CheckState state, const QString &document_name) {
    QString dirAndName(dir_.path() + "/" + document_name);

    if (state == Qt::Checked) {
        std::unique_ptr<Poppler::Document> doc = Poppler::Document::load(dirAndName);
        for (int i=0; i<doc->numPages(); i++) {
            if (std::unique_ptr<Poppler::Page> page = doc->page(i)) {
                QString hash_name(document_name + "/Page:" + QString::number(i));

                QImage image = page->renderToImage();
                viewed_doc_.insert(hash_name,image);
                page.reset();

                QListWidgetItem *listItem = formatItemToCheckable(hash_name);
                ui->viewedDocListWidget->addItem(listItem);
            }
        }
        doc.reset();
    } else {
        QList<QListWidgetItem*> itemList = ui->viewedDocListWidget->findItems(QString(document_name),
                                                                              Qt::MatchStartsWith);
        for(int i=0; i<itemList.count(); i++) {
            if(viewed_doc_.find(itemList[i]->text()) != viewed_doc_.end()) {
                QMap<QString, QImage>::iterator foundItem =
                    viewed_doc_.find(itemList[i]->text());
                viewed_doc_.erase(foundItem);
            } else {
                qWarning() << "Not found!";
            }
            int row = ui->viewedDocListWidget->row(itemList[i]);
            delete ui->viewedDocListWidget->takeItem(row);
        }
    }

    updateGraphicsViewScene();
}

void MainWindow::updateGraphicsViewScene() {
    delete ui->graphicsView->scene();
    qreal yPos = 0;
    QGraphicsScene *scene = new QGraphicsScene();
    QMap<QString, QImage>::const_iterator i = viewed_doc_.constBegin();
    while(i != viewed_doc_.constEnd()) {
        QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(i.value()));
        item->setPos(0, yPos);
        yPos += i.value().height();
        ++i;
    }
    ui->graphicsView->setScene(scene);
}
