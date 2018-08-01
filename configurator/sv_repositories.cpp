#include "sv_repositories.h"
#include "ui_sv_repositories.h"

SvRepositories* REPOSITORIES_UI;
extern SvSQLITE* SQLITE;
extern SvRepositoryEditor* REPOSITORYEDITOR_UI;

SvRepositories::SvRepositories(svlog::SvLog& log, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvRepositories)
{
  ui->setupUi(this);
  
  _log = log;
  
  ui->treeView->setAlternatingRowColors(true);
  
  createActions();
  
  _toolBar = new QToolBar(this);
  ui->verticalLayout->insertWidget(0, _toolBar);
  _toolBar->addActions(QList<QAction*>() << actionNewRepository << actionEditRepository << actionDeleteRepository);
  _toolBar->addSeparator();
  
  if(!(init() && readRepositories()))
    QDialog::reject();
    
  
  setModal(true);
  show();
  
}

SvRepositories::~SvRepositories()
{
  delete ui;
} 

bool SvRepositories::init()
{
  try {
    
    _model = new TreeModel(QString(REPOSITORIES_TREE_HEADERS).split(';'), ui->treeView);
    ui->treeView->setModel(_model);
     
    return true;
    
  }
  
  catch(SvException& e) {
    
    _log << svlog::Critical << e.error << svlog::endl;
    return false;
  }
}

void SvRepositories::createActions()
{
  QIcon icon;
  /** *********** actions ************** **/
  /// репозитории              
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/database_new.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionNewRepository = new QAction(this);
  actionNewRepository->setObjectName(QStringLiteral("actionNewRepository"));
  actionNewRepository->setIcon(icon);
  actionNewRepository->setText("Новый репозиторий");
  connect(actionNewRepository, &QAction::triggered, this, &SvRepositories::newRepository);
  
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/database_edit.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionEditRepository = new QAction(this);
  actionEditRepository->setObjectName(QStringLiteral("actionEditRepository"));
  actionEditRepository->setIcon(icon);
  actionEditRepository->setText("Редактировать");
  connect(actionEditRepository, &QAction::triggered, this, &SvRepositories::editRepository);
    
  icon.addFile(QStringLiteral(":/munich/icons/munich-icons/ico/blue/database_delete.ico"), QSize(), QIcon::Normal, QIcon::Off);
  actionDeleteRepository = new QAction(this);
  actionDeleteRepository->setObjectName(QStringLiteral("actionDeleteRepository"));
  actionDeleteRepository->setIcon(icon);
  actionDeleteRepository->setText("Удалить устройство");
  connect(actionDeleteRepository, &QAction::triggered, this, &SvRepositories::deleteRepository);
}

void SvRepositories::newRepository()
{
  REPOSITORYEDITOR_UI = new SvRepositoryEditor(this);
  
  int result = REPOSITORYEDITOR_UI->exec();
  
  switch (result) {
    
    case SvRepositoryEditor::Error:
      _log << svlog::Critical << REPOSITORYEDITOR_UI->lastError() << svlog::endl;
      break;
      
    case SvRepositoryEditor::Accepted:
      _model->clear();
      readRepositories();
      break;
    
  }
  delete REPOSITORYEDITOR_UI;
  
}

void SvRepositories::editRepository()
{
  REPOSITORYEDITOR_UI = new SvRepositoryEditor(this, _model->itemFormIndex(ui->treeView->currentIndex())->id);
  
  int result = REPOSITORYEDITOR_UI->exec();
  
  switch (result) {
    
    case SvRepositoryEditor::Error:
      _log << svlog::Critical << REPOSITORYEDITOR_UI->lastError() << svlog::endl;
      break;
      
    case SvRepositoryEditor::Accepted:
      _model->clear();
      readRepositories();
      break;
    
  }
  delete REPOSITORYEDITOR_UI;
}

void SvRepositories::deleteRepository()
{

}

bool SvRepositories::readRepositories()
{
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  try {
    
    // rootItem создается при создании объекта модели
    // кол-во столбцов задается TREE_HEADERS и должно соответствовать кол-ву полей в таблице sensors
    TreeItem *root = _model->rootItem();

    int child_count = root->childCount();
    int column_count = root->columnCount();
    
    for(int i = 0; i < column_count; i++)
      root->setInfo(i, ItemInfo());
    
    
    QSqlError serr = SQLITE->execSQL(SQL_SELECT_REPOSITORIES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
    
    // заполняем модель выбранными данными
    while(q->next()) { 
      
      child_count = root->childCount();
      
      root->insertChildren(child_count, 1, column_count);

      root->child(child_count)->id = q->value("repository_id").toInt();
      root->child(child_count)->parent_id = root->id;
      root->child(child_count)->is_main_row = false;
//        root->child(child_count)->item_state = q->value("last_state").toInt();
      root->child(child_count)->item_type = itRepository;

      root->child(child_count)->setData(0, q->value("repository_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itRepositoryName, "repository_name"));
      
      root->child(child_count)->setData(1, q->value("repository_host"));
      root->child(child_count)->setInfo(1, ItemInfo(itRepositoryHost, "repository_host"));
      
      root->child(child_count)->setData(2, q->value("repository_port"));
      root->child(child_count)->setInfo(2, ItemInfo(itRepositoryPort, "repository_port"));\
      
      root->child(child_count)->setData(3, q->value("repository_login"));
      root->child(child_count)->setInfo(3, ItemInfo(itRepositoryLogin, "repository_login"));
      
      root->child(child_count)->setData(4, q->value("repository_dbname"));
      root->child(child_count)->setInfo(4, ItemInfo(itRepositoryDBName, "repository_dbname"));
      
      root->child(child_count)->setData(5, q->value("repository_table_name"));
      root->child(child_count)->setInfo(5, ItemInfo(itRepositoryTableName, "repository_table_name"));

    }
    q->finish();
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    _log << svlog::Time << svlog::Critical << e.error << svlog::endl;
    return false;
    
  }

}
