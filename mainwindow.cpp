#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadDirectoryContent();

    // std::unique_ptr<Poppler::Document> document = Poppler::Document::load("/home/user/Arbeitsunterlagen/Page_Vaillant.pdf");
    // if (!document) {
    //     qWarning() << "Document not loaded";
    // }

    // qreal yPos = 0;
    // QGraphicsScene *scene = new QGraphicsScene();
    // for (int i=0; i<document->numPages(); i++) {
    //     if(std::unique_ptr<Poppler::Page> page = document->page(i)) {
    //         QImage image = page->renderToImage();
    //         QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(image));
    //         item->setPos(0, yPos);
    //         yPos += image.height();

    //         page.reset();
    //     }
    // }

    // ui->graphicsView->setScene(scene);

    // qDebug() << document->numPages();
    // std::unique_ptr<Poppler::Page> page0 = document->page(0);
    // std::unique_ptr<Poppler::Page> page1 = document->page(1);
    // QImage image0 = page0->renderToImage();
    // QPixmap pix0();
    // QImage image1 = page1->renderToImage();

    // QGraphicsScene *scene = new QGraphicsScene();
    // scene->addPixmap(QPixmap::fromImage(image0));
    // scene->addPixmap(QPixmap::fromImage(image1));
    // ui->graphicsView->setScene(scene);

    // QLabel myLabel;
    // myLabel.setPixmap(QPixmap::fromImage(image));

    // myLabel.show();

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
        state_map_[item->text()] = item->checkState();
        ui->dirContentListWidget->addItem(item);
    }
}

void MainWindow::stateCheck(QListWidgetItem *item) {
    if(item->checkState() != state_map_.value(item->text())) {
        state_map_[item->text()] = item->checkState();
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
    QString dirAndName(dir_.dirName() + "/" + document_name);
    qDebug() << dirAndName;
    // std::unique_ptr<Poppler::Document> doc = Poppler::Document::load();
}

void MainWindow::loadDocumentIntoView() {

}
