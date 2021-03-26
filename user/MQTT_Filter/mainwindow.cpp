#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"

static char buf[2048];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    active = 0;
    ui->label_mf_state->setText("系统状态: 关闭");
    fd = open_mf_dev();
    initRuleList();
    updateCommonRule();

    addCommonRuleDialog = new CommonRuleDialog(this);
    modCommonRuleDialog = new CommonRuleDialog(this);

    ui->tableWidget_rule->setSelectionBehavior(QAbstractItemView::SelectRows);     /*整行选中*/
    ui->tableWidget_rule->horizontalHeader()->setStretchLastSection(true);   /*列宽度填满整个表格区域*/
    ui->tableWidget_rule->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    //ui->tableWidget_rule->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);//对第0列单独设置固定宽度
    //ui->tableWidget_rule->setColumnWidth(0, 180);//设置固定宽
    //ui->tableWidget_rule->setColumnWidth(1, 180);//设置固定宽
    //ui->tableWidget_rule->setColumnWidth(2, 120);//设置固定宽

    connect(addCommonRuleDialog, SIGNAL(addCommonRuleSignal(struct RULE_ST, unsigned int)),
            this, SLOT(addCommonRule(struct RULE_ST, unsigned int)));
    connect(modCommonRuleDialog, SIGNAL(modCommonRuleSignal(struct RULE_ST, unsigned int)),
            this, SLOT(modCommonRule(struct RULE_ST, unsigned int)));
}

MainWindow::~MainWindow()
{
    close_mf_dev(fd);
    delete ui;
}

void MainWindow::initRuleList(){
    unsigned int len = 0;
    struct RULE_ST rule;
    char *pchar = buf;

    ioctl(fd, MF_GET_RULE, buf);
    len = *(unsigned int *)pchar;
    pchar = pchar + sizeof(unsigned int);

    for(unsigned int i = 0; i < len; i++){
        memcpy(&rule, pchar, sizeof(struct RULE_ST));
        rule_list.append(rule);
        pchar = pchar + sizeof(struct RULE_ST);
    }
    qDebug() << "initRuleList rule_num:" << rule_list.size();
}

void MainWindow::on_pushButton_mf_open_clicked()
{
    ioctl(fd, MF_SYS_OPEN);
    active = 1;
    ui->label_mf_state->setText("系统状态: 开启");
    updateCommonRule();
}

void MainWindow::on_pushButton_mf_close_clicked()
{
    ioctl(fd, MF_SYS_CLOSE);
    active = 0;
    ui->label_mf_state->setText("系统状态: 关闭");
}

void MainWindow::on_pushButton_add_rule_clicked()
{
    /*if(!active){
        QMessageBox::information(NULL, "提示", "请先点击\"开启过滤\"按钮开启系统");
        return;
    }*/
    addCommonRuleDialog->setMode(ADD_RULE);
    addCommonRuleDialog->setWindowTitle("插入规则");
    addCommonRuleDialog->exec();
}

void MainWindow::addCommonRule(struct RULE_ST rule, unsigned int pos)
{
    if(pos <= 0 || pos > ui->tableWidget_rule->rowCount() + 1)
        pos = ui->tableWidget_rule->rowCount() + 1;

    char *pchar = buf;
    *((unsigned int *)pchar) = pos;
    pchar = pchar + sizeof(unsigned int);
    memcpy(pchar, &rule, sizeof(struct RULE_ST));

    ioctl(fd, MF_ADD_RULE, buf);

    rule_list.insert(pos - 1, rule);
    updateCommonRule();

    //qDebug()<<rule.saddr<<rule.smask<<rule.daddr<<rule.dmask<<rule.mtype<<rule.log<<rule.action;
}

void MainWindow::on_pushButton_mod_rule_clicked()
{
    int pos = ui->tableWidget_rule->currentRow();
    if(pos < 0){
        QMessageBox::information(NULL, "提示", "请先点击要修改的规则");
        return;
    }

    modCommonRuleDialog->setSourceRule(ui->tableWidget_rule->item(pos, 0)->text(), 0);
    modCommonRuleDialog->setSourceRule(ui->tableWidget_rule->item(pos, 1)->text(), 1);
    modCommonRuleDialog->setSourceRule(ui->tableWidget_rule->item(pos, 2)->text(), 2);
    modCommonRuleDialog->setSourceRule(ui->tableWidget_rule->item(pos, 3)->text(), 3);
    modCommonRuleDialog->setSourceRule(ui->tableWidget_rule->item(pos, 4)->text(), 4);

    modCommonRuleDialog->setMode(MOD_RULE);
    modCommonRuleDialog->setWindowTitle("修改规则");
    modCommonRuleDialog->exec();

}

void MainWindow::modCommonRule(struct RULE_ST rule, unsigned int mod_pos)
{
    int src_pos = ui->tableWidget_rule->currentRow() + 1;

    if(mod_pos <= 0 || mod_pos > ui->tableWidget_rule->rowCount())
        mod_pos = src_pos;

    ioctl(fd, MF_DELETE_RULE, (unsigned int)src_pos);
    rule_list.removeAt(src_pos - 1);

    char *pchar = buf;
    *((unsigned int *)pchar) = mod_pos;
    pchar = pchar + sizeof(unsigned int);
    memcpy(pchar, &rule, sizeof(struct RULE_ST));

    ioctl(fd, MF_ADD_RULE, buf);
    rule_list.insert(mod_pos - 1, rule);

    updateCommonRule();
}

void MainWindow::updateCommonRule()
{
    unsigned int len = 0;
    struct RULE_ST rule;
    char *pchar = buf;

    ioctl(fd, MF_GET_RULE, buf);
    len = *(unsigned int *)pchar;
    pchar = pchar + sizeof(unsigned int);

    ui->tableWidget_rule->setRowCount(len);

    for(unsigned int i = 0; i < len; i++){
        memcpy(&rule, pchar, sizeof(struct RULE_ST));
        setRuleItem(&rule, i);
        pchar = pchar + sizeof(struct RULE_ST);
    }

}

void MainWindow::setRuleItem(struct RULE_ST *rule, int row){

    ui->tableWidget_rule->setItem(row, 0, new QTableWidgetItem(rule2ip(rule->saddr, rule->smask)));
    ui->tableWidget_rule->setItem(row, 1, new QTableWidgetItem(rule2ip(rule->daddr, rule->dmask)));
    ui->tableWidget_rule->setItem(row, 2, new QTableWidgetItem(rule2mtype(rule->mtype)));
    ui->tableWidget_rule->setItem(row, 3, new QTableWidgetItem(rule2log(rule->log)));
    ui->tableWidget_rule->setItem(row, 4, new QTableWidgetItem(rule2action(rule->action)));

    ui->tableWidget_rule->item(row, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_rule->item(row, 1)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_rule->item(row, 2)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_rule->item(row, 3)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_rule->item(row, 4)->setTextAlignment(Qt::AlignCenter);
}

void MainWindow::on_pushButton_del_rule_clicked()
{
    int pos = ui->tableWidget_rule->currentRow() + 1;
    if(pos <= 0){
        QMessageBox::information(NULL, "提示", "请先点击要删除的规则");
        return;
    }
    ioctl(fd, MF_DELETE_RULE, (unsigned int)pos);
    rule_list.removeAt(pos - 1);
    updateCommonRule();
}

void MainWindow::on_pushButton_clear_rule_clicked()
{
    QMessageBox::StandardButton choose = QMessageBox::question(NULL, "清除规则", "确定要清除所有规则？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(choose == QMessageBox::Yes){
        ioctl(fd, MF_CLEAR_RULE);
        rule_list.clear();
        updateCommonRule();
    }
}

void MainWindow::on_action_export_rule_file_triggered()
{
    QString filter = "rule文件(*.rule)";
    QString FilePath = QFileDialog::getSaveFileName(this, "保存文件", DEFAULT_DIR, filter);


    qDebug() << FilePath;
}
