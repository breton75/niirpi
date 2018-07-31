#ifndef SV_REPOSITORIES_H
#define SV_REPOSITORIES_H

#include <QDialog>
#include <QToolBar>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"

#include "../global/sql_defs.h"

#include "treeitem.h"
#include "treemodel.h"

#include "sv_repository_editor.h"

namespace Ui {
class SvRepositories;
}

class SvRepositories : public QDialog
{
  Q_OBJECT
  
public:
  explicit SvRepositories(svlog::SvLog& log, QWidget *parent = 0);
  ~SvRepositories();
  
  bool init();
  
private:
  Ui::SvRepositories *ui;
  
  TreeModel* _model;
  TreeItem* rootItem;
  
  svlog::SvLog _log;
  
  QAction *actionNewRepository;
  QAction *actionEditRepository;
  QAction *actionDeleteRepository;
  
  SvException _exception;
  
  QToolBar *_toolBar;
  
  bool readRepositories();
  
  void createActions();
  
private slots:
  void newRepository();
  void editRepository();
  void deleteRepository();
  
};

#endif // SV_REPOSITORIES_H
