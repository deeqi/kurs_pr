#ifndef RESULT_H
#define RESULT_H

#include <QWidget>
#include <QStringList>
#include <QTableWidgetItem>
#include <QList>

namespace Ui {
class result;
}

class result : public QWidget
{
    Q_OBJECT

public:
    explicit result(QWidget *parent = 0);
    int itemsCount;
    ~result();
signals:
    void itemToBasket(QString,QString,float);
    void showBasket();
public slots:
    void getQueryResult(QStringList);
private slots:
    void on_pushButton_clicked();
    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::result *ui;
};

#endif // RESULT_H
