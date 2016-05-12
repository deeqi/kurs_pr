#include "myserver.h"
#include "ui_myserver.h"

myServer::myServer(int nPort, QWidget *parent) :
    QMainWindow(parent), nextBlockSize(0),
    ui(new Ui::myServer)
{
    ui->setupUi(this);
    server = new QTcpServer(this);

    if (!server->listen(QHostAddress::Any, nPort)){
        ui->serverInfo->append("Server error");
        server->close();
        return;
    }
    QString str = QTime::currentTime().toString() + " Server started";
    ui->serverInfo->append(str);
    connect(server,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
    ui->serverInfo->setReadOnly(1);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("postgres");
    db.setUserName("postgres");
    db.setPassword("qweasdzxc123");
    db.setHostName("localhost");
    db.open();
    ui->serverInfo->append("Database "+db.databaseName()+":" + db.userName());
}

void myServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = server->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );
    ui->serverInfo->append(QTime::currentTime().toString() + " New connection");
}

void myServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    for (;;) {
        if (!nextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        ui->serverInfo->append(str);

        if(str[0]=='i'){
            insertQuery(str);
        }
        else{
        QSqlQuery query;
        query.exec(generateQuery(str));

        QStringList queryResult;
        QString value;

        while (query.next()) {
             value.clear();
             value += query.value(0).toString()+"/";
             value += query.value(1).toString()+"/";
             value += query.value(2).toString()+"/";
             value += query.value(3).toString()+"(";
             value += query.value(4).toString()+")";
             queryResult.append(value);
        }
        sendToClient(pClientSocket, queryResult);
        }
        nextBlockSize = 0;
    }
}

void myServer::sendToClient(QTcpSocket* pSocket, const QStringList& query)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << query;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}
void myServer::insertQuery(const QString &orderData)      //создание insert запроса к бд из параметров
{
    QStringList dataList;
    QSqlQuery query;
    QString clientId,orderId,dbQuery;
    int newId;

    //формирование id для клиента
    query.exec("SELECT max(clientid) FROM clients;");
    while (query.next()) {clientId = query.value(0).toString();}
    newId = clientId.toInt() + 1;
    clientId = QString::number(newId);

    //формирование id для заказа
    query.exec("SELECT max(orderid) FROM orders;");
    while (query.next()) {orderId = query.value(0).toString();}
    newId = orderId.toInt() + 1;
    orderId = QString::number(newId);

    dataList = orderData.split("/");
    dataList.removeLast();

    dbQuery = "INSERT INTO clients (clientid,fname,lname,phone,adress) VALUES ("+
            clientId+",'"+dataList[1]+"','"+dataList[2]+"','"+dataList[3]+"','"+dataList[4]+"')";
    query.exec(dbQuery);
    dbQuery.clear();

    dbQuery = "INSERT INTO orders (clientid,orderid) VALUES ("+clientId+","+orderId+")";
    query.exec(dbQuery);
    dbQuery.clear();

    dbQuery = "INSERT INTO content (orderid,clientid,detailid,supid,price,quantity) VALUES ";


    for (int i = 5; i<dataList.length();i++){
        QStringList items = dataList[i].split(",");
        QString quantity = QString::number(orderData.count(items[0]));
        if (!dbQuery.contains(items[0])){
        dbQuery+="("+orderId+","+clientId +","+items[0]+","+items[2]+","+items[1]+","+quantity+"),";
        }
        if (i==(dataList.length()-1))dbQuery.remove(dbQuery.length()-1,1);
    }
    ui->serverInfo->append(dbQuery);
    query.exec(dbQuery);
    dbQuery.clear();


    ui->serverInfo->append(clientId);
    ui->serverInfo->append(orderId);
}

QString myServer::generateQuery(const QString &parameters)      //создание select запроса к бд из параметров
{
    QString dbQuery = "SELECT details.detailid, details.title, pricelist.price, supplier.name, supplier.supid "
                      "FROM details, pricelist, supplier WHERE details.detailid IN ";
    QString dbSubQuery = "(SELECT ";
    QStringList params = parameters.split(",");
    int emptyCheck = 0;
    for (int m = 3; m<params.length();m++){
        if (!params[m].isEmpty()) emptyCheck++;
    }
    params.removeLast();

    //подзапрос для процессора
    if(params[0]=="cpu"){
        dbSubQuery+="cpuid FROM cpu ";
        if (emptyCheck>=1) dbSubQuery+="WHERE ";
        if(!params[3].isEmpty()){
            dbSubQuery+="(manufacturer = '";
            QStringList param3 = params[3].split(" ");
            if(param3.length()>1)param3.removeLast();
            for (int i=0;i<param3.length();i++){
                if (i == 0) dbSubQuery+=param3[i]+"' ";
                if (i > 0) dbSubQuery+="OR manufacturer = '"+param3[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[4].isEmpty()){
            if(params[3].isEmpty())dbSubQuery+=" (kernels = ";
            if(!params[3].isEmpty())dbSubQuery+="AND (kernels = ";
            QStringList param4 = params[4].split(" ");
            if(param4.length()>1)param4.removeLast();
            for (int i=0;i<param4.length();i++){
                if (i == 0) dbSubQuery+=param4[i]+" ";
                if (i > 0) dbSubQuery+="OR kernels = "+param4[i];
            }
            dbSubQuery+=") ";
        }
        if(!params[5].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()))dbSubQuery+=" (frequency ";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()))dbSubQuery+="AND (frequency ";
            QStringList param5 = params[5].split("/");
            if(param5.length()>1)param5.removeLast();
            for (int i=0;i<param5.length();i++){
                if (i == 0) dbSubQuery+=param5[i]+" ";
                if (i > 0) dbSubQuery+="OR frequency "+param5[i];
            }
            dbSubQuery+=") ";
        }
        dbSubQuery+=") ";
    }
    if(params[0]=="gpu"){
        dbSubQuery+="gpuid FROM gpu ";
        if (emptyCheck>=1) dbSubQuery+="WHERE ";
        if(!params[3].isEmpty()){
            dbSubQuery+="(manufacturer = '";
            QStringList param3 = params[3].split("/");
            if(param3.length()>1)param3.removeLast();
            for (int i=0;i<param3.length();i++){
                if (i == 0) dbSubQuery+=param3[i]+"' ";
                if (i > 0) dbSubQuery+="OR manufacturer = '"+param3[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[4].isEmpty()){
            if(params[3].isEmpty())dbSubQuery+=" (brand = '";
            if(!params[3].isEmpty())dbSubQuery+="AND (brand = '";
            QStringList param4 = params[4].split(" ");
            if(param4.length()>1)param4.removeLast();
            for (int i=0;i<param4.length();i++){
                if (i == 0) dbSubQuery+=param4[i]+"' ";
                if (i > 0) dbSubQuery+="OR brand = '"+param4[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[5].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()))dbSubQuery+=" (memory_type =  '";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()))dbSubQuery+="AND (memory_type = '";
            QStringList param5 = params[5].split(" ");
            if(param5.length()>1)param5.removeLast();
            for (int i=0;i<param5.length();i++){
                if (i == 0) dbSubQuery+=param5[i]+"' ";
                if (i > 0) dbSubQuery+="OR memory_type = '"+param5[i] +"' ";
            }
            dbSubQuery+=") ";
        }
         if(!params[6].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()) && (params[5].isEmpty()))dbSubQuery+=" (memory = ";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()) || (!params[5].isEmpty()))dbSubQuery+="AND (memory = ";
            QStringList param6 = params[6].split(" ");
            if(param6.length()>1)param6.removeLast();
            for (int i=0;i<param6.length();i++){
                 if (i == 0) dbSubQuery+=param6[i]+" ";
                 if (i > 0) dbSubQuery+="OR memory_type = "+param6[i] +" ";
            }
            dbSubQuery+=") ";
        }
        dbSubQuery+=") ";
        }

    if(params[0]=="mboard"){
        dbSubQuery+="mboardid FROM mboard ";
        if (emptyCheck>=1) dbSubQuery+="WHERE ";
        if(!params[3].isEmpty()){
            dbSubQuery+="(memory_slots = ";
            QStringList param3 = params[3].split(" ");
            if(param3.length()>1)param3.removeLast();
            for (int i=0;i<param3.length();i++){
                if (i == 0) dbSubQuery+=param3[i]+" ";
                if (i > 0) dbSubQuery+="OR memory_slots = "+param3[i]+" ";
            }
            dbSubQuery+=") ";
        }
        if(!params[4].isEmpty()){
            if(params[3].isEmpty())dbSubQuery+=" (brand = '";
            if(!params[3].isEmpty())dbSubQuery+="AND (brand = '";
            QStringList param4 = params[4].split(" ");
            if(param4.length()>1)param4.removeLast();
            for (int i=0;i<param4.length();i++){
                if (i == 0) dbSubQuery+=param4[i]+"' ";
                if (i > 0) dbSubQuery+="OR brand = '"+param4[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[5].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()))dbSubQuery+=" (form_factor =  '";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()))dbSubQuery+="AND (form_factor = '";
            QStringList param5 = params[5].split(" ");
            if(param5.length()>1)param5.removeLast();
            for (int i=0;i<param5.length();i++){
                if (i == 0) dbSubQuery+=param5[i]+"' ";
                if (i > 0) dbSubQuery+="OR form_factor = '"+param5[i] +"' ";
            }
            dbSubQuery+=") ";
        }
         if(!params[6].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()) && (params[5].isEmpty()))dbSubQuery+=" (socket = '";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()) || (!params[5].isEmpty()))dbSubQuery+="AND (socket = '";
            QStringList param6 = params[6].split("/");
            if(param6.length()>1)param6.removeLast();
            for (int i=0;i<param6.length();i++){
                 if (i == 0) dbSubQuery+=param6[i]+"' ";
                 if (i > 0) dbSubQuery+="OR socket = "+param6[i] +"' ";
            }
            dbSubQuery+=") ";
        }
        dbSubQuery+=") ";
        }
    if(params[0]=="hdd"){
        dbSubQuery+="hddid FROM hdd ";
        if (emptyCheck>=1) dbSubQuery+="WHERE ";
        if(!params[3].isEmpty()){
            dbSubQuery+="(interface = '";
            QStringList param3 = params[3].split("/");
            if(param3.length()>1)param3.removeLast();
            for (int i=0;i<param3.length();i++){
                if (i == 0) dbSubQuery+=param3[i]+"' ";
                if (i > 0) dbSubQuery+="OR interface = '"+param3[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[4].isEmpty()){
            if(params[3].isEmpty())dbSubQuery+=" (brand = '";
            if(!params[3].isEmpty())dbSubQuery+="AND (brand = '";
            QStringList param4 = params[4].split(" ");
            if(param4.length()>1)param4.removeLast();
            for (int i=0;i<param4.length();i++){
                if (i == 0) dbSubQuery+=param4[i]+"' ";
                if (i > 0) dbSubQuery+="OR brand = '"+param4[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[5].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()))dbSubQuery+=" (bufer_memory =  ";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()))dbSubQuery+="AND (bufer_memory = ";
            QStringList param5 = params[5].split(" ");
            if(param5.length()>1)param5.removeLast();
            for (int i=0;i<param5.length();i++){
                if (i == 0) dbSubQuery+=param5[i]+" ";
                if (i > 0) dbSubQuery+="OR bufer_memory = "+param5[i] +" ";
            }
            dbSubQuery+=") ";
        }
         if(!params[6].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()) && (params[5].isEmpty()))dbSubQuery+=" (memory = ";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()) || (!params[5].isEmpty()))dbSubQuery+="AND (memory = ";
            QStringList param6 = params[6].split(" ");
            if(param6.length()>1)param6.removeLast();
            for (int i=0;i<param6.length();i++){
                 if (i == 0) dbSubQuery+=param6[i]+" ";
                 if (i > 0) dbSubQuery+="OR memory = "+param6[i] +" ";
            }
            dbSubQuery+=") ";
        }
        dbSubQuery+=") ";
        }

    if(params[0]=="ram"){
        dbSubQuery+="ramid FROM ram ";
        if (emptyCheck>=1) dbSubQuery+="WHERE ";
        if(!params[3].isEmpty()){
            dbSubQuery+="(memory_frequency  ";
            QStringList param3 = params[3].split("/");
            if(param3.length()>1)param3.removeLast();
            for (int i=0;i<param3.length();i++){
                if (i == 0) dbSubQuery+=param3[i]+" ";
                if (i > 0) dbSubQuery+="OR memory_frequency  "+param3[i]+" ";
            }
            dbSubQuery+=") ";
        }
        if(!params[4].isEmpty()){
            if(params[3].isEmpty())dbSubQuery+=" (brand = '";
            if(!params[3].isEmpty())dbSubQuery+="AND (brand = '";
            QStringList param4 = params[4].split(" ");
            if(param4.length()>1)param4.removeLast();
            for (int i=0;i<param4.length();i++){
                if (i == 0) dbSubQuery+=param4[i]+"' ";
                if (i > 0) dbSubQuery+="OR brand = '"+param4[i]+"' ";
            }
            dbSubQuery+=") ";
        }
        if(!params[5].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()))dbSubQuery+=" (memory_type =  '";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()))dbSubQuery+="AND (memory_type = '";
            QStringList param5 = params[5].split(" ");
            if(param5.length()>1)param5.removeLast();
            for (int i=0;i<param5.length();i++){
                if (i == 0) dbSubQuery+=param5[i]+"' ";
                if (i > 0) dbSubQuery+="OR memory_type = '"+param5[i] +"' ";
            }
            dbSubQuery+=") ";
        }
         if(!params[6].isEmpty()){
            if((params[3].isEmpty()) && (params[4].isEmpty()) && (params[5].isEmpty()))dbSubQuery+=" (memory = ";
            if((!params[3].isEmpty()) || (!params[4].isEmpty()) || (!params[5].isEmpty()))dbSubQuery+="AND (memory = ";
            QStringList param6 = params[6].split(" ");
            if(param6.length()>1)param6.removeLast();
            for (int i=0;i<param6.length();i++){
                 if (i == 0) dbSubQuery+=param6[i]+" ";
                 if (i > 0) dbSubQuery+="OR memory = "+param6[i] +" ";
            }
            dbSubQuery+=") ";
        }
        dbSubQuery+=") ";
        }

    //условия на цену и получение поставщика
    dbQuery+=dbSubQuery+" ";

    dbQuery+= "and pricelist.detailid = details.detailid ";
    dbQuery+= "and pricelist.supid=supplier.supid ";
    if((!params[1].isEmpty()) && (!params[2].isEmpty()))dbQuery+=
            "and pricelist.price BETWEEN "+ params[1]+" AND "+ params[2];
    if((!params[1].isEmpty()) && (params[2].isEmpty()))dbQuery+=
            "and pricelist.price >= " + params[1];
    if((params[1].isEmpty()) && (!params[2].isEmpty()))dbQuery+=
            "and pricelist.price <= " + params[2];

    dbQuery+=";";
    return dbQuery;
}

myServer::~myServer()
{
    delete ui;
}

void myServer::on_closeButton_clicked()
{
    this->close();
}
