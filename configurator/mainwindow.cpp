#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvSQLITE* SQLITE;
extern SvDeviceEditor *DEVICE_UI;
extern SvRepositories* REPOSITORIES_UI;
extern SvSignalEditor* SIGNALEDITOR_UI;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  setWindowTitle(QString("Конфигуратор стойки v.%1").arg(APP_VERSION));
  
  log = svlog::SvLog(ui->textLog);
  
  QString s = AppParams::loadLayout(this);
  
  ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
  ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->treeView->setEditTriggers(0);
  ui->treeView->setAlternatingRowColors(true);
  
  createActions();
  
//  fileMenu = menuBar()->addMenu(tr("&File"));
//  fileMenu->addAction(openAct);
//  fileMenu->addAction(saveAsAct);
//  fileMenu->addAction(exitAct);

//  menuBar()->addSeparator();
  
  configMenu = menuBar()->addMenu(tr("&Configuration"));
  configMenu->addAction(actionNewDevice);
  configMenu->addAction(actionEditDevice);
  configMenu->addAction(actionDeleteDevice);
  configMenu->addSeparator();
  configMenu->addAction(actionNewSignal);
  configMenu->addAction(actionEditSignal);
  configMenu->addAction(actionDeleteSignal);
  configMenu->addSeparator();
  configMenu->addAction(actionOpenRepositories);
  configMenu->addSeparator();
  
  ui->mainToolBar->addActions(QList<QAction*>() << actionNewDevice << actionEditDevice << actionDeleteDevice);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionNewSignal << actionEditSignal << actionDeleteSignal);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addActions(QList<QAction*>() << actionOpenRepositories);
  
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

    _model = new TreeModel(QString(DEVICES_TREE_HEADERS).split(';'), ui->treeView);
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

        
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_DEVICES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
    
    // заполняем модель выбранными данными
    while(q->next()) { 
      
      child_count = root->childCount();
      
      root->insertChildren(child_count, 1, column_count);

      root->child(child_count)->id = q->value("device_id").toInt();
      root->child(child_count)->parent_id = root->id;
      root->child(child_count)->is_main_row = true; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
//        root->child(child_count)->item_state = q->value("last_state").toInt();
      root->child(child_count)->item_type = itDevice;
  

      root->child(child_count)->setData(0, q->value("device_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itDeviceName, "device_name"));
      
      root->child(child_count)->setData(1, QVariant());
      root->child(child_count)->setInfo(1, ItemInfo(itUndefined, ""));
      
      root->child(child_count)->setData(2, q->value("device_data_length"));
      root->child(child_count)->setInfo(2, ItemInfo(itDeviceDataLength, "device_data_length"));
      
      root->child(child_count)->setData(3, QVariant());
      root->child(child_count)->setInfo(3, ItemInfo(itUndefined, ""));
      
      root->child(child_count)->setData(4, QString("\nИнтерфейс:\t%1\nПротокол:\t%2\nПараметры:\t%3\nТип данных:\t%4")
                                          .arg(q->value("device_ifc_name").toString())
                                          .arg(q->value("device_protocol_name").toString())
                                          .arg(q->value("device_connection_params").toString())
                                          .arg(q->value("device_data_type_name").toString()));
      root->child(child_count)->setInfo(4, ItemInfo(itDeviceParams, ""));
      
      root->child(child_count)->setData(5, q->value("device_description").toString());
      root->child(child_count)->setInfo(5, ItemInfo(itDeviceDescription, "device_description"));
      
//      for (int column = 0; column < column_count; column++) {
        
//        QString field_name = q->record().fieldName(column);
        
//        ItemInfo inf;
        
//        if(field_name == "sensor_id")                     inf.type = itDeviceId;
//        else if(field_name == "sensor_name")              inf.type = itDeviceName;
//        else if(field_name == "sensor_ifc_type_name")     inf.type = itDeviceIfcType;
//        else if(field_name == "sensor_ifc_protocol_name") inf.type = itDeviceIfcProtocol;
//        else if(field_name == "sensor_ifc_name")          inf.type = itDeviceIfcName;
//        else if(field_name == "sensor_data_type")         inf.type = itDeviceDataType;
//        else if(field_name == "sensor_data_length")       inf.type = itDeviceDataLength;
//        else if(field_name == "sensor_description")       inf.type = itDeviceDescription;
//        else                                              inf.type = itUndefined;
        
//        inf.fieldName = field_name;
        
//        root->child(child_count)->setInfo(column, inf);
        
//      }
    }
    q->finish();
    
    
    /** заполняем список сигналов **/
    TreeItem *device;
    
    for(int i = 0; i < root->childCount(); i++) {
      
      device = root->child(i);
      
      serr = SQLITE->execSQL(QString(SQL_SELECT_DEVICE_SIGNALS).arg(device->id), q);
      
      if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
      
      // заполняем модель выбранными данными
      while(q->next()) {
        
        child_count = device->childCount();
        
        device->insertChildren(child_count, 1, column_count);
        
        device->child(child_count)->id = q->value("signal_id").toInt();
        device->child(child_count)->parent_id = device->id;
        device->child(child_count)->is_main_row = false; //q->value("task_type").toInt() == 1; //  q->value("parent_task_id").toInt() == -1;
  //        root->child(child_count)->item_state = q->value("last_state").toInt();
        device->child(child_count)->item_type = q->value("signal_data_type").toInt() == 0 ? itSignalTypeDiscrete : itSignalTypeAnalog;
  
        device->child(child_count)->setData(0, q->value("signal_name"));
        device->child(child_count)->setInfo(0, ItemInfo(itSignalName, "signal_name"));
        
        device->child(child_count)->setData(1, q->value("signal_cob_id"));
        device->child(child_count)->setInfo(1, ItemInfo(itSignalCANID, "signal_cob_id"));
        
        device->child(child_count)->setData(2, q->value("signal_data_length"));
        device->child(child_count)->setInfo(2, ItemInfo(itDeviceDataLength, "signal_data_length"));
        
        device->child(child_count)->setData(3, q->value("signal_data_offset"));
        device->child(child_count)->setInfo(1, ItemInfo(itSignalOffset, "signal_data_offset"));
        
        device->child(child_count)->setData(5, q->value("signal_description"));
        device->child(child_count)->setInfo(5, ItemInfo(itSignalDescription, "signal_description"));
        
//        for (int column = 0; column < column_count; column++) {
          
//          QString field_name = q->record().fieldName(column);
          
//          ItemInfo inf;
          
//          if(field_name == "signal_id")                     inf.type = itSignalId;
//          else if(field_name == "signal_name")              inf.type = itSignalName;
//          else if(field_name == "signal_data_length")       inf.type = itSignalDataLenght;
//          else if(field_name == "signal_data_offset")       inf.type = itSignalOffset;
//          else if(field_name == "signal_description")       inf.type = itSignalDescription;
//          else                                              inf.type = itUndefined;
          
//          inf.fieldName = field_name;
          
//          device->child(child_count)->setInfo(column, inf);
        
//        }
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
  actionNewDevice = new QAction(this);
  actionNewDevice->setObjectName(QStringLiteral("actionNewDevice"));
  actionNewDevice->setIcon(icon);
  actionNewDevice->setText("Новое устройство");
  connect(actionNewDevice, &QAction::triggered, this, &MainWindow::newDevice);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/project.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditDevice = new QAction(this);
  actionEditDevice->setObjectName(QStringLiteral("actionEditDevice"));
  actionEditDevice->setIcon(icon);
  actionEditDevice->setText("Редактировать");
  connect(actionEditDevice, &QAction::triggered, this, &MainWindow::editDevice);
    
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/cross.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionDeleteDevice = new QAction(this);
  actionDeleteDevice->setObjectName(QStringLiteral("actionDeleteDevice"));
  actionDeleteDevice->setIcon(icon);
  actionDeleteDevice->setText("Удалить устройство");
  connect(actionDeleteDevice, &QAction::triggered, this, &MainWindow::deleteDevice);
  
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
  connect(actionDeleteDevice, &QAction::triggered, this, &MainWindow::deleteDevice);
  
  /// репозитоии
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/database.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionOpenRepositories = new QAction(this);
  actionOpenRepositories->setObjectName(QStringLiteral("actionOpenRepositories"));
  actionOpenRepositories->setIcon(icon);
  actionOpenRepositories->setText("Репозитории");
  connect(actionOpenRepositories, &QAction::triggered, this, &MainWindow::openRepositories);
  
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


void MainWindow::newDevice()
{
  DEVICE_UI = new SvDeviceEditor(this);
  
  int result = DEVICE_UI->exec();
  
  switch (result) {
    
    case SvDeviceEditor::Error:
      log << svlog::Critical << DEVICE_UI->lastError() << svlog::endl;
      break;
      
    case SvDeviceEditor::Accepted:
      _model->clear();
      readConfig();
      break;
    
  }
  
  delete DEVICE_UI;
  
}

void MainWindow::editDevice()
{
  DEVICE_UI = new SvDeviceEditor(this, _model->itemFormIndex(ui->treeView->currentIndex())->id);
  
  int result = DEVICE_UI->exec();
  
  switch (result) {
    
    case SvDeviceEditor::Error:
      log << svlog::Critical << DEVICE_UI->lastError() << svlog::endl;
      break;
      
    case SvDeviceEditor::Accepted:
      _model->clear();
      readConfig();
      break;
    
  }
  delete DEVICE_UI;
}

void MainWindow::deleteDevice()
{

}

void MainWindow::newSignal()
{
  int device_id = -1;
  
  switch (_model->itemFormIndex(ui->treeView->currentIndex())->item_type) {
    case itDevice:
      device_id = _model->itemFormIndex(ui->treeView->currentIndex())->id;
      break;
      
    case itSignalTypeAnalog:
    case itSignalTypeDiscrete:
      device_id = _model->itemFormIndex(ui->treeView->currentIndex())->parent_id;
      break;
      
    default:
      return;
  }
        
  SIGNALEDITOR_UI = new SvSignalEditor(this, device_id);
  
  int result = SIGNALEDITOR_UI->exec();
  
  switch (result) {
    
    case SvSignalEditor::Error:
      log << svlog::Critical << SIGNALEDITOR_UI->lastError() << svlog::endl;
      break;
      
    case SvSignalEditor::Accepted:
      _model->clear();
      readConfig();
      break;
    
  }
  
  delete SIGNALEDITOR_UI;

}

void MainWindow::editSignal()
{
  int device_id = -1;
  int signal_id = -1;
  
  switch (_model->itemFormIndex(ui->treeView->currentIndex())->item_type) {
      
    case itSignalTypeAnalog:
    case itSignalTypeDiscrete:
      signal_id = _model->itemFormIndex(ui->treeView->currentIndex())->id;
      device_id = _model->itemFormIndex(ui->treeView->currentIndex())->parent_id;
      break;
      
    default:
      return;
  }
  
  SIGNALEDITOR_UI = new SvSignalEditor(this, device_id, signal_id);
  
  int result = SIGNALEDITOR_UI->exec();
  
  switch (result) {
    
    case SvSignalEditor::Error:
      log << svlog::Critical << SIGNALEDITOR_UI->lastError() << svlog::endl;
      break;
      
    case SvSignalEditor::Accepted:
      _model->clear();
      readConfig();
      break;
    
  }
  
  delete SIGNALEDITOR_UI;
}

void MainWindow::deleteSignal()
{

}

void MainWindow::openRepositories()
{
  REPOSITORIES_UI = new SvRepositories(log, this);
  
  int result = REPOSITORIES_UI->exec();
  
  delete REPOSITORIES_UI;
  
}
