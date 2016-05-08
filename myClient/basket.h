#ifndef BASKET_H
#define BASKET_H

#include <QWidget>
#include <QStringList>

namespace Ui {
class basket;
}

class basket : public QWidget
{
    Q_OBJECT

public:
    explicit basket(QWidget *parent = 0);
    QStringList itemsInBasket;
    float totalPrice;
    QString idList;
    ~basket();
public slots:
    void addToBasket(QString,int,float);
private slots:
    void on_pushButton_clicked();

private:
    Ui::basket *ui;
};

#endif // BASKET_H
