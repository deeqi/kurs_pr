#include "result.h"
#include "ui_result.h"

result::result(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::result)
{
    ui->setupUi(this);
    itemsCount=0;
    ui->pushButton_3->setDisabled(1);
    ui->tableWidget->setSortingEnabled(1);
    ui->tableWidget->setColumnWidth(0,40);
    ui->tableWidget->setColumnWidth(1,350);
    ui->tableWidget->setColumnWidth(2,70);
    ui->tableWidget->setColumnWidth(3,100);
}

void result::getQueryResult(QStringList queryResult)
{
    this->show();
    ui->tableWidget->setRowCount(queryResult.length());
    for(int i = 0; i<queryResult.length();i++){
        QStringList oneItem = queryResult[i].split("/");
        for(int j=0; j<oneItem.length();j++){
            QTableWidgetItem *itm;
            itm = new QTableWidgetItem();
            itm->setText(oneItem[j]);
            itm->setFlags(itm->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i,j,itm);
        }
    }
}

result::~result()
{
    delete ui;
}

void result::on_pushButton_clicked()
{
    this->close();
}

void result::on_tableWidget_cellClicked(int row, int column)
{
    ui->tableWidget->selectRow(row);
}


void result::on_pushButton_2_clicked()
{
    ui->pushButton_3->setDisabled(0);
    QString item;
    QList<QTableWidgetItem*> selectedItem = ui->tableWidget->selectedItems();
    QString id; float price;
    if(selectedItem.length() != 0){
        item+=selectedItem[0]->text()+", ";
        id = selectedItem[0]->text();
        item+=selectedItem[1]->text()+", ";
        item+=selectedItem[2]->text()+" р.";
        price = selectedItem[2]->text().toFloat();
        itemsCount++;
        ui->label_2->setText("Товар добавлен в корзину");
        ui->pushButton_3->setText("Корзина ("+QString::number(itemsCount)+")");
        emit itemToBasket(item,id,price);
    }
    else ui->label_2->setText("Сначала выберите товар");

}

void result::on_pushButton_3_clicked()
{
    emit showBasket();
}
