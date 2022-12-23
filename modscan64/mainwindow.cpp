#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"

///
/// \brief MainWindow::MainWindow
/// \param parent
///
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);
}

///
/// \brief MainWindow::~MainWindow
///
MainWindow::~MainWindow()
{
    delete ui;
}

///
/// \brief MainWindow::closeEvent
/// \param event
///
void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow())
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

///
/// \brief MainWindow::on_actionNew_triggered
///
void MainWindow::on_actionNew_triggered()
{
    auto frm = createMdiChild();
    frm->show();
}

///
/// \brief MainWindow::updateMenus
///
void MainWindow::updateMenus()
{
}

///
/// \brief MainWindow::createMdiChild
/// \return
///
FormModSca* MainWindow::createMdiChild()
{
    auto child = new FormModSca(this);
    const auto title = QString("ModSca%1").arg(ui->mdiArea->subWindowList().count() + 1);
    child->setWindowTitle(title);
    ui->mdiArea->addSubWindow(child);
    return child;
}
