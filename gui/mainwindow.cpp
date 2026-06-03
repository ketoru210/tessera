#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Call into core once to confirm gui is actually linked against it (phase 0 done-criterion).
    setWindowTitle(QStringLiteral("Tessera %1").arg(tessera::core::version()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
