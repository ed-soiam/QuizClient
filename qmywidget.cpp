#include "qmywidget.h"

QMyWidget::QMyWidget(QWidget *parent) : QWidget(parent)
{

    v_BoxLayout = new QVBoxLayout;
    h_BoxLayout=new QHBoxLayout;
    h_BoxLayout2=new QHBoxLayout;
    h_BoxLayout2->setSizeConstraint(QLayout::SetMaximumSize);
    gridButton=new QGridLayout();

    spacerItem=new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);


    buttonConnect = new QToolButton();//"Подключить");
    buttonConnect->setText("Подключить");
    buttonUpdate = new QToolButton();//("Обновить");
    buttonUpdate->setText("Обновить");
    //buttonScript = new QToolButton();//("Выполнить текущий скрипт");
    buttonRegister=new QToolButton();//("Регистрация");
    buttonRegister->setText("Регистрация");
    myTextEdit = new QTextEdit;
    myTextEditEnt = new QTextEdit;
    comBoxIP = new QComboBox;

    labelIP=new QLabel("Адрес");
    labelReg=new QLabel("Не зарегистрирован!");
    labelIP->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    labelReg->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    setLayout(v_BoxLayout);
    v_BoxLayout->addLayout(h_BoxLayout);
    h_BoxLayout->addWidget(labelIP);
    h_BoxLayout->addWidget(comBoxIP);
    h_BoxLayout->addWidget(buttonConnect);
    h_BoxLayout->addWidget(buttonUpdate);
    h_BoxLayout->addWidget(buttonRegister);
    v_BoxLayout->addLayout(h_BoxLayout2);
    h_BoxLayout2->addLayout(gridButton);
    h_BoxLayout2->addItem(spacerItem);






  //Тестовые
  /*    comboBoxGame=new QComboBox();
      comboBoxGame->addItem("Game 1");
      comboBoxGame->addItem("Game 2");
      myVBoxLayout->addWidget(comboBoxGame);
      buttonChoice=new QPushButton("Выбор игры");
      myVBoxLayout->addWidget(buttonChoice);
      myVBoxLayout->addWidget(myLabelEntText);
      myVBoxLayout->addWidget(myTextEditEnt);
      myVBoxLayout->addWidget(buttonScript);*/
  //    myVBoxLayout->addWidget(myLabelAnswer);
  //    myVBoxLayout->addWidget(myTextEdit);

      connect (buttonConnect , SIGNAL(clicked()),this, SLOT(slotButtonConnect()));
      connect (buttonUpdate , SIGNAL(clicked()),this, SLOT(slotButtonUpdate()));
      connect (buttonRegister , SIGNAL(clicked()),this, SLOT(slotButtonRegister()));

      //connect(buttonReset,SIGNAL(clicked(bool)),this,SLOT(slotChoiceMenu(QString)));
      //connect(managerInfo , SIGNAL(addrTextEdit(QString)),this, SLOT(slotAddrTextEdit(QString)));
      //connect(QSHManagerInfo::socket,SIGNAL(signalConnected(bool)),this,SLOT(slotSetConnectSocket(bool)));//Сигнал о подключении сокета

      connectHost = new MyClient();
      connect(connectHost , SIGNAL(signalAddrForComboBox(QList<QHostAddress>)),this, SLOT(slotAddrForComboBox(QList<QHostAddress>)));
      startWidget=true;
  }

  QMyWidget::~QMyWidget()
  {
      delete v_BoxLayout;
      delete buttonConnect;
      delete myTextEdit;
      delete myTextEditEnt;
      delete comBoxIP;
      delete labelIP;
      delete labelReg;
      //delete myLabelEntText;
      if (managerInfo != Q_NULLPTR)
          delete managerInfo;
      delete buttonRegister;
      //delete comboBoxGame;
 /*     if (gridButton != Q_NULLPTR)
          delete gridButton;
      if (labelNameGame != Q_NULLPTR)
          delete labelNameGame;*/

  }

  void QMyWidget :: slotButtonConnect() //Слот для подключения с адресом указанным в comboBox
  {
      if(connectSocket)
      {
          delete managerInfo;
          managerInfo = Q_NULLPTR;
          if(managerInfo==Q_NULLPTR)
          {
              buttonConnect->setText("Подключить");
              connectSocket=false;
              myTextEdit->append("Поток инрефейса остановлен");
          }
      }
      else if (managerInfo == Q_NULLPTR)
      {
          managerInfo=new QSHManagerInfo(0, comBoxIP->currentText());//Выделяем память, инициализируем с адресом порта
          managerInfo->start();
          connect(managerInfo,SIGNAL(setConnected(bool)),this,SLOT(slotSetConnectSocket(bool)),Qt::UniqueConnection);//Сигнал о подключении сокета, Qt::Unique коннектить один раз
          connect(managerInfo,SIGNAL(setVarMenu(QStringList)),this,SLOT(slotChoiceMenu(QStringList)));//Подключение сигнала с вариантом меню
          connect(managerInfo,SIGNAL(registerSuccessfully(bool)),this,SLOT(slotRegSuccessfully(bool)));
      }
  }

  void QMyWidget::slotRegSuccessfully(bool regS)
  {
      registerQuiz=regS;
      if(registerQuiz==true)
          labelReg->setText("Зарегистрирован");
      else
          labelReg->setText("Не зарегистрирован");

  }
  void QMyWidget::slotButtonUpdate() //Обновление списка подключений в comboBox
  {
      if(startWidget){
          comBoxIP->clear();
          connectHost->updateConnectHost();}
  }

  void QMyWidget::slotAddrForComboBox(QList<QHostAddress> openSocketList) //Добавление обнаруженых открытх портов и добавление в comboBox
  {
      QList<QHostAddress> list;
      list=openSocketList;

      for(int i=0; i<list.length(); i++)
          comBoxIP->addItem(list.at(i).toString());
  }

  void QMyWidget::slotSetConnectSocket(bool value)//Подключение к сокету по кнопке
  {
      connectSocket=value;
      if(connectSocket)
            buttonConnect->setText("Отключить");
  }

  void QMyWidget::slotChoiceMenu(QStringList numberMenu) //Слот выбора варианта меню.
  {
      QStringList listStringButton;
      listStringButton=numberMenu;
      //qDebug()<<listStringButton;

      if(listStringButton.isEmpty()) //Удаление меню предыдущего варианта игры
      {
          if (!listPushButton.isEmpty())
          {
              for(int i=0; i<listPushButton.length();i++)
                  delete listPushButton[i];
//              if(listPushButton.length()>1)
//                      delete gridButton;
              listPushButton.clear();
          }
      }

      if(listPushButton.isEmpty())
      {
          for(int i=0; i<listStringButton.length(); i++){ //Создание указателей кнопок
              listPushButton.append(new QPushButton());}

          for(int i=0; i<listPushButton.length(); i++)
                      connect(listPushButton.at(i), SIGNAL(clicked(bool)),this,SLOT(slotButtonGame()));

          for(int i=0; i<listPushButton.length(); i++)
              listPushButton[i]->setText(listStringButton[i]);

          if(listPushButton.length()==1)
          {
              listPushButton[0]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
              gridButton->addWidget(listPushButton[0],0,0);
          }
          if(listPushButton.length()>1)
          {
             int i=0;
             for(int v=0;v<listPushButton.length()/2;v++)
             {
                for(int h=0;h<listPushButton.length()/2;h++)
                {
                  if(i<(listPushButton.length()))
                  {
                      gridButton->addWidget(listPushButton[i],v,h);
                      listPushButton[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding); //Кнопки будут занимать максимальное пространство в виджете
                  }
                  i++;
                }
             }
          }
      }
  }

  void QMyWidget::slotButtonGame() //Слот обрабатывает нажатия на все 5 кнопок обоих вариантов меню
  {
      QPushButton *buttonGame=dynamic_cast<QPushButton*>(sender());
      for(int i=0;i<listPushButton.length();i++)
      {
          if(buttonGame==listPushButton[i])
          {
              QString _str = QString("{\"class\":\"netro\",\"command\":\"press\",\"label\":1,\"key\":\"%1\",\"button\":%2}").arg(settings_register_key(),QString::number(i+1));
            //В строке с командой стоят аргументы подстановки для всех кнопок: %1 (первый ключ регистрации), %2 второй аргумент номер кнопкии
            //Сами аргуменны указываюстся в arg в порядке следования номеров в строке.
             managerInfo->sendCommand(_str,QJSONTask::JSON_ANSWER_STANDARD); //Вторым агументом задается тип парсера который нужно использовать для расшифровки
          }
      }
  }

  QString QMyWidget::settings_register_key()  //Сохранение настроек в реестре
  {
      QString strLoc("ssssssss");
      QSettings app_settings("vasilenokEndCo","quiz"); //Создание именованной области для сохранения настроек данного проекта
      app_settings.beginGroup("/Settings");  //Создание группы для хранения настроек

      if(!app_settings.value("statusKey").toBool())
      {
          qsrand(QDateTime::currentMSecsSinceEpoch());
          int randomKey = qrand(); //Получение случайного числа в зависимости от времени для ключа регистрации
          app_settings.setValue("statusKey", true);
          app_settings.setValue("registerKey", randomKey);

          return strLoc.append(app_settings.value("registerKey").toString());
      }
      else
      {
          return strLoc.append(app_settings.value("registerKey").toString());
      }
      app_settings.endGroup();
  }

  void QMyWidget::slotButtonRegister() //Слот кнопки регистрации
  {
      QString commandRegister=QString("{\"class\":\"netro\", \"command\":\"register\", \"label\":1, \"key\":\"%1\"}").arg(settings_register_key());
      managerInfo->sendCommand(commandRegister,QJSONTask::JSON_ANSWER_STANDARD_REG);
  }
  /*
  void QMyWidget::load_settings() //Получение настроек из реестра
  {

  }

  void QMyWidget::slotButtonSendScript() //Выполнить текущий скрипт
  {
      if(connectSocket)
      {
          QString strCommand;
          strCommand=myTextEditEnt->toPlainText();
          if(!strCommand.isNull()){
              managerInfo->sendMyEvent(strCommand);
              myTextEdit->append("Команда принята к обработке");}
      }
  }*/

  /*
  void QMyWidget::slotSelectionGame() //Ручной выбор варианта игры. Имитация команды от сервера. Временный.
  {
      if(statusGame==false)
      {
          emit signalChoiceMenu(comboBoxGame->currentIndex());
          statusGame=true;
      }
      else
      {
          emit signalDelWidget();
          emit signalChoiceMenu(comboBoxGame->currentIndex());
          statusGame==false;
      }
  }*/

  /*
  void QMyWidget::slotAddrTextEdit(QString _answer) //Вывод сообщений и ответов в textEdit
  {
      myTextEdit->append(_answer);
      myTextEdit->append("Команда успешно выполнена");
  }
  */
