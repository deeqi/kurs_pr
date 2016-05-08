#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QDataStream>
#include <result.h>
#include <basket.h>

namespace Ui {
class MyClient;
}

class MyClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyClient(const QString& strHost, int nPort, QWidget *parent = 0);
    QTcpSocket* socket;
    basket *bask;
    result *res;
    QString userQuery;
    ~MyClient();
signals:
    void sendQueryResult(QStringList);
private:
    void clearItemMenu();
    quint16 nextBlockSize;
    Ui::MyClient *ui;
private slots:
    void slotReadyRead   ();
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected   ();
    void on_closeButton_clicked();
    void on_itemList_currentIndexChanged(const QString &arg1);
    void on_sendButton_clicked();
    void on_reconnectButton_clicked();
};

#endif // MYCLIENT_H
