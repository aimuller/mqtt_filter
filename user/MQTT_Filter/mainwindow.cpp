#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    active = 0;
    ui->label_mf_state->setText("系统状态: 关闭");
    fd = open_mf_dev();


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
    active = 1;
    ui->label_mf_state->setText("系统状态: 开启");
}

void MainWindow::on_pushButton_mf_close_clicked()
{
    ioctl(fd, MF_SYS_CLOSE);
    active = 0;
    ui->label_mf_state->setText("系统状态: 关闭");
}

void MainWindow::on_pushButton_add_rule_clicked()
{
    if(!active)
        return;
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
    if(!active)
        return;
    modCommonRuleDialog->setMode(MOD_RULE);
    modCommonRuleDialog->setWindowTitle("修改规则");
    modCommonRuleDialog->exec();
}
void MainWindow::modCommonRule(struct RULE_ST rule)
{
    qDebug() << "modCommonRule" << endl;
}


