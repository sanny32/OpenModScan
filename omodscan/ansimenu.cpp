#include <QTimer>
#include <QApplication>
#include <QMouseEvent>
#include "ansimenu.h"

///
/// \brief AnsiMenu::AnsiMenu
///
AnsiMenu::AnsiMenu(QWidget *parent)
    : QMenu(parent)
{
    _actionArabic =             new QAction(tr("Arabic"), parent);
    _actionBaltic =             new QAction(tr("Baltic"), parent);
    _actionCeltic =             new QAction(tr("Celtic"), parent);
    _actionCyrillic =           new QAction(tr("Cyrillic"), parent);
    _actionCentralEuropean =    new QAction(tr("Central European"), parent);
    _actionChinese =            new QAction(tr("Chinese"), parent);
    _actionEasternEuropean =    new QAction(tr("Eastern European"), parent);
    _actionGreek =              new QAction(tr("Greek"), parent);
    _actionHebrew =             new QAction(tr("Hebrew"), parent);
    _actionJapanese =           new QAction(tr("Japanese"), parent);
    _actionKorean =             new QAction(tr("Korean"), parent);
    _actionThai =               new QAction(tr("Thai"), parent);
    _actionTurkish =            new QAction(tr("Turkish"), parent);
    _actionWesternEuropean =    new QAction(tr("Western European"), parent);
    _actionVietnamese =         new QAction(tr("Vietnamese"), parent);

    createSubMenu(_actionArabic,            { "ISO 8859-6", "Windows-1256" });
    createSubMenu(_actionBaltic,            { "ISO 8859-4", "ISO 8859-13", "Windows-1257" });
    createSubMenu(_actionCeltic,            { "ISO 8859-14" });
    createSubMenu(_actionCyrillic,          { "ISO 8859-5", "KOI8-R", "KOI8-U", "Windows-1251" });
    createSubMenu(_actionCentralEuropean,   { "Windows-1250" });
    createSubMenu(_actionChinese,           { "Big5" });
    createSubMenu(_actionEasternEuropean,   { "ISO 8859-2" });
    createSubMenu(_actionGreek,             { "ISO 8859-7", "Windows-1253" });
    createSubMenu(_actionHebrew,            { "ISO 8859-8", "Windows-1255" });
    createSubMenu(_actionJapanese,          { "Shift-JIS" });
    createSubMenu(_actionKorean,            { "EUC-KR" });
    createSubMenu(_actionThai,              { "TIS-620" });
    createSubMenu(_actionTurkish,           { "ISO 8859-3", "ISO 8859-9", "Windows-1254" });
    createSubMenu(_actionWesternEuropean,   { "ISO 8859-1", "ISO 8859-15", "Windows-1252" });
    createSubMenu(_actionVietnamese,        { "Windows-1258" });
}

///
/// \brief AnsiMenu::mousePressEvent
/// \param event
///
void AnsiMenu::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && menuAction())
    {
        menuAction()->trigger();
        closeMenu();
    }

    QMenu::mousePressEvent(event);
}

///
/// \brief AnsiMenu::closeMenu
///
void AnsiMenu::closeMenu()
{
    for (auto&& widget : QApplication::topLevelWidgets())
    {
        auto menu = qobject_cast<QMenu*>(widget);
        if (menu && menu->isVisible())
            menu->close();
    }
}

///
/// \brief AnsiMenu::createSubMenu
/// \param a
/// \param encodings
///
void AnsiMenu::createSubMenu(QAction* a, const QStringList& encodings)
{
    auto menu = new QMenu(this);
    for(auto&& e : encodings)
    {
        auto action = new QAction(e, a);
        action->setCheckable(true);

        menu->addAction(action);
        connect(action, &QAction::triggered, this, [this, action, a](){
            resetState();
            a->setChecked(true);
            action->setChecked(true);
            emit codepageSelected(action->text());
        });
    }

    a->setCheckable(true);
    a->setMenu(menu);

    addAction(a);
}

///
/// \brief AnsiMenu::selectCodepage
/// \param name
///
void AnsiMenu::selectCodepage(const QString& name)
{
    for(auto&& a : actions())
    {
        a->setChecked(false);
        for(auto&& ma : a->menu()->actions())
        {
            if(ma->text() == name)
            {
                a->setChecked(true);
                ma->setChecked(true);
            }
        }
    }
}

///
/// \brief AnsiMenu::resetState
///
void AnsiMenu::resetState()
{
    for(auto&& a : actions())
    {
        a->setChecked(false);
        for(auto&& ma : a->menu()->actions())
            ma->setChecked(false);
    }
}
