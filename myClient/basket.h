#ifndef BASKET_H
#define BASKET_H

#include <QWidget>
#include <QStringList>
#include <QRegExp>
#include <QListWidgetItem>

namespace Ui {
class basket;
}

class basket : public QWidget
{
    Q_OBJECT

public:
    explicit basket(QWidget *parent = 0);
    QStringList itemsInBasket;
    QString orderData;
    float totalPrice;
    QStringList idList;
    QStringList priceList;
    QStringList supplierList;
    void clearForms();
    ~basket();
signals:
    void sendOrderData(QString);
public slots:
    void addToBasket(QString,QString,float,QString);
private slots:
    void on_pushButton_clicked();

    void on_makeOrder_clicked();

    void on_deleteButton_clicked();

    void on_listWidget_itemClicked();
private:
    Ui::basket *ui;
};

#endif // BASKET_H
