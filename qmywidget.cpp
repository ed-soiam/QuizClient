#include "qmywidget.h"

QMyWidget::QMyWidget(QWidget *parent) : QWidget(parent)//,
    //labelImage(new QLabel()) //первый способ инициализации умной переменной - инициализация при создании класса new QLabel
    //Воторой способ описан ниже - инициализация при помощи присвоения локального обьекта глобальной переменной.
{
    v_BoxLayout = new QVBoxLayout;
    h_BoxLayout=new QHBoxLayout;
    h_BoxLayout2=new QHBoxLayout;
    h_BoxLayout2->setSizeConstraint(QLayout::SetMaximumSize);
    gridButton=new QGridLayout();
    spacerItem=new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding); //Растяжка
    QFrame* frameButton=new QFrame();
    buttonConnect = new QToolButton();//"Подключить");
    buttonConnect->setText("Подключить");
    buttonUpdate = new QToolButton();//("Обновить");
    buttonUpdate->setText("Обновить");
    buttonRegister=new QToolButton();//("Регистрация");
    buttonRegister->setText("Регистрация");
    comBoxIP = new QComboBox;
    comBoxIP->setMinimumContentsLength(13);
    labelIP=new QLabel("Адрес");
    buttonConnect->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    buttonUpdate->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    buttonRegister->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    comBoxIP->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    labelIP->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);


    setLayout(v_BoxLayout);
    v_BoxLayout->addWidget(frameButton);
    frameButton->setFrameShape(QFrame::Panel);
    frameButton->setLayout(h_BoxLayout);
    h_BoxLayout->addWidget(labelIP);
    h_BoxLayout->addWidget(comBoxIP);
    h_BoxLayout->addWidget(buttonConnect);
    h_BoxLayout->addWidget(buttonUpdate);
    h_BoxLayout->addWidget(buttonRegister);
    v_BoxLayout->addLayout(h_BoxLayout2);
    h_BoxLayout2->addLayout(gridButton);
    h_BoxLayout2->addItem(spacerItem);

    strNameImage<<":/image/not_reg.jpg"<<":/image/tick.png";    //Необходимо обязательно создать файл ресурса. Добавляется аналогично как класс
    QPixmap pixmap(strNameImage[0]);
//Пробовал с QScopedPointer. Обьтект удаляется при выходе из поля видимости. Принудительно его не нужно удалять
//    QScopedPointer <QLabel> labelImage(new QLabel());
//Пробовал с QPointer. При удаление обьекта, выделенной памяти присваивается Q_NULLPTR. Обьект сам не удаляется при выходе из поля видимости
//    QPointer <QLabel> labelImage(new QLabel());

//**********************************************************************Нужно запомнить
//Второй способ выделения памяти для умной переменной. Выделяем память как для локальной переменой, а после присваиваем ее глобальной.
//Уную переменную нельзя инициализировать как обычную переменную
    labelImage = QSharedPointer<QLabel>(new QLabel());
    labelImage->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    labelImage->setPixmap(pixmap.scaledToHeight(17));
    h_BoxLayout->addWidget(labelImage.data()); //Для использования умной меременной нужно использовать data()
    //h_BoxLayout->addWidget(labelImage.data());
    //h_BoxLayout->addWidget(labelImage);

      connect (buttonConnect , SIGNAL(clicked()),this, SLOT(slotButtonConnect()));
      connect(buttonConnect,SIGNAL(clicked(bool)),this,SLOT(slotButtonRegister())); //Подключение и регистррация по одному нажатию на кнопку подключения
      connect (buttonUpdate , SIGNAL(clicked()),this, SLOT(slotButtonUpdate()));
      connect (buttonRegister , SIGNAL(clicked()),this, SLOT(slotButtonRegister()));

      connectHost = new MyClient();
      connect(connectHost , SIGNAL(signalAddrForComboBox(QList<QHostAddress>)),this, SLOT(slotAddrForComboBox(QList<QHostAddress>)));
  }

  QMyWidget::~QMyWidget()
  {
      delete connectHost;
      if (managerInfo != Q_NULLPTR)
          delete managerInfo;
      if(!listPushButton.isEmpty()) //Удаление кнопкок если прокрамма была закрыта принудительно до их самоуничтожения.
      {
          for(int i=0; i<listPushButton.length(); i++)
              if(listPushButton[i]!=Q_NULLPTR)
              {
                  delete listPushButton[i];
                  listPushButton[i]=Q_NULLPTR;
              }
      }
      delete labelIP;
      delete comBoxIP;
      delete buttonConnect;
      delete buttonUpdate;
      delete buttonRegister;
      h_BoxLayout2->removeItem(spacerItem);//Удаляет элемент макета из макета. Ответственность вызывающего абонента заключается в удалении элемента.
      //Обратите внимание, что элемент может быть макетом (поскольку QLayout наследует QLayoutItem).
      //Без этого нельзя удалить Layout
      delete spacerItem;
      delete gridButton;
      delete h_BoxLayout;
      delete h_BoxLayout2;
      delete v_BoxLayout;
  }

  void QMyWidget :: slotButtonConnect() //Слот для подключения с адресом указанным в comboBox
  {
      if(connectSocket)
      {
          delete managerInfo; //Реализовано в деструкторое родителя QThreadObject::~QThreadObject()
          managerInfo = Q_NULLPTR;
          if(managerInfo==Q_NULLPTR)
          {
              buttonConnect->setText("Подключить");
              connectSocket=false;
              regStatus(false);
              buttonDeleteGame();
          }
      }
      else if (managerInfo == Q_NULLPTR)
      {
          managerInfo=new QSHManagerInfo(0, comBoxIP->currentText());//Выделяем память, инициализируем с адресом порта
          managerInfo->start(); //Функция определенная в классе родителя
          connect(managerInfo,SIGNAL(setConnected(bool)),this,SLOT(slotSetConnectSocket(bool)),Qt::UniqueConnection);//Сигнал о подключении сокета, Qt::Unique коннектить один раз
          connect(managerInfo,SIGNAL(setVarMenu(QStringList)),this,SLOT(slotChoiceMenu(QStringList)));//Подключение сигнала с вариантом меню
          connect(managerInfo,SIGNAL(registerSuccessfully(bool)),this,SLOT(slotRegStatus(bool)));
      }
  }

void QMyWidget::slotRegStatus(bool regS) //Благополучная регистрация
{
     bool registerQuiz=regS;
     regStatus(registerQuiz);

}

void QMyWidget::regStatus(bool statusReg)//Изменение иконки со статусом
{
    if(statusReg==true)
    {
        QPixmap pixmapWell(strNameImage[1]);
        labelImage->setPixmap(pixmapWell.scaledToHeight(17));
    }
    else
    {
        QPixmap pixmapBed(strNameImage[0]);
        labelImage->setPixmap(pixmapBed.scaledToHeight(17));
    }
}

  void QMyWidget::slotButtonUpdate() //Обновление списка подключений в comboBox
  {    
       comBoxIP->clear();
       connectHost->updateConnectHost();
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

  void QMyWidget::buttonDeleteGame()//Кнопки нужно удалять из слота и из самого виджета, поэтому пришлось добавить отдельную функцию
  {
      if(!listPushButton.isEmpty())
      {
          for(int i=0; i<listPushButton.length();i++){

              if(listPushButton[i]!=Q_NULLPTR)
              {
                  delete listPushButton[i];
                  listPushButton[i]=Q_NULLPTR;  //Выделенная память не больше не занята
              }
          }
          listPushButton.clear();
      }
  }

  void QMyWidget::slotChoiceMenu(QStringList numberMenu) //Слот выбора варианта меню.
  {
      QStringList listStringButton;
      listStringButton=numberMenu;      

      if(listStringButton.isEmpty()) //Удаление меню при приходте пустого варианта игры
      {
          buttonDeleteGame();
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
      QPushButton *buttonGame=dynamic_cast<QPushButton*>(sender()); //QObject/ Возвращает указатель на объект, который отправил сигнал, если он вызван в слот, активированный сигналом; в противном случае он возвращает 0
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
     if(managerInfo!=Q_NULLPTR)
      {
          QString commandRegister=QString("{\"class\":\"netro\", \"command\":\"register\", \"label\":1, \"key\":\"%1\"}").arg(settings_register_key());
          managerInfo->sendCommand(commandRegister,QJSONTask::JSON_ANSWER_STANDARD_REG);
      }

  }


