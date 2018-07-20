/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "treeitem.h"
#include "treemodel.h"

//extern SvSQLITE* PGDB;

TreeModel::TreeModel(const QStringList &headers, QObject *parent)
    : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers)
    rootData << header.remove(0, 1);

  _rootItem = new TreeItem(rootData);
//  setupModelData(/*data.split(QString("\n")), */rootItem);
}

TreeModel::TreeModel(int headersCount, QObject *parent)
    : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  for(int i = 0; i < headersCount; i++)
    rootData << QString::number(i);

  _rootItem = new TreeItem(rootData);
//  setupModelData(/*data.split(QString("\n")), */rootItem);
}

TreeModel::~TreeModel()
{
    delete _rootItem;
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return _rootItem->columnCount();
}
//! [2]

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    
//    if (role != Qt::DissplayRole && role != Qt::EditRole)
//        return QVariant();
    TreeItem *item = getItem(index);;
    switch(role)
    {
      case Qt::DisplayRole:
      case Qt::EditRole:
        return item->data(index.column());
        break;
          
      case Qt::FontRole: {  
        
        QFont font;
        if((item->is_main_row && (item->info(index.column()).type != itSensorParams)) 
           || (item->info(index.column()).type == itSignalName))
          font.setBold(true);

//        if(item->info(index.column()).type == itEmployeeLoadFact)
//          if(item->item_state == TaskOnWork) font.setItalic(true);

        return font;
      }
        
      case Qt::TextAlignmentRole: {
        
        QStringList h = QString(TREE_HEADERS).split(';');
        
        if (index.column() < h.count()) {
          
          if(QString(h.at(index.column())).at(0) == '|') return int(Qt::AlignCenter | Qt::AlignVCenter);
          else if(QString(h.at(index.column())).at(0) == '>') return int(Qt::AlignRight | Qt::AlignVCenter);
          else return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
      }
        
      case Qt::ForegroundRole:
      {
        QBrush color;
         color.setColor(Qt::black);
         
//        if(item->info(index.column()).type == itTaskSum)
//          color.setColor(Qt::blue);

//        else if(item->info(index.column()).type == itEmployeeLoadFact)
////        {
//          if(item->item_state == TaskOnWork) color.setColor(Qt::red);
////          else color.setColor(Qt::black);
////        }
        return color;
      }
        
      case Qt::BackgroundRole:
        if(item->is_main_row) {
          
            QBrush color(QColor(240, 240, 240));
            return color;
        }
        
//        else if(item->info(index.column()).type == itTaskSumFact)
//        {
//            QBrush color(QColor(170, 255, 255));
//            return color;
//        }
        break; 
        
      case Qt::DecorationRole:
        if(index.column() == 0)
        {
          switch (item->info(index.column()).type)
          {
            case itSensor:
              return QIcon(":/munich/icons/munich-icons/ico/blue/task_kp.ico");
              break;
              
            case itSignal:
              return QIcon(":/munich/icons/munich-icons/ico/blue/task_onwork.ico");
              break;
              
            case itSignalType:
              return QIcon(":/munich/icons/munich-icons/ico/blue/task_paused.ico");
              break;
              
            case itSensorIfcName:
              return QIcon(":/tree/icons/tick.png");
              break;
              
            default:
              break;
          }
        }
        break;
        
      default:
        return QVariant();
          
    }
}

//! [3]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    
    TreeItem* item = getItem(index);
    
    Qt::ItemFlags flags =  QAbstractItemModel::flags(index);
    
    return flags;
    
//    if(projectState != ProjectNew) return flags ;
    
//    switch (item->info(index.column()).type)
//    {
//      case itTaskName:
//      case itTaskBegin:
//      case itTaskLaboriousness:
//      case itEmployeeLoadPlan:
//        return flags |= Qt::ItemIsEditable;
//    }
}
//! [3]

//! [4]
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return _rootItem;
}

TreeItem *TreeModel::itemFormIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return _rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
  if(orientation != Qt::Horizontal /*&& role == Qt::DisplayRole*/) return QVariant();
  
  switch (role)
  {
    case Qt::DisplayRole:
      return _rootItem->data(section);
//      break;
      
    case Qt::FontRole:
    {
      QFont boldFont;
      boldFont.setBold(true);
      return boldFont;
//      break; 
    }
      
    case Qt::TextAlignmentRole:
      return Qt::AlignCenter;
//      break;
      
    default:
      return QVariant();
//      break;
  }
}

//! [5]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
//! [5]

//! [6]
    TreeItem *parentItem = getItem(parent);

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = _rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, _rootItem->columnCount());
    endInsertRows();

    return success;
}

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}
//! [7]

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = _rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (_rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}
//! [8]

//bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if (role != Qt::EditRole) return false;

//    TreeItem *item = getItem(index);

//    QString sql;
//    switch (item->info(index.column()).type)
//    {
//      case itTaskId:
//      case itTaskAuthor:
//        break;
      
//      case itTaskName:
//        sql = QString("update tasks set task_name = '%1' where task_name = '%2'")
//            .arg(value.toString())
//            .arg(item->data(index.column()).toString());
//        break;
        
//      case itTaskBegin:
////        QString s = QDateTime::fromString(value.toString()).toString("dd/MM/yyyy hh:mm");
        
////        if(s == "")
////        {
////          QMessageBox::critical(0, "Ошибка", "Неверный формат даты / времени", QMessageBox::Ok);
////          return false;
////        }          
          
//        sql = QString("update tasks set task_begin = '%1' where task_name = '%2'")
//            .arg(value.toString())
//            .arg(item->data(index.column()).toString());
//        break;
        
//      case itTaskLaboriousness:
//        sql = QString("update tasks set laboriousness = %1 where id = %2")
//            .arg(value.toFloat())
//            .arg(item->id);
//        break;           
        
//      case itEmployeeLoadPlan:
//        sql = QString("update general set %1 = %2 where task_id = %3")
//            .arg(item->info(index.column()).fieldName)
//            .arg(value.toFloat())
//            .arg(item->id);
//        break;
        
//      default:
//        return false;
//    }
    
//    // сохраняем новое значение в базу
//    QSqlError err = PGDB->execSQL(sql);
//    if(err.type() != QSqlError::NoError) return false;
    
//    // если изменилось значение трудоемкости (плановое), то высчитываем их сумму для задачи
//    if(item->info(index.column()).type == itEmployeeLoadPlan)
//    {
//      // ищем номер столбца суммы плановой трудоемкости для задачи
//      int i = 0;
//      while(item->info(i).type != itTaskSum) i++;
      
//      QSqlQuery* q = new QSqlQuery(PGDB->db);
//      QSqlError err = PGDB->execSQL(QString("select task_sum from get_task_sum(%1)")
//                                    .arg(item->id), q);
//      q->first();
      
//      // заносим полученные значение в столбцы суммы
//      item->setData(i, q->value("task_sum").toFloat());
      
//      q->finish();
//      free(q);
      
//    }

//    bool result = item->setData(index.column(), value);
    
//    if (result) emit dataChanged(index, index);
    
//    return result;
//}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = _rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}


