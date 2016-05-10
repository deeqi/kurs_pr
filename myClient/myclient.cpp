#include "myclient.h"
#include "ui_myclient.h"

MyClient::MyClient(const QString& hostName, int nPort,QWidget *parent) :
    QMainWindow(parent),nextBlockSize(0),
    ui(new Ui::MyClient)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    socket->connectToHost(hostName,nPort);
    bask = new basket();

    res = new result();
    connect(this,SIGNAL(sendQueryResult(QStringList)),res,SLOT(getQueryResult(QStringList)));
    connect(res,SIGNAL(itemToBasket(QString,QString,float)),bask,SLOT(addToBasket(QString,QString,float)));
    connect(res,SIGNAL(showBasket()),bask,SLOT(show()));
    connect(bask,SIGNAL(sendOrderData(QString)),this,SLOT(getOrderData(QString)));

    connect(socket,SIGNAL(connected()),this,SLOT(slotConnected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slotError(QAbstractSocket::SocketError)));

    //настройки интерфейса
    connect(this,SIGNAL(sendUserQuery(QString)),this,SLOT(slotSendToServer(QString)));
    //ограничения на ввод цены
    ui->lowPrice->setValidator(new QIntValidator(0, 100000, ui->lowPrice));
    ui->maxPrice->setValidator(new QIntValidator(0, 100000, ui->maxPrice));

    ui->reconnectButton->hide();
    ui->fourthArgLabel->hide();
    ui->arg1_3->hide();
    ui->arg4_1->hide();
    ui->arg4_2->hide();
    ui->arg4_3->hide();
    ui->arg4_4->hide();
    ui->arg4_5->hide();
    ui->arg4_6->hide();
    ui->arg4_7->hide();
    ui->arg4_8->hide();
}

void MyClient::clearItemMenu()                  //все чекбоксы и поля в исходное состояние
{
    ui->lowPrice->clear();
    ui->maxPrice->clear();
    ui->arg1_1->setChecked(0);
    ui->arg1_2->setChecked(0);
    ui->arg1_3->setChecked(0);

    ui->arg2_1->setChecked(0);
    ui->arg2_2->setChecked(0);
    ui->arg2_3->setChecked(0);
    ui->arg2_4->setChecked(0);
    ui->arg2_5->setChecked(0);
    ui->arg2_6->setChecked(0);

    ui->arg3_1->setChecked(0);
    ui->arg3_2->setChecked(0);
    ui->arg3_3->setChecked(0);
    ui->arg3_4->setChecked(0);

    ui->arg4_1->setChecked(0);
    ui->arg4_2->setChecked(0);
    ui->arg4_3->setChecked(0);
    ui->arg4_4->setChecked(0);
    ui->arg4_5->setChecked(0);
    ui->arg4_6->setChecked(0);
    ui->arg4_7->setChecked(0);
    ui->arg4_8->setChecked(0);
}

//ответ сервера
void MyClient::slotReadyRead()
{
    QStringList qr;
    QDataStream in(socket);
    for (;;) {
        if (!nextBlockSize) {
            if (socket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }

        if (socket->bytesAvailable() < nextBlockSize) {
            break;
        }

        in >> qr;
        nextBlockSize = 0;
    }
    emit sendQueryResult(qr);
    qr.clear();
}

void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(socket->errorString())
                    );
    ui->serverStatus->setText(strError);
    ui->reconnectButton->show();

}

void MyClient::slotSendToServer(QString query)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << QTime::currentTime() << query;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void MyClient::slotConnected()
{
    ui->serverStatus->setText("Received the connected signal");
    ui->reconnectButton->hide();
}

MyClient::~MyClient()
{
    delete ui;
}

void MyClient::on_closeButton_clicked()             //кнопка закрыть
{
    this->close();
}

void MyClient::on_itemList_currentIndexChanged(const QString &arg1)
{
    //Меняем парметры выбора в интерфейсе
    if (arg1 == "Видеокарта"){                          //Видеокарта
        clearItemMenu();
        //производитель
        ui->arg1_1->setText("nVidia GeForce");
        ui->arg1_2->setText("AMD Radeon");
        ui->arg1_3->hide();
        //бренд видеокарты
        ui->secondArgLabel->setText("Бренд");
        ui->arg2_1->setText("ASUS");
        ui->arg2_2->setText("DELL");
        ui->arg2_3->setText("GIGABYTE");
        ui->arg2_4->setText("HP");
        ui->arg2_5->setText("MSI");
        ui->arg2_6->setText("SAPPHIRE");
        ui->arg2_5->setVisible(1);
        ui->arg2_6->setVisible(1);
        //память
        ui->fourthArgLabel->setVisible(1);
        ui->arg4_1->setVisible(1);
        ui->arg4_1->setText("512 Мб");
        ui->arg4_2->setVisible(1);
        ui->arg4_2->setText("1 Гб");
        ui->arg4_3->setVisible(1);
        ui->arg4_3->setText("2 Гб");
        ui->arg4_4->setVisible(1);
        ui->arg4_4->setText("12 Гб");
        ui->arg4_5->setVisible(1);
        ui->arg4_5->setText("6 Гб");
        ui->arg4_6->setVisible(1);
        ui->arg4_6->setText("4 Гб");
        ui->arg4_7->setVisible(1);
        ui->arg4_7->setText("8 Гб");
        ui->arg4_8->setVisible(1);
        ui->arg4_8->setText("3 Гб");
        //тип памяти
        ui->thirdArgLabel->setText("Тип памяти");
        ui->arg3_1->setText("DDR3");
        ui->arg3_2->setText("DDR5");
        ui->arg3_3->setText("GDDR3");
        ui->arg3_4->setText("GDDR5");
    }
    if (arg1 == "Процессор"){                           //Процессор
        //производитель
        clearItemMenu();
        ui->arg1_2->setText("Intel");
        ui->arg1_1->setText("AMD");
        ui->arg1_3->hide();
        //ядра
        ui->secondArgLabel->setText("Количество ядер");
        ui->arg2_1->setText("2");
        ui->arg2_3->setText("6");
        ui->arg2_2->setText("4");
        ui->arg2_4->setText("8");
        ui->arg2_5->hide();
        ui->arg2_6->hide();
        //память
        ui->fourthArgLabel->hide();
        ui->arg4_1->hide();
        ui->arg4_2->hide();
        ui->arg4_3->hide();
        ui->arg4_4->hide();
        ui->arg4_5->hide();
        ui->arg4_6->hide();
        ui->arg4_7->hide();
        ui->arg4_8->hide();
        //частота
        ui->thirdArgLabel->setText("Частота");
        ui->arg3_1->setText("До 2 ГГц");
        ui->arg3_3->setText("От 2 до 3 ГГц");
        ui->arg3_2->setText("От 3 до 4 ГГц");
        ui->arg3_4->setText("От 4 ГГц");
    }
    if (arg1 == "Материнская плата"){                        //Мат. плата
        //количество слотов памяти
        clearItemMenu();
        ui->firstArgLabel->setText("Количество слотов памяти");
        ui->arg1_1->setText("2");
        ui->arg1_2->setText("4");
        ui->arg1_3->setText("8");
        ui->arg1_3->setVisible(1);
        //бренд
        ui->secondArgLabel->setText("Бренд");
        ui->arg2_1->setText("ASROCK");
        ui->arg2_2->setText("ASUS");
        ui->arg2_3->setText("GIGABYTE");
        ui->arg2_4->setText("MSI");
        ui->arg2_5->hide();
        ui->arg2_6->hide();
        //сокет
        ui->fourthArgLabel->setVisible(1);
        ui->fourthArgLabel->setText("Сокет");
        ui->arg4_1->setVisible(1);
        ui->arg4_1->setText("Celeron");
        ui->arg4_2->setVisible(1);
        ui->arg4_2->setText("LGA 1150");
        ui->arg4_3->setVisible(1);
        ui->arg4_3->setText("LGA 1151");
        ui->arg4_4->setVisible(1);
        ui->arg4_4->setText("LGA 1155");
        ui->arg4_5->setVisible(1);
        ui->arg4_5->setText("Socket FM2");
        ui->arg4_6->setVisible(1);
        ui->arg4_6->setText("Socket FM2+");
        ui->arg4_7->setVisible(1);
        ui->arg4_7->setText("Socket AM1");
        ui->arg4_8->setVisible(1);
        ui->arg4_8->setText("Socket AM3+");
        //частота
        ui->thirdArgLabel->setText("Форм-фактор");
        ui->arg3_1->setText("ATX");
        ui->arg3_2->setText("mATX");
        ui->arg3_3->setText("mini-ITX");
        ui->arg3_4->hide();
     }
    if (arg1 == "Жесткий диск"){                           //Жесткий диск
        //интерфейс
        clearItemMenu();
        ui->firstArgLabel->setText("Интерфейс");
        ui->arg1_1->setText("SATA II");
        ui->arg1_2->setText("SATA III");
        ui->arg1_3->hide();
        //бренд
        ui->secondArgLabel->setText("Бренд");
        ui->arg2_1->setText("HGST");
        ui->arg2_2->setText("HP");
        ui->arg2_3->setText("SEAGATE");
        ui->arg2_4->setText("TOSHIBA");
        ui->arg2_5->setVisible(1);
        ui->arg2_5->setText("WD");
        ui->arg2_6->hide();
        //объем памяти
        ui->fourthArgLabel->setVisible(1);
        ui->fourthArgLabel->setText("Объем накопителя");
        ui->arg4_1->setVisible(1);
        ui->arg4_1->setText("250 Гб");
        ui->arg4_2->setVisible(1);
        ui->arg4_2->setText("320 Гб");
        ui->arg4_3->setVisible(1);
        ui->arg4_3->setText("500 Гб");
        ui->arg4_4->setVisible(1);
        ui->arg4_4->setText("От 3 Тб");
        ui->arg4_5->setVisible(1);
        ui->arg4_5->setText("2 Тб");
        ui->arg4_6->setVisible(1);
        ui->arg4_6->setText("1 Тб");
        ui->arg4_7->setVisible(1);
        ui->arg4_7->setText("3 Тб");
        ui->arg4_8->setVisible(1);
        ui->arg4_8->setText("750 Гб");
        //буферная память
        ui->thirdArgLabel->setText("Буферная память");
        ui->arg3_1->setText("32 Мб");
        ui->arg3_2->setText("64 Мб");
        ui->arg3_3->setText("128 Мб");
        ui->arg3_4->setText("256 Мб");
     }
    if (arg1 == "Оперативная память"){                           //Оперативная память
        //частота
        clearItemMenu();
        ui->firstArgLabel->setText("Частота памяти");
        ui->arg1_1->setText("1333-2133 МГц");
        ui->arg1_2->setText("2133-2800 МГц");
        ui->arg1_3->setVisible(1);
        ui->arg1_3->setText("2800-3600 МГц");
        //бренд
        ui->secondArgLabel->setText("Бренд");
        ui->arg2_1->setText("AMD");
        ui->arg2_2->setText("CORSAIR");
        ui->arg2_3->setText("CRUCIAL");
        ui->arg2_4->setText("KINGSTON");
        ui->arg2_5->setVisible(1);
        ui->arg2_5->setText("PATRIOT");
        ui->arg2_6->setVisible(1);
        ui->arg2_6->setText("KINGMAX");
        //объем модуля
        ui->fourthArgLabel->setVisible(1);
        ui->fourthArgLabel->setText("Объем одного модуля");
        ui->arg4_1->setVisible(1);
        ui->arg4_1->setText("1 Гб");
        ui->arg4_2->setVisible(1);
        ui->arg4_2->setText("2 Гб");
        ui->arg4_3->setVisible(1);
        ui->arg4_3->setText("4 Гб");
        ui->arg4_4->hide();
        ui->arg4_6->setVisible(1);
        ui->arg4_6->setText("16 Гб");
        ui->arg4_5->hide();
        ui->arg4_7->hide();
        ui->arg4_8->setVisible(1);
        ui->arg4_8->setText("8 Гб");

        //тип памяти
        ui->thirdArgLabel->setText("Тип памяти");
        ui->arg3_1->setText("DDR2");
        ui->arg3_2->setText("DDR3");
        ui->arg3_3->setText("DDR3L");
        ui->arg3_4->setText("DDR4");
     }
}



void MyClient::on_sendButton_clicked()              //кнопка "выбрать"
{
    userQuery.clear();
    if (ui->itemList->currentText()=="Процессор"){
        userQuery+="cpu,";
        userQuery+=ui->lowPrice->text()+","+ui->maxPrice->text()+",";
        //производитель
        QString manufacturer;
        if (ui->arg1_1->isChecked())manufacturer+="AMD ";
        if (ui->arg1_2->isChecked())manufacturer+="Intel ";
        userQuery+=manufacturer+",";
        //ядра
        QString kernels;
        if (ui->arg2_1->isChecked())kernels+="2 ";
        if (ui->arg2_2->isChecked())kernels+="4 ";
        if (ui->arg2_3->isChecked())kernels+="6 ";
        if (ui->arg2_4->isChecked())kernels+="8 ";
        userQuery+=kernels+",";
        //частота
        QString frequency;
        if (ui->arg3_1->isChecked())frequency+="<=2000/";
        if (ui->arg3_2->isChecked())frequency+="BETWEEN 3000 AND 4000/";
        if (ui->arg3_3->isChecked())frequency+="BETWEEN 2000 AND 3000/";
        if (ui->arg3_4->isChecked())frequency+=">=4000/";
        userQuery+=frequency+",";
    }
    if (ui->itemList->currentText()=="Видеокарта"){
        userQuery+="gpu,";
        userQuery+=ui->lowPrice->text()+","+ui->maxPrice->text()+",";
        //производитель
        QString manufacturer;
        if (ui->arg1_1->isChecked())manufacturer+="nVidia GeForce/";
        if (ui->arg1_2->isChecked())manufacturer+="AMD Radeon/";
        userQuery+=manufacturer+",";
        //бренд
        QString brand;
        if (ui->arg2_1->isChecked())brand+="ASUS ";
        if (ui->arg2_2->isChecked())brand+="DELL ";
        if (ui->arg2_3->isChecked())brand+="GIGABYTE ";
        if (ui->arg2_4->isChecked())brand+="HP ";
        if (ui->arg2_5->isChecked())brand+="MSI ";
        if (ui->arg2_6->isChecked())brand+="SAPPHIRE ";
        userQuery+=brand+",";
        //тип памяти
        QString type;
        if (ui->arg3_1->isChecked())type+="DDR3 ";
        if (ui->arg3_2->isChecked())type+="DDR5 ";
        if (ui->arg3_3->isChecked())type+="GDDR3 ";
        if (ui->arg3_4->isChecked())type+="GDDR5 ";
        userQuery+=type+",";
        //память
        QString memory;
        if (ui->arg4_1->isChecked())memory+="512 ";
        if (ui->arg4_2->isChecked())memory+="1024 ";
        if (ui->arg4_3->isChecked())memory+="2048 ";
        if (ui->arg4_4->isChecked())memory+="12288 ";
        if (ui->arg4_5->isChecked())memory+="6144 ";
        if (ui->arg4_6->isChecked())memory+="4096 ";
        if (ui->arg4_7->isChecked())memory+="8192 ";
        if (ui->arg4_8->isChecked())memory+="3072 ";
        userQuery+=memory+",";

    }
    if (ui->itemList->currentText()=="Материнская плата"){
        userQuery+="mboard,";
        userQuery+=ui->lowPrice->text()+","+ui->maxPrice->text()+",";
        //слоты памяти
        QString memslots;
        if (ui->arg1_1->isChecked())memslots+="2 ";
        if (ui->arg1_2->isChecked())memslots+="4 ";
        if (ui->arg1_3->isChecked())memslots+="8 ";
        userQuery+=memslots+",";
        //бренд
        QString brand;
        if (ui->arg2_1->isChecked())brand="ASROCK ";
        if (ui->arg2_2->isChecked())brand="ASUS ";
        if (ui->arg2_3->isChecked())brand="GYGABYTE ";
        if (ui->arg2_4->isChecked())brand="MSI ";
        userQuery+=brand+",";
        //форм-фактор
        QString formfactor;
        if (ui->arg3_1->isChecked())formfactor+="ATX ";
        if (ui->arg3_2->isChecked())formfactor+="mATX ";
        if (ui->arg3_3->isChecked())formfactor+="mini-ITX ";
        userQuery+=formfactor+",";
        //сокет
        QString socket;
        if (ui->arg4_1->isChecked())socket+="Celeron/";
        if (ui->arg4_2->isChecked())socket+="LGA 1150/";
        if (ui->arg4_3->isChecked())socket+="LGA 1151/";
        if (ui->arg4_4->isChecked())socket+="LGA 1155/";
        if (ui->arg4_5->isChecked())socket+="Socket FM2/";
        if (ui->arg4_6->isChecked())socket+="Socket FM2+/";
        if (ui->arg4_7->isChecked())socket+="Socket AM1/";
        if (ui->arg4_8->isChecked())socket+="Socket AM3+/";
        userQuery+=socket+",";
    }
    if (ui->itemList->currentText()=="Жесткий диск"){
        userQuery+="hdd,";
        userQuery+=ui->lowPrice->text()+","+ui->maxPrice->text()+",";
        //интерфейс
        QString interface;
        if (ui->arg1_1->isChecked())interface+="SATA II/";
        if (ui->arg1_2->isChecked())interface+="SATA III/";
        userQuery+=interface+",";
        //бренд
        QString brand;
        if (ui->arg2_1->isChecked())brand="HGST ";
        if (ui->arg2_2->isChecked())brand="HP ";
        if (ui->arg2_3->isChecked())brand="SEAGATE ";
        if (ui->arg2_4->isChecked())brand="TOSHIBA ";
        if (ui->arg2_5->isChecked())brand="WD ";
        userQuery+=brand+",";
        //буфер
        QString bufer;
        if (ui->arg3_1->isChecked())bufer+="32 ";
        if (ui->arg3_2->isChecked())bufer+="64 ";
        if (ui->arg3_3->isChecked())bufer+="128 ";
        if (ui->arg3_4->isChecked())bufer+="256 ";
        userQuery+=bufer+",";
        //память
        QString memory;
        if (ui->arg4_1->isChecked())memory+="250 ";
        if (ui->arg4_2->isChecked())memory+="320 ";
        if (ui->arg4_3->isChecked())memory+="500 ";
        if (ui->arg4_4->isChecked())memory+="4000 ";
        if (ui->arg4_5->isChecked())memory+="2000 ";
        if (ui->arg4_6->isChecked())memory+="1000 ";
        if (ui->arg4_7->isChecked())memory+="3000 ";
        if (ui->arg4_8->isChecked())memory+="750 ";
        userQuery+=memory+",";
    }
    if (ui->itemList->currentText()=="Оперативная память"){
        userQuery+="ram,";
        userQuery+=ui->lowPrice->text()+","+ui->maxPrice->text()+",";
        //интерфейс
        QString frequency;
        if (ui->arg1_1->isChecked())frequency+="BETWEEN 1333 AND 2133/";
        if (ui->arg1_2->isChecked())frequency+="BETWEEN 2133 AND 2800/";
        if (ui->arg1_3->isChecked())frequency+="BETWEEN 2800 AND 3600/";
        userQuery+=frequency+",";
        //бренд
        QString brand;
        if (ui->arg2_1->isChecked())brand="AMD ";
        if (ui->arg2_2->isChecked())brand="CORSAIR ";
        if (ui->arg2_3->isChecked())brand="CRUCIAL ";
        if (ui->arg2_4->isChecked())brand="KINGSTON ";
        if (ui->arg2_5->isChecked())brand="PATRIOT ";
        if (ui->arg2_6->isChecked())brand="KINGMAX ";
        userQuery+=brand+",";
        //тип памяти
        QString type;
        if (ui->arg3_1->isChecked())type+="DDR2 ";
        if (ui->arg3_2->isChecked())type+="DDR3 ";
        if (ui->arg3_3->isChecked())type+="DDR3L ";
        if (ui->arg3_4->isChecked())type+="DDR4 ";
        userQuery+=type+",";
        //память
        QString memory;
        if (ui->arg4_1->isChecked())memory+="1 ";
        if (ui->arg4_2->isChecked())memory+="2 ";
        if (ui->arg4_3->isChecked())memory+="4 ";
        if (ui->arg4_8->isChecked())memory+="8 ";
        if (ui->arg4_6->isChecked())memory+="16 ";
        userQuery+=memory+",";
    }
    emit sendUserQuery(userQuery);
}

void MyClient::on_reconnectButton_clicked()         //кнопка "переподключиться"
{
    ui->serverStatus->clear();
    socket->connectToHost("localhost",1234);
}

void MyClient::getOrderData(QString orderData){
    emit sendUserQuery(orderData);
}

void MyClient::on_pushButton_clicked()
{
    bask->show();
}
