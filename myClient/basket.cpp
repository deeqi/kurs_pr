#include "basket.h"
#include "ui_basket.h"

basket::basket(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::basket)
{
    ui->setupUi(this);
    ui->deleteButton->setDisabled(1);

    QRegExp fnameRestrict ("[A-Za-z А-Яа-я ]{1,20}");
    QRegExp lnameRestrict ("[A-Za-z А-Яа-я -]{1,40}");
    QRegExp phoneRestrict ("[0-9]{1,11}");
    QRegExp adressRestrict ("[A-Za-z А-Яа-я 0-9 \\-,]{1,40}");

    ui->fname->setValidator (new QRegExpValidator (fnameRestrict, this));
    ui->lname->setValidator (new QRegExpValidator (lnameRestrict, this));
    ui->phone->setValidator (new QRegExpValidator (phoneRestrict, this));
    ui->adress->setValidator (new QRegExpValidator (adressRestrict, this));

    totalPrice=0;
    idList.clear();
    itemsInBasket.clear();
}

basket::~basket()
{
    delete ui;
}

void basket::addToBasket(QString item,QString id, float price,QString sup)
{
    ui->error->clear();
    itemsInBasket.append(item);
    ui->listWidget->addItem(item);
    idList.append(id);
    priceList.append(QString::number(price));
    supplierList.append(sup);
    totalPrice+=price;
    ui->price->setText(QString::number(totalPrice)+ " р.");
}

void basket::on_makeOrder_clicked()
{
    orderData.clear();
    orderData+="insert/";
    int errorCount = 0;
    QString errorMessage = "Неправильное заполнение полей:\n";
    QString adress;
    if (ui->fname->text().isEmpty()){
        errorCount++;
        errorMessage+="Имя ";
    }
    if (ui->lname->text().isEmpty()){
        errorCount++;
        errorMessage+="Фамилия ";
    }
    if (ui->phone->text().isEmpty()){
        errorCount++;
        errorMessage+="Телефон ";
    }
    if (ui->listWidget->count() == 0){
        errorCount++;
        ui->basketError->setText("В корзине ничего нет!");
    }

    if (errorCount == 0){
        orderData+=ui->fname->text()+"/";
        orderData+=ui->lname->text()+"/";
        orderData+=ui->phone->text()+"/";

        if (ui->adress->text().isEmpty()){
            adress = "-/";
            orderData+=adress;
        }
        else orderData+=ui->adress->text()+"/";

        for (int i = 0; i<idList.length();i++){
            orderData+=idList[i]+",";
            orderData+=priceList[i]+",";
            orderData+=supplierList[i];
            orderData+="/";
        }
        emit sendOrderData(orderData);
        ui->error->setText("Заказ оформлен!");
        clearForms();
    }
    else ui->error->setText(errorMessage);
    ui->deleteButton->setDisabled(1);
    ui->basketError->clear();
}

void basket::on_pushButton_clicked()
{
    ui->deleteButton->setDisabled(1);
    ui->error->clear();
    ui->basketError->clear();
    this->close();
}

void basket::clearForms()
{
    ui->listWidget->clear();
    ui->fname->clear();
    ui->lname->clear();
    ui->phone->clear();
    ui->adress->clear();
    ui->price->clear();
    itemsInBasket.clear();
    totalPrice = 0;
    idList.clear();
    priceList.clear();
    supplierList.clear();
}


void basket::on_deleteButton_clicked()
{
    if (ui->listWidget->count()==0){
        ui->basketError->setText("Укажите товар");
    }
    else {
        QString deletedItem = ui->listWidget->currentItem()->text();
        ui->listWidget->takeItem(ui->listWidget->currentRow());
        int index = itemsInBasket.indexOf(deletedItem);
        itemsInBasket.removeAt(index);
        idList.removeAt(index);
        totalPrice-=priceList[index].toFloat();
        ui->price->setText(QString::number(totalPrice)+" р.");
        priceList.removeAt(index);
        supplierList.removeAt(index);

        if(ui->listWidget->count()==0){
            ui->deleteButton->setDisabled(1);
        }
    }

}

void basket::on_listWidget_itemClicked()
{
    ui->deleteButton->setDisabled(0);
}
