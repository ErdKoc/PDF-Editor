#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsPixmapItem>
#include <QInputDialog>

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
    QObject::connect(ui->saveButton, &QPushButton::clicked,
                     this, &MainWindow::saveDocToCurDir);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QListWidgetItem* MainWindow::formatItemToCheckable(const QString &entry) {
    QListWidgetItem *item = new QListWidgetItem(entry);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    return item;
}

void MainWindow::stateCheck(QListWidgetItem *item) {
    if(item->checkState() != directory_state_map_.value(item->text())) {
        directory_state_map_.insert(item->text(), item->checkState());
        emit checkboxStateChanged(item->checkState(), item->text());
    }
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
                order_of_display_.append(hash_name);
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
                QHash<QString, QImage>::iterator foundItem =
                    viewed_doc_.find(itemList[i]->text());
                int index_of_searched_item = order_of_display_.indexOf(itemList[i]->text());
                order_of_display_.remove(index_of_searched_item);
                viewed_doc_.erase(foundItem);
            } else {
                qWarning() << "Not found!";
            }
            int row = ui->viewedDocListWidget->row(itemList[i]);
            delete ui->viewedDocListWidget->takeItem(row);
        }
    }

    if((ui->viewedDocListWidget->count() > 0) || (not ui->saveButton->isEnabled())) {
        ui->saveButton->setEnabled(true);
    } else {
        ui->saveButton->setEnabled(false);
    }
    updateGraphicsViewScene();
}

void MainWindow::saveDocToCurDir() {
    PoDoFo::PdfMemDocument inputDoc, outputDoc;
    QString file_name;
    for (int i=0; i < ui->viewedDocListWidget->count(); i++) {
        std::tuple<QString, int> doc_info = splitItemName(ui->viewedDocListWidget->item(i));

        try {
            if (file_name != std::get<0>(doc_info)) {
                inputDoc.Reset();
                QString fileDir = dir_.path() + "/" + std::get<0>(doc_info);
                inputDoc.Load(fileDir.toStdString());
                file_name = std::get<0>(doc_info);
            }

            try {
                outputDoc.GetPages().AppendDocumentPages(inputDoc, std::get<1>(doc_info), 1);
                qDebug() << "outputDoc GetPages(): " << outputDoc.GetPages().GetCount();
            }
            catch (const PoDoFo::PdfError &e) {
                qDebug() << "Fehler beim Kopieren der Seite: " << e.what();
            }
        }
        catch (const PoDoFo::PdfError &e) {
            qDebug() << "Error: " << e.what();
        }
    }

    bool ok{};
    QString save_name = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                              tr("Save as ..."), QLineEdit::Normal,
                                              QDir::home().dirName(), &ok);

    if (ok && !save_name.isEmpty()) {
        if (!save_name.contains(".pdf")) {
            save_name = save_name + ".pdf";
        }

        outputDoc.Save(QString(dir_.path() + "/" + save_name).toStdString());
    }
}

void MainWindow::updateGraphicsViewScene() {
    delete ui->graphicsView->scene();
    qreal yPos = 0;
    QGraphicsScene *scene = new QGraphicsScene();
    QList<QString>::const_iterator i = order_of_display_.constBegin();
    while(i != order_of_display_.constEnd()) {
        QImage image = viewed_doc_.value(*i);
        QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(image));
        item->setPos(0, yPos);
        yPos += image.height() + 5;
        ++i;
    }
    ui->graphicsView->setScene(scene);
}

void MainWindow::loadDirectoryContent() {
    QStringList entry_list = dir_.entryList(QStringList() << "*.pdf");
    for(int i=0; i<entry_list.count(); i++) {
        QListWidgetItem *item = formatItemToCheckable(entry_list[i]);
        directory_state_map_.insert(item->text(), item->checkState());
        ui->dirContentListWidget->addItem(item);
    }
}

std::tuple<QString, int> MainWindow::splitItemName(const QListWidgetItem *item) {
    qsizetype index_of_backshlash = item->text().indexOf("/");
    qsizetype index_of_colon = item->text().lastIndexOf(":")+1;

    QString file_name = item->text().first(index_of_backshlash);
    int page_index = item->text().sliced(index_of_colon).toInt();

    return std::make_tuple(file_name, page_index);
}
