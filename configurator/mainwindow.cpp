#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvSQLITE* SQLITE;

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
  ui->treeView->setLineWidth(5);
  
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
                                          .arg(q->value("sensor_ifc_name").toString())
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
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
//    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

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
