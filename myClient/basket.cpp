#include "basket.h"
#include "ui_basket.h"

basket::basket(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::basket)
{
    ui->setupUi(this);
    totalPrice =0;
    idList.clear();
    itemsInBasket.clear();
}

basket::~basket()
{
    delete ui;
}

void basket::addToBasket(QString item,int id, float price)
{
    itemsInBasket.append(item);
    ui->listWidget->addItem(item);
    idList.append(QString::number(id));
    totalPrice+=price;
    ui->price->setText(QString::number(totalPrice)+ " р.");
}

void basket::on_pushButton_clicked()
{
    this->close();
}
