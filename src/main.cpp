/***************************************************************************
                          main.cpp  -  Where everything starts.
                             -------------------
    begin                : Don Jul 11 12:31:29 CEST 2002
    copyright            : (C) 2002-2007 by Joachim Eibl
    email                : joachim.eibl at gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "version.h"
#include "kdiff3_shell.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QClipboard>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kdiff3");

    KCrash::initialize();

    KAboutData aboutData(QStringLiteral("kdiff3"), i18n("KDiff3"),QStringLiteral(VERSION),i18n("Tool to Compare and Merge Files and Directories"), KAboutLicense::GPL_V2, i18n("(c) 2002-2014 Joachim Eibl"), QString(), QStringLiteral("http://kdiff3.sourceforge.net/"), QStringLiteral("fischer@unix-ag.uni-kl.de"));
    aboutData.setOrganizationDomain("kde.org");
   
    aboutData.addAuthor(i18n("Joachim Eibl"), QString(), QStringLiteral("joachim.eibl@gmx.de"));
    aboutData.addCredit(i18n("Thomas Fischer"), i18n("Port to KDE Frameworks 5"), QStringLiteral("fischer@unix-ag.uni-kl.de"));
    aboutData.addCredit(i18n("Eike Sauer"), i18n("Bugfixes, Debian package maintainer") );
    aboutData.addCredit(i18n("Sebastien Fricker"), i18n("Windows installer") );
    aboutData.addCredit(i18n("Stephan Binner"), i18n("i18n-help"), QStringLiteral("binner@kde.org") );
    aboutData.addCredit(i18n("Stefan Partheymueller"), i18n("Clipboard-patch" ));
    aboutData.addCredit(i18n("David Faure"), i18n("KIO-Help"), QStringLiteral("faure@kde.org" ));
    aboutData.addCredit(i18n("Bernd Gehrmann"), i18n("Class CvsIgnoreList from Cervisia" ));
    aboutData.addCredit(i18n("Andre Woebbeking"), i18n("Class StringMatcher" ));
    aboutData.addCredit(i18n("Michael Denio"), i18n("Directory Equality-Coloring patch"));
    aboutData.addCredit(i18n("Manfred Koehler"), i18n("Fix for slow startup on Windows"));
    aboutData.addCredit(i18n("Sergey Zorin"), i18n("Diff Ext for Windows"));
    aboutData.addCredit(i18n("Paul Eggert, Mike Haertel, David Hayes, Richard Stallman, Len Tower"), i18n("GNU-Diffutils"));
    aboutData.addCredit(i18n("Tino Boellsterling, Timothy Mee"), i18n("Intensive test, use and feedback"));
    aboutData.addCredit(i18n("Michael Schmidt"), i18n("Mac support"));
    aboutData.addCredit(i18n("Valentin Rusu"), i18n("KDE4 porting"), QStringLiteral("kde@rusu.info"));
    aboutData.addCredit(i18n("Albert Astals Cid"), i18n("KDE4 porting"), QStringLiteral("aacid@kde.org"));
    aboutData.addCredit(i18n("Silvan Scherrer"), i18n("OS2 port") );
    aboutData.addCredit(i18n("Nigel Stewart"), i18n("Cygwin support") );
    aboutData.addCredit(i18n("Maurice van der Pot"), i18n("Framework for automated tests") );
    aboutData.addCredit(i18n("+ Many thanks to those who reported bugs and contributed ideas!"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption(QStringList()<<QStringLiteral("m")<<QStringLiteral("merge"),i18n("Merge the input.")));
    parser.addOption(QCommandLineOption(QStringList()<<QStringLiteral("b")<<QStringLiteral("base"),i18n("Explicit base file. For compatibility with certain tools."),QStringLiteral("file")));
    parser.addOption(QCommandLineOption(QStringList()<<QStringLiteral("o")<<QStringLiteral("out")<<QStringLiteral("output"),i18n("Output file. Implies -m."),QStringLiteral("file")));
    parser.addOption(QCommandLineOption(QStringLiteral("auto"),i18n("No GUI if all conflicts are auto-solvable. Needs -o file.")));
    parser.addPositionalArgument(QStringLiteral("file1"), i18n("File1 to open (base, if not specified via --base)."));
    parser.addPositionalArgument(QStringLiteral("file2"), i18n("File2 to open."));
    parser.addPositionalArgument(QStringLiteral("file3"), i18n("File3 to open (optional)."));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

// TODO KF5
//    options.add( "qall",        ki18n("Don't solve conflicts automatically.") );
//    options.add( "L1 alias1",   ki18n("Visible name replacement for input file 1 (base).") );
//    options.add( "L2 alias2",   ki18n("Visible name replacement for input file 2.") );
//    options.add( "L3 alias3",   ki18n("Visible name replacement for input file 3.") );
//    options.add( "L" ).add( "fname alias", ki18n("Alternative visible name replacement. Supply this once for every input.") );
//    options.add( "cs string",   ki18n("Override a config setting. Use once for every setting. E.g.: --cs \"AutoAdvance=1\"") );
//    options.add( "confighelp",  ki18n("Show list of config settings and current values.") );
//    options.add( "config file", ki18n("Use a different config file.") );


    KDiff3Shell *p = new KDiff3Shell(&parser);
    p->show();
    p->setWindowState(p->windowState() | Qt::WindowActive);  // FIXME needed? Patch for ubuntu: window not active on startup

//app.installEventFilter( new CFilter );
  int retVal = app.exec();
  if (QApplication::clipboard()->text().size() == 0)
     QApplication::clipboard()->clear(); // FIXME needed? Patch for Ubuntu: Fix issue with Qt clipboard
  return retVal;
}

// Suppress warning with --enable-final
#undef VERSION
