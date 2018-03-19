#include "mytcpsocket.h"

MyTcpSocket::MyTcpSocket(QObject* parent):QTcpSocket(parent)
{
    myThread.start();
    moveToThread(&myThread); // переносим его в поток QThreadObject
    //Изменяет сходство потоков для этого объекта и его дочерних элементов.
    //Объект нельзя перемещать, если он имеет родителя. Обработка событий будет продолжена в targetThread.
}

void MyTcpSocket::setPeerAddressMy(QHostAddress & addrHost, int & nPort)
{
     addrTcpThread=addrHost;
     myPort=nPort;
}

void MyTcpSocket::slotWaitConnect()
{
  connectToHost(addrTcpThread, myPort, QIODevice::NotOpen);
  if (waitForConnected(2000))   //Ожидание ответа в течени 2 сек.
  {      
      emit signalAddrOpenPort(addrTcpThread);  //Отправляется адрес подключения
      close();                                 //Подключение закрывается, разрывается соединение
  }
  else  
      emit signalAddrClosedPort(addrTcpThread);
}


