/***************************************************************************
 * Copyright (C) 2003-2007 Joachim Eibl <joachim.eibl at gmx.de>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA 02110-1301, USA.           *
 ***************************************************************************/

#include "kdiff3_shell.h"
#include "kdiff3.h"
#include "kdiff3_part.h"

#include <QAction>
#include <QStatusBar>
#include <QCloseEvent>
#include <QApplication>
#include <QCommandLineParser>

#include <KMessageBox>
#include <KLocalizedString>
#include <KToggleAction>
#include <KToolBar>
#include <KShortcutsDialog>
#include <KConfigGroup>
#include <KEditToolBar>

KDiff3Shell::KDiff3Shell(QCommandLineParser *cmdLineParser, bool bCompleteInit)
    : KParts::MainWindow(), m_cmdLineParser(cmdLineParser)
{
    m_bUnderConstruction = true;
    // set the shell's ui resource file
    setXMLFile("kdiff3_shell.rc");

    // and a status bar
    statusBar()->show();

    m_part = new KDiff3Part(cmdLineParser, this, "KDiff3Part", this);
    m_widget = qobject_cast<KDiff3App *>(m_part->widget());

    // and integrate the part's GUI with the shell's
    createGUI(m_part);
    //toolBar()->setToolButtonStyle( Qt::ToolButtonIconOnly );

    // tell the KParts::MainWindow that this is indeed the main widget
    setCentralWidget(m_widget);

    if (bCompleteInit)
        m_widget->completeInit();
    connect(m_widget, SIGNAL(createNewInstance(const QString &, const QString &, const QString &)), this, SLOT(slotNewInstance(const QString &, const QString &, const QString &)));

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();
    m_bUnderConstruction = false;
}

KDiff3Shell::~KDiff3Shell()
{
}

bool KDiff3Shell::queryClose()
{
   if (m_part)
      return ((KDiff3App*)m_part->widget())->queryClose();
   else
      return true;
}

bool KDiff3Shell::queryExit()
{
   return true;
}

void KDiff3Shell::closeEvent(QCloseEvent*e)
{
   if ( queryClose() )
   {
      e->accept();
      bool bFileSaved = ((KDiff3App*)m_part->widget())->isFileSaved();
      bool bDirCompare = ((KDiff3App*)m_part->widget())->isDirComparison();
      QApplication::exit( bFileSaved || bDirCompare ? 0 : 1 );
   }
   else
      e->ignore();
}

void KDiff3Shell::optionsShowToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}

void KDiff3Shell::optionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void KDiff3Shell::optionsConfigureKeys()
{
    KShortcutsDialog::configure(actionCollection() /*, "kdiff3_shell.rc" */ );
}

void KDiff3Shell::optionsConfigureToolbars()
{
   KConfigGroup mainWindowGroup( KSharedConfig::openConfig(QStringLiteral("kdiff3_shell.rc")), "MainWindow" );
   saveMainWindowSettings( mainWindowGroup );

    // use the standard toolbar editor
    KEditToolBar dlg(factory());
    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(applyNewToolbarConfig()));
    dlg.exec();
}

void KDiff3Shell::applyNewToolbarConfig()
{
   KConfigGroup mainWindowGroup( KSharedConfig::openConfig(QStringLiteral("kdiff3_shell.rc")), "MainWindow" );
   applyMainWindowSettings( mainWindowGroup );
}

void KDiff3Shell::slotNewInstance( const QString& fn1, const QString& fn2, const QString& fn3 )
{
    KDiff3Shell *pKDiff3Shell = new KDiff3Shell(m_cmdLineParser, false);
    ((KDiff3App *)pKDiff3Shell->m_widget)->completeInit(fn1, fn2, fn3);
}

//#include "kdiff3_shell.moc"
