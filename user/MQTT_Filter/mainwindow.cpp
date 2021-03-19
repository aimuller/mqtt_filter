#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_mf_state->setText("系统状态: 开启");

    addCommonRuleDialog = new CommonRuleDialog(this);
    modCommonRuleDialog = new CommonRuleDialog(this);


    connect(addCommonRuleDialog, SIGNAL(addCommonRuleSignal(struct RULE_ST)), this, SLOT(addCommonRule(struct RULE_ST)));
    connect(modCommonRuleDialog, SIGNAL(modCommonRuleSignal(struct RULE_ST)), this, SLOT(modCommonRule(struct RULE_ST)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_mf_open_clicked()
{
    ioctl(fd, MF_SYS_OPEN);
    ui->label_mf_state->setText("系统状态: 开启");
}

void MainWindow::on_pushButton_mf_close_clicked()
{
    ioctl(fd, MF_SYS_OPEN);
    ui->label_mf_state->setText("系统状态: 关闭");
}

void MainWindow::on_pushButton_add_rule_clicked()
{
    addCommonRuleDialog->setMode(ADD_RULE);
    addCommonRuleDialog->setWindowTitle("插入规则");
    addCommonRuleDialog->exec();
}

void MainWindow::addCommonRule(struct RULE_ST rule)
{
    qDebug() << "addCommonRule" << endl;
}

void MainWindow::on_pushButton_mod_rule_clicked()
{
    modCommonRuleDialog->setMode(MOD_RULE);
    modCommonRuleDialog->setWindowTitle("修改规则");
    modCommonRuleDialog->exec();
}
void MainWindow::modCommonRule(struct RULE_ST rule)
{
    qDebug() << "modCommonRule" << endl;
}


