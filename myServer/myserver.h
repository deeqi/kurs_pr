#ifndef MYSERVER_H
#define MYSERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include <QTime>
#include <QSqlDatabase>
#include <QSql>
#include <QSqlQuery>

namespace Ui {
class myServer;
}

class myServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit myServer(int nPort,QWidget *parent = 0);
    ~myServer();

public slots:
    virtual void slotNewConnection();
    void slotReadClient   ();

private slots:
    void on_closeButton_clicked();

private:
    void sendToClient(QTcpSocket* pSocket, const QStringList& query);
    void insertQuery(const QString& orderData);
    QString generateQuery(const QString& parameters);
    QTcpServer* server;
    quint16     nextBlockSize;

    Ui::myServer *ui;
};

#endif // MYSERVER_H
