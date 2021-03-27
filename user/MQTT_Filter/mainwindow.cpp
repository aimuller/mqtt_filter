#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"

static u_int8_t buf[BUF_SIZE];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    active = 0;
    ui->label_mf_state->setText("系统状态: 关闭");
    fd = open_mf_dev();

    //if(fd < 0){
    //    QMessageBox::information(NULL, "错误", "打开设备文件失败！");
    //    exit(-1);
    //}

    getRuleFromKernel();
    updateCommonRule();

    addCommonRuleDialog = new CommonRuleDialog(this);
    modCommonRuleDialog = new CommonRuleDialog(this);

    ui->tableWidget_rule->setSelectionBehavior(QAbstractItemView::SelectRows);     /*整行选中*/
    ui->tableWidget_rule->horizontalHeader()->setStretchLastSection(true);   /*列宽度填满整个表格区域*/
    ui->tableWidget_rule->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->tableWidget_rule->setColumnWidth(0, 110);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(1, 80);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(2, 70);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(3, 170);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(4, 170);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(5, 200);//设置固定宽

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

void MainWindow::on_pushButton_mf_open_clicked()
{
    ioctl(fd, MF_SYS_OPEN);
    active = 1;
    ui->label_mf_state->setText("系统状态: 开启");
    getRuleFromKernel();
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

    u_int8_t *ptr = buf;
    *((unsigned int *)ptr) = pos;
    ptr = ptr + sizeof(unsigned int);

    *ptr = rule.mtype; 	ptr += sizeof(u_int8_t);
    *ptr = rule.action; ptr += sizeof(u_int8_t);
    *ptr = rule.log; 	ptr += sizeof(u_int8_t);
    *((u_int32_t *)ptr) = (rule.saddr);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.smask);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.daddr);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.dmask);	ptr += sizeof(u_int32_t);

    switch(rule.mtype){
    case CONNECT:
        *ptr = rule.deep.connect.flag;
        ptr += sizeof(u_int8_t);
        break;
    case PUBLISH:

        break;
    case SUBSCRIBE:

        break;
    case UNSUBSCRIBE:

        break;
    default:
        break;
    }

    ioctl(fd, MF_ADD_RULE, buf);

    rule_list.insert(pos - 1, rule);
    getRuleFromKernel();
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

    modCommonRuleDialog->setSourceRule(rule_list[pos]);

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

    u_int8_t *ptr = buf;
    *((unsigned int *)ptr) = mod_pos;
    ptr = ptr + sizeof(unsigned int);

    *ptr = rule.mtype; 	ptr += sizeof(u_int8_t);
    *ptr = rule.action; ptr += sizeof(u_int8_t);
    *ptr = rule.log; 	ptr += sizeof(u_int8_t);
    *((u_int32_t *)ptr) = (rule.saddr);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.smask);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.daddr);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.dmask);	ptr += sizeof(u_int32_t);

    switch(rule.mtype){
    case CONNECT:
        *ptr = rule.deep.connect.flag;
        ptr += sizeof(u_int8_t);
        break;
    case PUBLISH:

        break;
    case SUBSCRIBE:

        break;
    case UNSUBSCRIBE:

        break;
    default:
        break;
    }

    ioctl(fd, MF_ADD_RULE, buf);
    rule_list.insert(mod_pos - 1, rule);

    getRuleFromKernel();
    updateCommonRule();
}

void MainWindow::updateCommonRule()
{
    for(unsigned int i = 0; i < rule_list.size(); i++){
        setRuleItem(&rule_list[i], i);
    }
}

void MainWindow::getRuleFromKernel(){
    unsigned int len = 0;
    struct RULE_ST rule;
    u_int8_t *ptr = buf;

    rule_list.clear();
    ioctl(fd, MF_GET_RULE, buf);
    len = *((unsigned int *)ptr);
    ptr = ptr + sizeof(unsigned int);

    ui->tableWidget_rule->setRowCount(len);

    for(unsigned int i = 0; i < len; i++){
        rule.mtype = *ptr;  ptr += sizeof(u_int8_t);
        rule.action = *ptr;	ptr += sizeof(u_int8_t);
        rule.log = *ptr;	ptr += sizeof(u_int8_t);
        rule.saddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
        rule.smask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
        rule.daddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
        rule.dmask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);

        switch(rule.mtype){
        case CONNECT:
            rule.deep.connect.flag = *ptr;	ptr += sizeof(u_int8_t);
            break;
        case PUBLISH:

            break;
        case SUBSCRIBE:

            break;
        case UNSUBSCRIBE:

            break;
        default:
            break;
        }
        rule_list.push_back(rule);
    }
}

void MainWindow::setRuleItem(struct RULE_ST *rule, int row){
    ui->tableWidget_rule->setItem(row, 0, new QTableWidgetItem(rule2mtype(rule->mtype)));
    ui->tableWidget_rule->setItem(row, 1, new QTableWidgetItem(rule2action(rule->action)));
    ui->tableWidget_rule->setItem(row, 2, new QTableWidgetItem(rule2log(rule->log)));
    ui->tableWidget_rule->setItem(row, 3, new QTableWidgetItem(rule2addr(rule->saddr) + "/" + rule2mask(rule->smask)));
    ui->tableWidget_rule->setItem(row, 4, new QTableWidgetItem(rule2addr(rule->daddr) + "/" + rule2mask(rule->dmask)));

    switch(rule->mtype){
    case CONNECT:
        ui->tableWidget_rule->setItem(row, 5, new QTableWidgetItem(rule2cflag(rule->deep.connect.flag)));
        break;
    case PUBLISH:

        break;
    case SUBSCRIBE:

        break;
    case UNSUBSCRIBE:

        break;
    default:
        ui->tableWidget_rule->setItem(row, 5, new QTableWidgetItem("/"));
        break;
    }

    for(unsigned int i = 0; i < ui->tableWidget_rule->columnCount(); i++)
        ui->tableWidget_rule->item(row, i)->setTextAlignment(Qt::AlignCenter);
}

void MainWindow::on_pushButton_del_rule_clicked()
{
    int pos = ui->tableWidget_rule->currentRow() + 1;
    if(pos <= 0){
        QMessageBox::information(NULL, "提示", "请先点击要删除的规则");
        return;
    }

    QMessageBox::StandardButton choose = QMessageBox::question(NULL, "删除规则", "确定删除？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(choose == QMessageBox::No){
        return;
    }

    ioctl(fd, MF_DELETE_RULE, (unsigned int)pos);
    rule_list.removeAt(pos - 1);
    getRuleFromKernel();
    updateCommonRule();
}

void MainWindow::on_pushButton_clear_rule_clicked()
{
    QMessageBox::StandardButton choose = QMessageBox::question(NULL, "清除规则", "确定要清除所有规则？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(choose == QMessageBox::Yes){
        ioctl(fd, MF_CLEAR_RULE);
        rule_list.clear();
        getRuleFromKernel();
        updateCommonRule();
    }
}

void MainWindow::on_action_export_rule_file_triggered()
{
    QString filter = "rule文件(*.rule)";
    QString FilePath = QFileDialog::getSaveFileName(this, "保存文件", DEFAULT_DIR, filter);


    qDebug() << FilePath;
}
