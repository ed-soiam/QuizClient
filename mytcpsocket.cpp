#include "mytcpsocket.h"

MyTcpSocket::MyTcpSocket(QObject* parent):QTcpSocket(parent)
{
    myThread.start();
    moveToThread(&myThread); // переносим его в поток QThreadObject
}

void MyTcpSocket::setPeerAddressMy(QHostAddress & addrHost, int & nPort)
{
     addrTcpThread=addrHost;
     myPort=nPort;
}

void MyTcpSocket::slotWaitConnect()
{
  connectToHost(addrTcpThread, myPort, QIODevice::NotOpen);
  if (waitForConnected(2000))
  {      
      emit signalAddrOpenPort(addrTcpThread);
      //disconnect();
      close();

  }
  else
  {
      emit signalAddrClosedPort(addrTcpThread);
      //disconnect();
  }
}


