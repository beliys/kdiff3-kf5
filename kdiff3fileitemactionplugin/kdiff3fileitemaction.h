/* This file is part of the KDiff3 project

   Copyright (C) 2008 Joachim Eibl <Joachim dot Eibl at gmx dot de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KDIFF3FILEITEMACTIONPLUGIN_H_
#define _KDIFF3FILEITEMACTIONPLUGIN_H_

#include <KAbstractFileItemActionPlugin>
#include <KFileItemListProperties>

class QStringList;

class KDiff3FileItemAction : public KAbstractFileItemActionPlugin
{
   Q_OBJECT
public:
  KDiff3FileItemAction (QObject* pParent, const QVariantList & args);
  virtual ~KDiff3FileItemAction();
  // implement pure virtual method from KonqPopupMenuPlugin
  virtual QList<QAction*> actions( const KFileItemListProperties& fileItemInfos, QWidget* pParentWidget ) Q_DECL_OVERRIDE;

private slots:
   void slotCompareWith();
   void slotCompareTwoFiles();
   void slotCompareThreeFiles();
   void slotMergeWith();
   void slotMergeThreeWay();
   void slotSaveForLater();
   void slotClearList();
   void slotCompareWithHistoryItem();
   void slotAbout();

private:
   QList<QUrl> m_list;
   QWidget* m_pParentWidget;
   //KFileItemListProperties m_fileItemInfos;
};
#endif
