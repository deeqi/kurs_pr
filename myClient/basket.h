#ifndef BASKET_H
#define BASKET_H

#include <QWidget>
#include <QStringList>
#include <QRegExp>

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
    ~basket();
signals:
    void sendOrderData(QString);
public slots:
    void addToBasket(QString,QString,float);
private slots:
    void on_pushButton_clicked();

    void on_makeOrder_clicked();

    void on_deleteButton_clicked();

private:
    Ui::basket *ui;
};

#endif // BASKET_H
