/* This file is part of the KDiff3 project

   Copyright (C) 2008 Joachim Eibl <joachim dot eibl at gmx dot de>

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

#include "kdiff3fileitemaction.h"

#include <QAction>
#include <QMenu>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>
#include <KProcess>

//#include <iostream>

static QStringList* s_pHistory=0;

class KDiff3PluginHistory
{
   KConfig* m_pConfig;
   KConfigGroup* m_pConfigGroup;
public:
   KDiff3PluginHistory()
   {
      m_pConfig = 0;
      if (s_pHistory==0)
      {
         //std::cout << "New History: " << instanceName << std::endl;
         s_pHistory = new QStringList;
         m_pConfig = new KConfig( "kdiff3fileitemactionrc", KConfig::SimpleConfig );
         m_pConfigGroup = new KConfigGroup( m_pConfig, "KDiff3Plugin" );
         *s_pHistory = m_pConfigGroup->readEntry("HistoryStack", QStringList() );
      }
   }

   ~KDiff3PluginHistory()
   {
      //std::cout << "Delete History" << std::endl;
      if ( s_pHistory && m_pConfigGroup )
         m_pConfigGroup->writeEntry("HistoryStack",*s_pHistory);
      delete s_pHistory;
      delete m_pConfigGroup;
      delete m_pConfig;
      s_pHistory = 0;
      m_pConfig = 0;
   }
};

static KDiff3PluginHistory s_history;

K_PLUGIN_FACTORY_WITH_JSON(KDiff3FileItemActionFactory, "kdiff3fileitemaction.json", registerPlugin<KDiff3FileItemAction>();)

KDiff3FileItemAction::KDiff3FileItemAction (QObject* pParent, const QVariantList & /*args*/)
: KAbstractFileItemActionPlugin(pParent)
{
}

QList<QAction*> KDiff3FileItemAction::actions( const KFileItemListProperties& fileItemInfos, QWidget* pParentWidget )
{
   QList< QAction* > actions;
   
   if (QStandardPaths::findExecutable("kdiff3").isNull ())
      return actions;

   KDiff3FileItemAction *pThis = const_cast<KDiff3FileItemAction*>(this);
   //pThis->m_fileItemInfos = fileItemInfos;
   pThis->m_pParentWidget = pParentWidget;

   QAction *pMenuAction = new QAction(QIcon::fromTheme("kdiff3"), i18n("KDiff3 ..."), pThis);
   QMenu *pActionMenu = new QMenu();
   pMenuAction->setMenu( pActionMenu );
   
   
   // remember currently selected files (copy to a QStringList)
   QList<QUrl> itemList = fileItemInfos.urlList();
   foreach(const QUrl& item, itemList)
   {
      //m_urlList.append( item.url() );
      pThis->m_list.append( item );
   }


   /* Menu structure:
      KDiff3 -> (1 File selected):  Save 'selection' for later comparison (push onto history stack)
                                    Compare 'selection' with first file on history stack.
                                    Compare 'selection' with -> choice from history stack
                                    Merge 'selection' with first file on history stack.
                                    Merge 'selection' with last two files on history stack.
                (2 Files selected): Compare 's1' with 's2'
                                    Merge 's1' with 's2'
                (3 Files selected): Compare 's1', 's2' and 's3'
   */

   QAction* pAction = 0;
   QString s;

   if(m_list.count() == 1)
   {
      int historyCount = s_pHistory ? s_pHistory->count() : 0;

      s = i18n("Compare with %1", (historyCount>0 ? s_pHistory->first() : QString()) );
      pAction = new QAction ( s,pThis );
      connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotCompareWith);
      pAction->setEnabled( m_list.count()>0 && historyCount>0 );
      pActionMenu->addAction(pAction);

      s = i18n("Merge with %1", historyCount>0 ? s_pHistory->first() : QString() );
      pAction = new QAction( s, pThis);
      connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotMergeWith);
      pAction->setEnabled( m_list.count()>0 && historyCount>0 );
      pActionMenu->addAction (pAction);

      s = i18n("Save '%1' for later", ( m_list.first().toDisplayString(QUrl::PreferLocalFile) ) );
      pAction = new QAction ( s, pThis);
      connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotSaveForLater);
      pAction->setEnabled( m_list.count()>0 );
      pActionMenu->addAction(pAction);

      pAction = new QAction (i18n("3-way merge with base"), pThis);
      connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotMergeThreeWay);
      pAction->setEnabled( m_list.count()>0 && historyCount>=2 );
      pActionMenu->addAction (pAction);

      if (s_pHistory && !s_pHistory->empty())
      {
         QAction* pHistoryMenuAction = new QAction( i18n("Compare with ..."), pThis );
         QMenu* pHistoryMenu = new QMenu();
         pHistoryMenuAction->setMenu( pHistoryMenu );
         pHistoryMenu->setEnabled( m_list.count()>0 && historyCount>0 );
         pActionMenu->addAction(pHistoryMenuAction);
         foreach (const QString &file, *s_pHistory) {
            pAction = new QAction(file, pThis);
            pAction->setData(file);
            connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotCompareWithHistoryItem);
            pHistoryMenu->addAction(pAction);
         }

         pAction = new QAction(i18n("Clear list"), pThis);
         connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotClearList);
         pActionMenu->addAction (pAction);
         pAction->setEnabled( historyCount>0 );
      }
   }
   else if(m_list.count() == 2)
   {
      pAction = new QAction (i18n("Compare"), pThis);
      connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotCompareTwoFiles);
      pActionMenu->addAction (pAction);
   }
   else if ( m_list.count() == 3 )
   {
      pAction = new QAction (i18n("3 way comparison"), pThis);
      connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotCompareThreeFiles);
      pActionMenu->addAction (pAction);
   }
   pAction = new QAction (i18n("About KDiff3 menu plugin ..."), pThis);
   connect(pAction, &QAction::triggered, this, &KDiff3FileItemAction::slotAbout);
   pActionMenu->addAction (pAction);

   //pMenu->addSeparator();
   //pMenu->addAction( pActionMenu );
   //pMenu->addSeparator();
   actions << pMenuAction;
   return actions;
}

KDiff3FileItemAction::~KDiff3FileItemAction ()
{
}

void KDiff3FileItemAction::slotCompareWith()
{
   if ( m_list.count() > 0 && s_pHistory && ! s_pHistory->empty() )
   {
      QStringList args;
      args << s_pHistory->first();
      args << m_list.first().toDisplayString(QUrl::PreferLocalFile);
      KProcess::startDetached("kdiff3", args);
   }
}

void KDiff3FileItemAction::slotCompareWithHistoryItem()
{
   const QAction* pAction = dynamic_cast<const QAction*>( sender() );
   if (!m_list.isEmpty() && pAction)
   {
      QStringList args;
      args << pAction->data().toString();
      args << m_list.first().toDisplayString(QUrl::PreferLocalFile);
      KProcess::startDetached ("kdiff3", args);
   }
}

void KDiff3FileItemAction::slotCompareTwoFiles()
{
   if (m_list.count() == 2)
   {
      QStringList args;
      args << m_list.first().toDisplayString(QUrl::PreferLocalFile);
      args << m_list.last().toDisplayString(QUrl::PreferLocalFile);
      KProcess::startDetached ("kdiff3", args);
   }
}

void KDiff3FileItemAction::slotCompareThreeFiles()
{
   if ( m_list.count() == 3 )
   {
      QStringList args;
      args << m_list.at(0).toDisplayString(QUrl::PreferLocalFile);
      args << m_list.at(1).toDisplayString(QUrl::PreferLocalFile);
      args << m_list.at(2).toDisplayString(QUrl::PreferLocalFile);
      KProcess::startDetached ("kdiff3", args);
   }
}

void KDiff3FileItemAction::slotMergeWith()
{
   if ( m_list.count() > 0 && s_pHistory && ! s_pHistory->empty() )
   {
      QStringList args;
      args << s_pHistory->first();
      args << m_list.first().toDisplayString(QUrl::PreferLocalFile);
      args << ( "-o" + m_list.first().toDisplayString(QUrl::PreferLocalFile) );
      KProcess::startDetached ("kdiff3", args);
   }
}

void KDiff3FileItemAction::slotMergeThreeWay()
{
   if ( m_list.count() > 0 && s_pHistory &&  s_pHistory->count()>=2 )
   {
      QStringList args;
      args << (*s_pHistory).at(1);
      args << (*s_pHistory).at(0);
      args << m_list.first().toDisplayString(QUrl::PreferLocalFile);
      args << ("-o" + m_list.first().toDisplayString(QUrl::PreferLocalFile));
      KProcess::startDetached ("kdiff3", args);
   }
}

void KDiff3FileItemAction::slotSaveForLater()
{
   if (!m_list.isEmpty() && s_pHistory)
   {
      while (s_pHistory->count()>=10) {
         s_pHistory->removeLast();
      }
      const QString file = m_list.first().toDisplayString(QUrl::PreferLocalFile);
      s_pHistory->removeAll(file);
      s_pHistory->prepend(file);
   }
}

void KDiff3FileItemAction::slotClearList()
{
   if (s_pHistory) {
      s_pHistory->clear();
   }
}

void KDiff3FileItemAction::slotAbout()
{
   QString s = i18n("KDiff3 File Item Action Plugin: Copyright (C) 2011 Joachim Eibl\n"
                    "KDiff3 homepage: http://kdiff3.sourceforge.net\n\n");
   s += i18n("Using the contextmenu extension:\n"
             "For simple comparison of two selected files choose \"Compare\".\n"
             "If the other file is somewhere else \"Save\" the first file for later. "
             "It will appear in the \"Compare With ...\" submenu. "
             "Then use \"Compare With\" on the second file.\n"
             "For a 3-way merge first \"Save\" the base file, then the branch to merge and "
             "choose \"3-way merge with base\" on the other branch which will be used as destination.\n"
             "Same also applies to directory comparison and merge.");
   KMessageBox::information(m_pParentWidget, s, i18n("About KDiff3 File Item Action Plugin") );
}

#include "kdiff3fileitemaction.moc"
