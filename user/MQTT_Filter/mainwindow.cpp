#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"

#define DEVIDE "#"

static char buf[BUF_SIZE];
int update_kdate = 1;

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
    //update_kdate = 0;
    showUserRuleList();

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
    showUserRuleList();
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

    setRuleToBuffer(rule, pos);

    ioctl(fd, MF_ADD_RULE, buf);
    rule_list.insert(pos - 1, rule);

    getRuleFromKernel();
    showUserRuleList();
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

void MainWindow::setRuleToBuffer(struct RULE_ST &rule, unsigned int pos){
    QByteArray ba;
    char *pchar;
    u_int8_t *ptr = (u_int8_t *)buf;

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
        *ptr = rule.deep.publish.flag;
        ptr += sizeof(u_int8_t);

        if(rule.deep.publish.topic != NULL){
            ba = rule.deep.publish.topic->toLatin1();
            pchar = ba.data();
            strcpy((char *)ptr, pchar);
            ptr += (strlen(pchar) + 1);
        }
        else{
            *ptr = 0;
            ptr += sizeof(u_int8_t);
        }

        if(rule.deep.publish.topic != NULL){
            ba = rule.deep.publish.keyword->toLatin1();
            pchar = ba.data();
            strcpy((char *)ptr, pchar);
            ptr += (strlen(pchar) + 1);
        }
        else{
            *ptr = 0;
            ptr += sizeof(u_int8_t);
        }
        break;
    case SUBSCRIBE:
        if(rule.deep.subscribe.topic_filter != NULL){
            ba = rule.deep.subscribe.topic_filter->toLatin1();
            pchar = ba.data();
            strcpy((char *)ptr, pchar);
            ptr += (strlen(pchar) + 1);
            *ptr = rule.deep.subscribe.rqos;
            ptr += sizeof(u_int8_t);
        }
        else{
            *ptr = 0;
            ptr += sizeof(u_int8_t);
        }
        break;
    case UNSUBSCRIBE:
        if(rule.deep.unsubscribe.topic_filter != NULL){
            ba = rule.deep.unsubscribe.topic_filter->toLatin1();
            pchar = ba.data();
            strcpy((char *)ptr, pchar);
            ptr += (strlen(pchar) + 1);
        }
        else{
            *ptr = 0;
            ptr += sizeof(u_int8_t);
        }
        break;
    default:
        break;
    }
    //qDebug() << "buf len: " << (ptr - (u_int8_t *)buf);
}

void MainWindow::modCommonRule(struct RULE_ST rule, unsigned int mod_pos)
{
    int src_pos = ui->tableWidget_rule->currentRow() + 1;
    if(mod_pos <= 0 || mod_pos > ui->tableWidget_rule->rowCount())
        mod_pos = src_pos;

    ioctl(fd, MF_DELETE_RULE, (unsigned int)src_pos);

    free_qstring(src_pos - 1);
    rule_list.removeAt(src_pos - 1);

    setRuleToBuffer(rule, mod_pos);
    ioctl(fd, MF_ADD_RULE, buf);
    rule_list.insert(mod_pos - 1, rule);

    getRuleFromKernel();
    showUserRuleList();
}



void MainWindow::showUserRuleList()
{
    ui->tableWidget_rule->setRowCount(rule_list.size());
    for(unsigned int i = 0; i < rule_list.size(); i++){
        setRuleItem(&rule_list[i], i);
    }
}

void MainWindow::getRuleFromKernel(){
    if(!update_kdate)
        return;
    unsigned int len = 0;
    struct RULE_ST rule;
    u_int8_t *ptr = (u_int8_t *)buf;

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
            rule.deep.connect.flag = *ptr;
            ptr += sizeof(u_int8_t);
            break;
        case PUBLISH:
            rule.deep.publish.flag = *ptr;
            ptr += sizeof(u_int8_t);

            if(*ptr){
                rule.deep.publish.topic = new QString((char *)ptr);
                ptr += (strlen((char *)ptr) + 1);
            }
            else{
                rule.deep.publish.topic = NULL;
                ptr += sizeof(u_int8_t);
            }

            if(*ptr){
                rule.deep.publish.keyword = new QString((char *)ptr);
                ptr += (strlen((char *)ptr) + 1);
            }
            else{
                rule.deep.publish.keyword = NULL;
                ptr += sizeof(u_int8_t);
            }

            break;
        case SUBSCRIBE:
            if(*ptr){
                rule.deep.subscribe.topic_filter = new QString((char *)ptr);
                ptr += (strlen((char *)ptr) + 1);
                rule.deep.subscribe.rqos = *ptr;
                ptr += sizeof(u_int8_t);
            }
            else{
                rule.deep.subscribe.topic_filter = NULL;
                ptr += sizeof(u_int8_t);
            }
            break;
        case UNSUBSCRIBE:
            if(*ptr){
                rule.deep.unsubscribe.topic_filter = new QString((char *)ptr);
                ptr += (strlen((char *)ptr) + 1);
            }
            else{
                rule.deep.unsubscribe.topic_filter = NULL;
                ptr += sizeof(u_int8_t);
            }
            break;
        default:
            break;
        }
        rule_list.push_back(rule);
    }
}

void MainWindow::setRuleItem(struct RULE_ST *rule, int row){
    QString str = "/";

    ui->tableWidget_rule->setItem(row, 0, new QTableWidgetItem(rule2mtype(rule->mtype)));
    ui->tableWidget_rule->setItem(row, 1, new QTableWidgetItem(rule2action(rule->action)));
    ui->tableWidget_rule->setItem(row, 2, new QTableWidgetItem(rule2log(rule->log)));
    ui->tableWidget_rule->setItem(row, 3, new QTableWidgetItem(rule2addr(rule->saddr) + "/" + rule2mask(rule->smask)));
    ui->tableWidget_rule->setItem(row, 4, new QTableWidgetItem(rule2addr(rule->daddr) + "/" + rule2mask(rule->dmask)));

    switch(rule->mtype){
    case CONNECT:
        str = rule2conflag(rule->deep.connect.flag);
        ui->tableWidget_rule->setItem(row, 5, new QTableWidgetItem(str));
        break;
    case PUBLISH:
        str = rule2pubflag(rule->deep.publish.flag);
        if(rule->deep.publish.topic != NULL)
            str += ", Topic=\"" + *(rule->deep.publish.topic) + "\"";
        if(rule->deep.publish.keyword != NULL)
            str += ", Keyword=\"" + *(rule->deep.publish.keyword) + "\"";
        break;
    case SUBSCRIBE:
        if(rule->deep.subscribe.topic_filter != NULL){
            str = "TopicFilter=\"";
            str += *(rule->deep.subscribe.topic_filter) + "\",  Requested QoS=";
            str += QString::number(rule->deep.subscribe.rqos);
        }
        break;
    case UNSUBSCRIBE:
        if(rule->deep.unsubscribe.topic_filter != NULL){
            str = "TopicFilter=\"";
            str += *(rule->deep.unsubscribe.topic_filter) + "\"";
        }
        break;
    default:
        break;
    }

    ui->tableWidget_rule->setItem(row, 5, new QTableWidgetItem(str));
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

    free_qstring(pos - 1);
    rule_list.removeAt(pos - 1);

    getRuleFromKernel();
    showUserRuleList();
}

void MainWindow::free_qstring(int pos){
    switch(rule_list[pos].mtype){
    case PUBLISH:
        if(rule_list[pos].deep.publish.topic != NULL){
            delete rule_list[pos].deep.publish.topic;
            rule_list[pos].deep.publish.topic = NULL;
        }
        if(rule_list[pos].deep.publish.keyword != NULL){
            delete rule_list[pos].deep.publish.keyword;
            rule_list[pos].deep.publish.keyword = NULL;
        }
        break;
    case SUBSCRIBE:
        if(rule_list[pos].deep.subscribe.topic_filter != NULL){
            delete rule_list[pos].deep.subscribe.topic_filter;
            rule_list[pos].deep.subscribe.topic_filter = NULL;
        }
        break;
    case UNSUBSCRIBE:
        if(rule_list[pos].deep.unsubscribe.topic_filter != NULL){
            delete rule_list[pos].deep.unsubscribe.topic_filter;
            rule_list[pos].deep.unsubscribe.topic_filter = NULL;
        }
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_clear_rule_clicked()
{
    QMessageBox::StandardButton choose = QMessageBox::question(NULL, "清除规则", "确定要清除所有规则？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(choose == QMessageBox::Yes){
        ioctl(fd, MF_CLEAR_RULE);

        for(unsigned int i = 0; i < rule_list.size(); i++)
            free_qstring(i);
        rule_list.clear();

        getRuleFromKernel();
        showUserRuleList();
    }
}

void MainWindow::on_action_export_rule_file_triggered()
{
    QString filter = tr("规则文件(*.rule);;全部文件(*);;");
    QString FilePath = QFileDialog::getSaveFileName(this, "保存文件", DEFAULT_DIR, filter);
    if(FilePath.isEmpty()){
        return;
    }

    QFile RuleFile(FilePath);
    if(RuleFile.open(QIODevice::ReadWrite | QIODevice::Text)){
        for(unsigned int i = 0; i < rule_list.size(); i++){
            QString rule_str;

            rule_str += rule2mtype(rule_list[i].mtype) + DEVIDE;
            rule_str += rule2action(rule_list[i].action) + DEVIDE;
            rule_str += rule2log(rule_list[i].log) + DEVIDE;
            rule_str += rule2addr(rule_list[i].saddr) +  DEVIDE + rule2mask(rule_list[i].smask) + DEVIDE;
            rule_str += rule2addr(rule_list[i].daddr) +  DEVIDE + rule2mask(rule_list[i].dmask);

            switch(rule_list[i].mtype){
            case CONNECT:
                rule_str += DEVIDE + QString::number(rule_list[i].deep.connect.flag, 2);
                break;
            case PUBLISH:
                rule_str += DEVIDE + QString::number(rule_list[i].deep.publish.flag, 2);
                if(rule_list[i].deep.publish.topic != NULL)
                    rule_str += DEVIDE + *(rule_list[i].deep.publish.topic);
                if(rule_list[i].deep.publish.keyword != NULL)
                    rule_str += DEVIDE + *(rule_list[i].deep.publish.keyword);
                break;
            case SUBSCRIBE:
                if(rule_list[i].deep.subscribe.topic_filter != NULL){
                    rule_str += DEVIDE + *(rule_list[i].deep.subscribe.topic_filter);
                    rule_str += DEVIDE + QString::number(rule_list[i].deep.subscribe.rqos, 2);
                }
                break;
            case UNSUBSCRIBE:
                if(rule_list[i].deep.unsubscribe.topic_filter != NULL){
                   rule_str += DEVIDE + *(rule_list[i].deep.unsubscribe.topic_filter);
                }
                break;
            default:
                break;
            }
            rule_str += "\n";
            RuleFile.write(rule_str.toStdString().data());
        }
    }
    RuleFile.close();
}

void MainWindow::on_action_Import_rule_file_triggered()
{
    QString filter = tr("规则文件(*.rule);;全部文件(*);;");
    QString FilePath = QFileDialog::getOpenFileName(this, "导入文件", DEFAULT_DIR, filter);
    if(FilePath.isEmpty()){
        return;
    }

    QFile RuleFile(FilePath);
    if(RuleFile.open(QIODevice::ReadWrite | QIODevice::Text)){
        while(RuleFile.atEnd() == false){
            QString rule_str = RuleFile.readLine();
            QStringList rule_items = rule_str.split(DEVIDE);
            struct RULE_ST rule;
            bool OK;

            rule.mtype = mtype2rule(rule_items[0]);
            rule.action = action2rule(rule_items[1]);
            rule.log = log2rule(rule_items[2]);
            rule.saddr = addr2rule(rule_items[3]);
            rule.smask = mask2rule(rule_items[4]);
            rule.daddr = addr2rule(rule_items[5]);
            rule.dmask = mask2rule(rule_items[6]);

            switch(rule.mtype){
            case CONNECT:
                rule.deep.connect.flag = (u_int8_t)rule_items[7].toUInt(&OK, 2);
                break;
            case PUBLISH:
                rule.deep.publish.flag = (u_int8_t)rule_items[7].toUInt(&OK, 2);
                if(rule_items.size() > 8)
                    rule.deep.publish.topic = new QString(rule_items[8]);
                if(rule_items.size() > 9)
                    rule.deep.publish.keyword = new QString(rule_items[9]);
                break;
            case SUBSCRIBE:
                if(rule_items.size() > 7){
                    rule.deep.subscribe.topic_filter = new QString(rule_items[7]);
                    rule.deep.subscribe.rqos = (u_int8_t)rule_items[8].toUInt(&OK, 2);
                }
                break;
            case UNSUBSCRIBE:
                if(rule_items.size() > 7){
                    rule.deep.unsubscribe.topic_filter = new QString(rule_items[7]);
                }
                break;
            default:
                break;
            }
            rule_list.append(rule);
        }
    }

    RuleFile.close();
    showUserRuleList();

}
