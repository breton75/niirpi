#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvSQLITE* SQLITE;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  log = svlog::SvLog(ui->textLog);
  
  QString s = AppParams::loadLayout(this);
  
  ui->treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
  ui->treeWidget->setHeaderLabels(QStringList()
                              << "Сигнал"
                              << "Тип интерфейса"
                              << "Интерфейс"
                              << "Протокол"
                              << "Размер (бит)"
                              << "Смещение (бит)");
  
  ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->treeWidget->setEditTriggers(0);
  
  QString fn = QString("%1/config.xbel").arg(qApp->applicationDirPath());
  bool b = readConfig(fn);
    
  
}

MainWindow::~MainWindow()
{
  QString s = AppParams::saveLayout(this);
  
  delete ui;
}

bool MainWindow::init()
{
  try {
    
    SQLITE = new SvSQLITE(this, "config.db");
    QSqlError serr = SQLITE->connectToDB();
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.databaseText());

    return true;
    
  }
  
  catch(SvException& e) {
    
    log << svlog::Critical << e.error << svlog::endl;
    return false;
  }
  
  
}

bool MainWindow::readConfig(QString& filename)
{
  try {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    serr = SQLITE->execSQL(SQL_SELECT_FROM_SENSORS, q);
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
    
    
    while(q->next()) {
      
      QVector<QVariant> data;
      
      data << q->value("id")
           << q->value("sensor_name")
           << q->value("ifc_type_name")
           << q->value("ifc_protocol_name")
           << q->value("ifc_name")
           << q->value("data_type")
           << q->value("data_length");

              
      TreeItem* item = new TreeItem()
      ui->treeWidget->addTopLevelItem();
      
      
      
    }
    
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    log << svlog::Time << svlog::Critical << e.err << svlog::endl;
    return false;
    
  }

}

void MainWindow::saveAs()
{
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save Bookmark File"),
                                         QDir::currentPath(),
                                         tr("XBEL Files (*.xbel *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    XbelWriter writer(ui->treeWidget);
    if (writer.writeFile(&file))
        statusBar()->showMessage(tr("File saved"), 2000);
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
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

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
