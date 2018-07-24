#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvSQLITE* SQLITE;
extern SvSensor *SENSOR_UI;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  actionPp = new QAction(this);
  actionPp->setText("Pp");
  
  setWindowTitle(QString("Конфигуратор стойки v.%1").arg(APP_VERSION));
  
  log = svlog::SvLog(ui->textLog);
  
  QString s = AppParams::loadLayout(this);
  
  ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
  ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->treeView->setEditTriggers(0);
  ui->treeView->setLineWidth(5);
  
  createActions();
  
//  fileMenu = menuBar()->addMenu(tr("&File"));
//  fileMenu->addAction(openAct);
//  fileMenu->addAction(saveAsAct);
//  fileMenu->addAction(exitAct);

//  menuBar()->addSeparator();
  
  configMenu = menuBar()->addMenu(tr("&Configuration"));
  configMenu->addAction(actionNewSensor);
  configMenu->addAction(actionEditSensor);
  configMenu->addAction(actionDeleteSensor);
  configMenu->addSeparator();
  configMenu->addAction(actionNewSignal);
  configMenu->addAction(actionEditSignal);
  configMenu->addAction(actionDeleteSignal);
  configMenu->addSeparator();
  configMenu->addAction(actionNewRepository);
  configMenu->addAction(actionEditRepository);
  configMenu->addAction(actionDeleteRepository);
  configMenu->addSeparator();
  
  ui->mainToolBar->addActions(QList<QAction*>() << actionNewSensor << actionEditSensor << actionDeleteSensor);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionNewSignal << actionEditSignal << actionDeleteSignal);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionNewRepository << actionEditRepository << actionDeleteRepository);
  
//  helpMenu = menuBar()->addMenu(tr("&Help"));
//  helpMenu->addAction(aboutAct);
//  helpMenu->addAction(aboutQtAct);

  
//  QMetaObject::connectSlotsByName(this);
  
  
#ifdef APP_DEBUG
  _config_path = QString("D:/c++/NIIRPI/configurator/config.db");
#else
  _config_path = QString("%1/config.db").arg(qApp->applicationDirPath());
#endif
    
  
}

MainWindow::~MainWindow()
{
  QString s = AppParams::saveLayout(this);
  
  delete ui;
}

bool MainWindow::init()
{
  try {
    
    SQLITE = new SvSQLITE(this, _config_path);
    QSqlError serr = SQLITE->connectToDB();
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.databaseText());

    _model = new TreeModel(QString(TREE_HEADERS).split(';'), ui->treeView);
    ui->treeView->setModel(_model);
    
    bool b = readConfig();
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    log << svlog::Critical << e.error << svlog::endl;
    return false;
  }
  
  
}

bool MainWindow::readConfig()
{
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    /** заполняем список датчиков **/
    // rootItem создается при создании объекта модели
    // кол-во столбцов задается TREE_HEADERS и должно соответствовать кол-ву полей в таблице sensors
    TreeItem *root = _model->rootItem();

    int child_count = root->childCount();
    int column_count = root->columnCount();
    
    for(int i = 0; i < column_count; i++)
      root->setInfo(i, ItemInfo());
    
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_FROM_SENSORS, q);
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
    
    // заполняем модель выбранными данными
    while(q->next()) { 
      
      child_count = root->childCount();
      
      root->insertChildren(child_count, 1, column_count);

      root->child(child_count)->id = q->value("sensor_id").toInt();
      root->child(child_count)->parent_id = root->id;
      root->child(child_count)->is_main_row = true; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
//        root->child(child_count)->item_state = q->value("last_state").toInt();
      root->child(child_count)->item_type = itSensor;
  

      root->child(child_count)->setData(0, q->value("sensor_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itSensorName, "sensor_name"));
      
      root->child(child_count)->setData(1, q->value("sensor_data_length"));
      root->child(child_count)->setInfo(1, ItemInfo(itSensorDataLength, "sensor_data_length"));
      
      root->child(child_count)->setData(2, QVariant());
      root->child(child_count)->setInfo(2, ItemInfo(itUndefined, ""));
      
      root->child(child_count)->setData(3, QString("\nИнтерфейс:\t%1\nПротокол:\t%2\nПорт:\t%3\nТип данных:\t%4")
                                          .arg(q->value("sensor_ifc_type_name").toString())
                                          .arg(q->value("sensor_ifc_protocol_name").toString())
                                          .arg(q->value("sensor_ifc_port_name").toString())
                                          .arg(q->value("sensor_data_type").toString()));
      root->child(child_count)->setInfo(3, ItemInfo(itSensorParams, ""));
      
      root->child(child_count)->setData(4, q->value("sensor_description").toString());
      root->child(child_count)->setInfo(4, ItemInfo(itSensorDescription, "sensor_description"));
      
//      for (int column = 0; column < column_count; column++) {
        
//        QString field_name = q->record().fieldName(column);
        
//        ItemInfo inf;
        
//        if(field_name == "sensor_id")                     inf.type = itSensorId;
//        else if(field_name == "sensor_name")              inf.type = itSensorName;
//        else if(field_name == "sensor_ifc_type_name")     inf.type = itSensorIfcType;
//        else if(field_name == "sensor_ifc_protocol_name") inf.type = itSensorIfcProtocol;
//        else if(field_name == "sensor_ifc_name")          inf.type = itSensorIfcName;
//        else if(field_name == "sensor_data_type")         inf.type = itSensorDataType;
//        else if(field_name == "sensor_data_length")       inf.type = itSensorDataLength;
//        else if(field_name == "sensor_description")       inf.type = itSensorDescription;
//        else                                              inf.type = itUndefined;
        
//        inf.fieldName = field_name;
        
//        root->child(child_count)->setInfo(column, inf);
        
//      }
    }
    q->finish();
    
    
    /** заполняем список сигналов **/
    TreeItem *sensor;
    
    for(int i = 0; i < root->childCount(); i++) {
      
      sensor = root->child(i);
      
      serr = SQLITE->execSQL(QString(SQL_SELECT_FROM_SIGNALS).arg(sensor->id), q);
      
      if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
      
      // заполняем модель выбранными данными
      while(q->next()) {
        
        child_count = sensor->childCount();
        
        sensor->insertChildren(child_count, 1, column_count);
        
        sensor->child(child_count)->id = q->value("signal_id").toInt();
        sensor->child(child_count)->parent_id = sensor->id;
        sensor->child(child_count)->is_main_row = false; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
  //        root->child(child_count)->item_state = q->value("last_state").toInt();
        sensor->child(child_count)->item_type = itSignal;
  
        sensor->child(child_count)->setData(0, q->value("signal_name"));
        sensor->child(child_count)->setData(1, q->value("signal_data_length"));
        sensor->child(child_count)->setData(2, q->value("signal_data_offset"));
        sensor->child(child_count)->setData(4, q->value("signal_description"));
        
        for (int column = 0; column < column_count; column++) {
          
          QString field_name = q->record().fieldName(column);
          
          ItemInfo inf;
          
          if(field_name == "signal_id")                     inf.type = itSignalId;
          else if(field_name == "signal_name")              inf.type = itSignalName;
          else if(field_name == "signal_data_length")       inf.type = itSignalDataLenght;
          else if(field_name == "signal_data_offset")       inf.type = itSignalOffset;
          else if(field_name == "signal_description")       inf.type = itSignalDescription;
          else                                              inf.type = itUndefined;
          
          inf.fieldName = field_name;
          
          sensor->child(child_count)->setInfo(column, inf);
        
        }
      }
      
      q->finish();
      
    }
      
    delete q;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    log << svlog::Time << svlog::Critical << e.error << svlog::endl;
    return false;
    
  }

}

void MainWindow::saveAs()
{
//    QString fileName =
//            QFileDialog::getSaveFileName(this, tr("Save Bookmark File"),
//                                         QDir::currentPath(),
//                                         tr("XBEL Files (*.xbel *.xml)"));
//    if (fileName.isEmpty())
//        return;

//    QFile file(fileName);
//    if (!file.open(QFile::WriteOnly | QFile::Text)) {
//        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString()));
//        return;
//    }

}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About QXmlStream Bookmarks"),
            tr("The <b>QXmlStream Bookmarks</b> example demonstrates how to use Qt's "
               "QXmlStream classes to read and write XML documents."));
}

void MainWindow::createActions()
{
  QIcon icon;
  /** *********** actions ************** **/
  /// датчики
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/project_add.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionNewSensor = new QAction(this);
  actionNewSensor->setObjectName(QStringLiteral("actionNewSensor"));
  actionNewSensor->setIcon(icon);
  actionNewSensor->setText("Новое устройство");
  connect(actionNewSensor, &QAction::triggered, this, &MainWindow::newSensor);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/project.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditSensor = new QAction(this);
  actionEditSensor->setObjectName(QStringLiteral("actionEditSensor"));
  actionEditSensor->setIcon(icon);
  actionEditSensor->setText("Редактировать");
  connect(actionEditSensor, &QAction::triggered, this, &MainWindow::editSensor);
    
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/cross.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionDeleteSensor = new QAction(this);
  actionDeleteSensor->setObjectName(QStringLiteral("actionDeleteSensor"));
  actionDeleteSensor->setIcon(icon);
  actionDeleteSensor->setText("Удалить устройство");
  connect(actionDeleteSensor, &QAction::triggered, this, &MainWindow::deleteSensor);
  
  /// сигналы
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/task_add.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionNewSignal = new QAction(this);
  actionNewSignal->setObjectName(QStringLiteral("actionNewSignal"));
  actionNewSignal->setIcon(icon);
  actionNewSignal->setText("Новый сигнал");
  connect(actionNewSignal, &QAction::triggered, this, &MainWindow::newSignal);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/task.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditSignal = new QAction(this);
  actionEditSignal->setObjectName(QStringLiteral("actionEditSignal"));
  actionEditSignal->setIcon(icon);
  actionEditSignal->setText("Редактировать");
  connect(actionEditSignal, &QAction::triggered, this, &MainWindow::editSignal);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/cross.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionDeleteSignal = new QAction(this);
  actionDeleteSignal->setObjectName(QStringLiteral("actionDeleteSignal"));
  actionDeleteSignal->setIcon(icon);
  actionDeleteSignal->setText("Удалить сигнал");
  connect(actionDeleteSensor, &QAction::triggered, this, &MainWindow::deleteSensor);
  
  /// репозитоии
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/add.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionNewRepository = new QAction(this);
  actionNewRepository->setObjectName(QStringLiteral("actionNewRepository"));
  actionNewRepository->setIcon(icon);
  actionNewRepository->setText("Новый репозиторий");
  connect(actionNewRepository, &QAction::triggered, this, &MainWindow::newRepository);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/issue.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditRepository = new QAction(this);
  actionEditRepository->setObjectName(QStringLiteral("actionEditRepository"));
  actionEditRepository->setIcon(icon);
  actionEditRepository->setText("Редактировать");
  connect(actionEditRepository, &QAction::triggered, this, &MainWindow::editRepository);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/cross.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionDeleteRepository = new QAction(this);
  actionDeleteRepository->setObjectName(QStringLiteral("actionDeleteRepository"));
  actionDeleteRepository->setIcon(icon);
  actionDeleteRepository->setText("Удалить репозиторий");
  connect(actionDeleteRepository, &QAction::triggered, this, &MainWindow::deleteRepository);
  
  /// служебные
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
//  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAsAct = new QAction(tr("&Save As..."), this);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(exitAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutQtAct);
}


void MainWindow::newSensor()
{
  SENSOR_UI = new SvSensor(this);
  int result = SENSOR_UI->exec();
  switch (result) {
    
    case SvSensor::Error:
      log << svlog::Critical << SENSOR_UI->lastError() << svlog::endl;
      break;
      
    case SvSensor::Accepted:
      _model->clear();
      readConfig();
      break;
    
  }
  delete SENSOR_UI;
  
}

void MainWindow::editSensor()
{
  SENSOR_UI = new SvSensor(this, _model->itemFormIndex(ui->treeView->currentIndex())->id);
  int result = SENSOR_UI->exec();
  switch (result) {
    
    case SvSensor::Error:
      log << svlog::Critical << SENSOR_UI->lastError() << svlog::endl;
      break;
      
    case SvSensor::Accepted:
      _model->clear();
      readConfig();
      break;
    
  }
  delete SENSOR_UI;
}

void MainWindow::deleteSensor()
{

}

void MainWindow::newSignal()
{

}

void MainWindow::editSignal()
{

}

void MainWindow::deleteSignal()
{

}

void MainWindow::newRepository()
{

}

void MainWindow::editRepository()
{

}

void MainWindow::deleteRepository()
{

}

