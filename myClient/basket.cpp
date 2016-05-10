#include "basket.h"
#include "ui_basket.h"

basket::basket(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::basket)
{
    ui->setupUi(this);

    QRegExp fnameRestrict ("[A-Za-z А-Яа-я -]{1,20}");
    QRegExp lnameRestrict ("[A-Za-z А-Яа-я -]{1,40}");
    QRegExp phoneRestrict ("[0-9 -]{1,11}");
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

void basket::addToBasket(QString item,QString id, float price)
{
    itemsInBasket.append(item);
    ui->listWidget->addItem(item);
    idList.append(id);
    totalPrice+=price;
    ui->price->setText(QString::number(totalPrice)+ " р.");
}

void basket::on_makeOrder_clicked()
{
    orderData.clear();
    orderData+="insert/";
    int errorCount = 0;
    QString errorMessage = "Неправильное заполнение полей:\n";
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
    if (ui->adress->text().isEmpty()){
        errorCount++;
        errorMessage+="Адрес ";
    }

    if (errorCount == 0){
        orderData+=ui->fname->text()+"/";
        orderData+=ui->lname->text()+"/";
        orderData+=ui->phone->text()+"/";
        orderData+=ui->adress->text()+"/";
        for (int i = 0; i<idList.length();i++){
            orderData+=idList[i]+"/";
        }

    }
    else ui->error->setText(errorMessage);
    emit sendOrderData(orderData);
}

void basket::on_pushButton_clicked()
{
    this->close();
}


void basket::on_deleteButton_clicked()
{

}
