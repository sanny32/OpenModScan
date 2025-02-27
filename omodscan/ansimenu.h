#ifndef ANSIMENU_H
#define ANSIMENU_H

#include <QMenu>

///
/// \brief The AnsiMenu class
///
class AnsiMenu : public QMenu
{
    Q_OBJECT

public:
    explicit AnsiMenu(QWidget *parent = nullptr);

    void selectCodepage(const QString& name);

signals:
    void codepageSelected(const QString& name);

protected:
    void changeEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void closeMenu();
    void resetState();
    void createSubMenu(QAction* a, const QStringList& encodings);

private:
    QAction* _actionArabic;
    QAction* _actionBaltic;
    QAction* _actionCeltic;
    QAction* _actionCyrillic;
    QAction* _actionCentralEuropean;
    QAction* _actionChinese;
    QAction* _actionEasternEuropean;
    QAction* _actionGreek;
    QAction* _actionHebrew;
    QAction* _actionJapanese;
    QAction* _actionKorean;
    QAction* _actionThai;
    QAction* _actionTurkish;
    QAction* _actionWesternEuropean;
    QAction* _actionVietnamese;
};

#endif // ANSIMENU_H
