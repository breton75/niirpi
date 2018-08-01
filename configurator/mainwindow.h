#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QDebug>
//#include <QXmlStreamReader>
//#include <QXmlReader>
//#include <QtXml>
//#include "xbelreader.h"
//#include "xbelwriter.h"
#include <QtWidgets>
//#include <QSqlError>
#include <QSql>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"

#include "../global/sql_defs.h"
#include "treeitem.h"
#include "treemodel.h"
#include "sv_device_editor.h"
#include "sv_repositories.h"
#include "sv_signal_editor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  svlog::SvLog log;
  
  bool init();
  
private:
  Ui::MainWindow *ui;
  
  SvException _exception;
  
  QString _config_file_name = "config.xml";

  QString _config_path;
  
  QMenu *fileMenu;
  QMenu *configMenu;
  QMenu *helpMenu;
  
  QToolBar* toolBar;
  
  QAction *openAct;
  QAction *saveAsAct;
  QAction *exitAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  
  QAction *actionNewDevice;
  QAction *actionEditDevice;
  QAction *actionDeleteDevice;
  QAction *actionNewSignal;
  QAction *actionEditSignal;
  QAction *actionDeleteSignal;
  QAction *actionOpenRepositories;
//  QAction *action;
  
  
  TreeModel* _model;
  TreeItem* rootItem;
  
  void createActions();
  void createMenus();
  
  bool readConfig();
  
  
private slots:
  void newDevice();
  void editDevice();
  void deleteDevice();
  void newSignal();
  void editSignal();
  void deleteSignal();

  void openRepositories();
  
  
public slots:
    void saveAs();
    void about();

  
};

#endif // MAINWINDOW_H
