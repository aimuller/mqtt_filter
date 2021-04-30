#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"

#define DEVIDE " % "
#define ITEM_NULL "---"

static char buf[BUF_SIZE];
int update_kdate = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    active = 0;

    ui->setupUi(this);
    ui->label_mf_state->setText("系统状态: 关闭");

    fd = open_mf_dev();
    if(fd < 0){
        QMessageBox::information(NULL, "错误", "打开设备文件失败！");
        exit(-1);
    }

    getSystemState();       //获取系统开启状态
    getRuleFromKernel();    //获取当前内核模块中的规则链表，存放到rule_list中
    showUserRuleList();     //打印rule_list中的规则链表

    addCommonRuleDialog = new CommonRuleDialog(this);
    modCommonRuleDialog = new CommonRuleDialog(this);
    logTimer = new QTimer(this);
    connectTimer = new QTimer(this);

    ui->tableWidget_rule->setSelectionBehavior(QAbstractItemView::SelectRows);     /*整行选中*/
    ui->tableWidget_rule->horizontalHeader()->setStretchLastSection(true);   /*列宽度填满整个表格区域*/
    ui->tableWidget_rule->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_rule->setMouseTracking(true);   //对鼠标进行监控，使得可以调用QToolTip

    ui->tableWidget_rule->setColumnWidth(0, 120);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(1, 70);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(2, 60);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(3, 160);//设置固定宽
    ui->tableWidget_rule->setColumnWidth(4, 160);//设置固定宽
    //ui->tableWidget_rule->setColumnWidth(5, 500);//设置固定宽

    ui->tableWidget_connect->setSelectionBehavior(QAbstractItemView::SelectRows);     /*整行选中*/
    ui->tableWidget_connect->horizontalHeader()->setStretchLastSection(true);   /*列宽度填满整个表格区域*/
    //ui->tableWidget_connect->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_connect->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget_connect->setMouseTracking(true);   //对鼠标进行监控，使得可以调用QToolTip

    ui->tableWidget_connect->setColumnWidth(0, 170);//设置固定宽
    ui->tableWidget_connect->setColumnWidth(1, 170);//设置固定宽
    ui->tableWidget_connect->setColumnWidth(2, 90);//设置固定宽
    ui->tableWidget_connect->setColumnWidth(3, 90);//设置固定宽
    ui->tableWidget_connect->setColumnWidth(4, 300);//设置固定宽
    //ui->tableWidget_rule->setColumnWidth(5, 500);//设置固定宽

    logTimer->start(500);
    connectTimer->start(500);

    connect(addCommonRuleDialog, SIGNAL(addCommonRuleSignal(struct RULE_ST, int)),
            this, SLOT(addCommonRule(struct RULE_ST, int)));
    connect(modCommonRuleDialog, SIGNAL(modCommonRuleSignal(struct RULE_ST, int)),
            this, SLOT(modCommonRule(struct RULE_ST, int)));
    connect(logTimer, SIGNAL(timeout()), this, SLOT(updateSystemLog()));
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(updateConnectList()));

}

MainWindow::~MainWindow()
{
    close_mf_dev(fd);
    delete ui;
}

void MainWindow::getSystemState(){
    ioctl(fd, MF_SYS_STATE, &active);
    if(active)
        ui->label_mf_state->setText("系统状态: 开启");
    else
        ui->label_mf_state->setText("系统状态: 关闭");
}

void MainWindow::on_pushButton_mf_open_clicked()
{
    ioctl(fd, MF_SYS_OPEN);
    active = 1;
    ui->label_mf_state->setText("系统状态: 开启");
    //getRuleFromKernel();
    //showUserRuleList();
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

void MainWindow::addCommonRule(struct RULE_ST rule, int pos)
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

    modCommonRuleDialog->setSourceRule(rule_list[pos], pos + 1);

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
    *((u_int16_t *)ptr) = (rule.sport);	ptr += sizeof(u_int16_t);
    *((u_int32_t *)ptr) = (rule.daddr);	ptr += sizeof(u_int32_t);
    *((u_int32_t *)ptr) = (rule.dmask);	ptr += sizeof(u_int32_t);
    *((u_int16_t *)ptr) = (rule.dport);	ptr += sizeof(u_int16_t);

    *ptr = rule.enabled_deep; 	ptr += sizeof(u_int8_t);

    if(rule.enabled_deep == ENABLED){
        switch(rule.mtype){
        case CONNECT:
            *ptr = rule.deep.connect.flag;
            ptr += sizeof(u_int8_t);

            if(rule.deep.connect.client_id != NULL){
                ba = rule.deep.connect.client_id->toLatin1();
                pchar = ba.data();
                strcpy((char *)ptr, pchar);
                ptr += (strlen(pchar) + 1);
            }
            else{
                *ptr = 0;
                ptr += sizeof(u_int8_t);
            }

            if(rule.deep.connect.username != NULL){
                ba = rule.deep.connect.username->toLatin1();
                pchar = ba.data();
                strcpy((char *)ptr, pchar);
                ptr += (strlen(pchar) + 1);
            }
            else{
                *ptr = 0;
                ptr += sizeof(u_int8_t);
            }

            if(rule.deep.connect.will_topic != NULL){
                ba = rule.deep.connect.will_topic->toLatin1();
                pchar = ba.data();
                strcpy((char *)ptr, pchar);
                ptr += (strlen(pchar) + 1);
            }
            else{
                *ptr = 0;
                ptr += sizeof(u_int8_t);
            }

            if(rule.deep.connect.will_message != NULL){
                ba = rule.deep.connect.will_message->toLatin1();
                pchar = ba.data();
                strcpy((char *)ptr, pchar);
                ptr += (strlen(pchar) + 1);
            }
            else{
                *ptr = 0;
                ptr += sizeof(u_int8_t);
            }

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

            if(rule.deep.publish.keyword != NULL){
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
    }
    //qDebug() << "buf len: " << (ptr - (u_int8_t *)buf);
}

void MainWindow::runShell(QString cmd){
    QProcess *shell = new QProcess(this);
    shell->setWorkingDirectory("../../sh");
    shell->start(cmd);
    shell->waitForFinished();
}

void MainWindow::updateSystemLog(){
    QString cmd = "log.sh";
    runShell(cmd);

    QFile logFile("../../data/log.txt");
    if(logFile.size() == log_size){
        logFile.close();
        return;
    }

    log_size = logFile.size();

    if(logFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        while(logFile.atEnd() == false){
            QString line = logFile.readLine();
            ui->textBrowser->append(line);
        }
    }
    else{
        QMessageBox::critical(this, "错误", "无法打开log.txt！");
        logTimer->stop();
    }
    logFile.close();
}

void MainWindow::updateConnectList(){
    getConnectFromKernel();
}

void MainWindow::modCommonRule(struct RULE_ST rule, int mod_pos)
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
    for(int i = 0; i < rule_list.size(); i++){
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
        rule.sport = *((u_int16_t *)ptr); ptr += sizeof(u_int16_t);
        rule.daddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
        rule.dmask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
        rule.dport = *((u_int16_t *)ptr); ptr += sizeof(u_int16_t);
        rule.enabled_deep = *ptr;  ptr += sizeof(u_int8_t);

        if(rule.enabled_deep == ENABLED){
            switch(rule.mtype){
            case CONNECT:
                rule.deep.connect.flag = *ptr;
                ptr += sizeof(u_int8_t);

                if(*ptr){
                    rule.deep.connect.client_id = new QString((char *)ptr);
                    ptr += (strlen((char *)ptr) + 1);
                }
                else{
                    rule.deep.connect.client_id = NULL;
                    ptr += sizeof(u_int8_t);
                }

                if(*ptr){
                    rule.deep.connect.username = new QString((char *)ptr);
                    ptr += (strlen((char *)ptr) + 1);
                }
                else{
                    rule.deep.connect.username = NULL;
                    ptr += sizeof(u_int8_t);
                }

                if(*ptr){
                    rule.deep.connect.will_topic = new QString((char *)ptr);
                    ptr += (strlen((char *)ptr) + 1);
                }
                else{
                    rule.deep.connect.will_topic = NULL;
                    ptr += sizeof(u_int8_t);
                }

                if(*ptr){
                    rule.deep.connect.will_message = new QString((char *)ptr);
                    ptr += (strlen((char *)ptr) + 1);
                }
                else{
                    rule.deep.connect.will_message = NULL;
                    ptr += sizeof(u_int8_t);
                }

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
        }
        rule_list.push_back(rule);
    }
}

void MainWindow::getConnectFromKernel(){
    unsigned int len = 0;
    u_int8_t *ptr = (u_int8_t *)buf;


    ioctl(fd, MF_GET_CONNECT, buf);
    len = *((unsigned int *)ptr);

    ptr += sizeof(unsigned int);

    ui->tableWidget_connect->setRowCount(len);

    for(unsigned int i = 0; i < len; i++){
        QString str;

        str = rule2addr(*((u_int32_t *)ptr));  ptr += sizeof(u_int32_t) ;
        str += ":";
        str += rule2port(*((u_int16_t *)ptr));  ptr += sizeof(u_int16_t) ;
        ui->tableWidget_connect->setItem(i, 0, new QTableWidgetItem(str));

        str = rule2addr(*((u_int32_t *)ptr));  ptr += sizeof(u_int32_t) ;
        str += ":";
        str += rule2port(*((u_int16_t *)ptr));  ptr += sizeof(u_int16_t) ;
        ui->tableWidget_connect->setItem(i, 1, new QTableWidgetItem(str));

        str = QString::number(uint(*ptr), 2);   ptr += sizeof(u_int8_t);
        ui->tableWidget_connect->setItem(i, 2, new QTableWidgetItem(str));

        str = QString::number(uint(*((u_int16_t *)ptr)));   ptr += sizeof(u_int16_t);
        ui->tableWidget_connect->setItem(i, 3, new QTableWidgetItem(str));

        if(*ptr){
            ui->tableWidget_connect->setItem(i, 4, new QTableWidgetItem(QString((char *)ptr)));
            ptr += (strlen((char *)ptr) + 1);
        }
        else{
            ui->tableWidget_connect->setItem(i, 4, new QTableWidgetItem("/"));
            ptr += sizeof(u_int8_t);
        }

        if(*ptr){
            ui->tableWidget_connect->setItem(i, 5, new QTableWidgetItem(QString((char *)ptr)));
            ptr += (strlen((char *)ptr) + 1);
        }
        else{
            ui->tableWidget_connect->setItem(i, 5, new QTableWidgetItem("/"));
            ptr += sizeof(u_int8_t);
        }

        if(*ptr){
            ui->tableWidget_connect->setItem(i, 6, new QTableWidgetItem(QString((char *)ptr)));
            ptr += (strlen((char *)ptr) + 1);
        }
        else{
            ui->tableWidget_connect->setItem(i, 6, new QTableWidgetItem("/"));
            ptr += sizeof(u_int8_t);
        }

        if(*ptr){
            ui->tableWidget_connect->setItem(i, 7, new QTableWidgetItem(QString((char *)ptr)));
            ptr += (strlen((char *)ptr) + 1);
        }
        else{
            ui->tableWidget_connect->setItem(i, 7, new QTableWidgetItem("/"));
            ptr += sizeof(u_int8_t);
        }

        for(int j = 0; j < ui->tableWidget_connect->columnCount(); j++){
            QTableWidgetItem* pItem = ui->tableWidget_connect->item(i, j);
            pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
            pItem->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MainWindow::setRuleItem(struct RULE_ST *rule, int row){
    QString str = "/";

    ui->tableWidget_rule->setItem(row, 0, new QTableWidgetItem(rule2mtype(rule->mtype)));
    ui->tableWidget_rule->setItem(row, 1, new QTableWidgetItem(rule2action(rule->action)));
    ui->tableWidget_rule->setItem(row, 2, new QTableWidgetItem(rule2log(rule->log)));
    ui->tableWidget_rule->setItem(row, 3, new QTableWidgetItem(rule2addr(rule->saddr) + "/" + rule2mask(rule->smask) + " : " + rule2port(rule->sport)));
    ui->tableWidget_rule->setItem(row, 4, new QTableWidgetItem(rule2addr(rule->daddr) + "/" + rule2mask(rule->dmask) + " : " + rule2port(rule->dport)));

    if(rule->enabled_deep == ENABLED){
        switch(rule->mtype){
        case CONNECT:
            str = rule2conflag(rule->deep.connect.flag);
            if(rule->deep.connect.client_id != NULL)
                str += ",  Client ID = \"" + *(rule->deep.connect.client_id) + "\"";
            if(rule->deep.connect.username != NULL)
                str += ",  User Name = \"" + *(rule->deep.connect.username) + "\"";
            if(rule->deep.connect.will_topic != NULL)
                str += ",  Will Topic = \"" + *(rule->deep.connect.will_topic) + "\"";
            if(rule->deep.connect.will_message != NULL)
                str += ",  Will Message = \"" + *(rule->deep.connect.will_message) + "\"";
            break;
        case PUBLISH:
            str = rule2pubflag(rule->deep.publish.flag);
            if(rule->deep.publish.topic != NULL)
                str += ",  Topic = \"" + *(rule->deep.publish.topic) + "\"";
            if(rule->deep.publish.keyword != NULL)
                str += ",  Keyword = \"" + *(rule->deep.publish.keyword) + "\"";
            break;
        case SUBSCRIBE:
            if(rule->deep.subscribe.topic_filter != NULL){
                str = "TopicFilter = \"";
                str += *(rule->deep.subscribe.topic_filter) + "\",  Requested QoS = ";
                str += QString::number(rule->deep.subscribe.rqos);
            }
            break;
        case UNSUBSCRIBE:
            if(rule->deep.unsubscribe.topic_filter != NULL){
                str = "TopicFilter = \"";
                str += *(rule->deep.unsubscribe.topic_filter) + "\"";
            }
            break;
        default:
            break;
        }
    }
    ui->tableWidget_rule->setItem(row, 5, new QTableWidgetItem(str));
    for(int i = 0; i < ui->tableWidget_rule->columnCount(); i++){
        QTableWidgetItem* pItem = ui->tableWidget_rule->item(row, i);
        pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
        pItem->setTextAlignment(Qt::AlignCenter);
    }
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
    if(rule_list[pos].enabled_deep == ENABLED){
        switch(rule_list[pos].mtype){
        case CONNECT:
            if(rule_list[pos].deep.connect.client_id != NULL){
                delete rule_list[pos].deep.connect.client_id;
                rule_list[pos].deep.connect.client_id = NULL;
            }

            if(rule_list[pos].deep.connect.username != NULL){
                delete rule_list[pos].deep.connect.username;
                rule_list[pos].deep.connect.username = NULL;
            }

            if(rule_list[pos].deep.connect.will_topic != NULL){
                delete rule_list[pos].deep.connect.will_topic;
                rule_list[pos].deep.connect.will_topic = NULL;
            }

            if(rule_list[pos].deep.connect.will_message != NULL){
                delete rule_list[pos].deep.connect.will_message;
                rule_list[pos].deep.connect.will_message = NULL;
            }

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
}

void MainWindow::on_pushButton_clear_rule_clicked()
{
    QMessageBox::StandardButton choose = QMessageBox::question(NULL, "清除规则", "确定要清除所有规则？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(choose == QMessageBox::Yes){
        ioctl(fd, MF_CLEAR_RULE);

        for(int i = 0; i < rule_list.size(); i++)
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
    if(RuleFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)){
        for(int i = 0; i < rule_list.size(); i++){
            QString rule_str;

            rule_str += rule2mtype(rule_list[i].mtype) + DEVIDE;
            rule_str += rule2action(rule_list[i].action) + DEVIDE;
            rule_str += rule2log(rule_list[i].log) + DEVIDE;

            rule_str += rule2addr(rule_list[i].saddr) +  "/" + rule2mask(rule_list[i].smask) + DEVIDE;
            rule_str += rule2port(rule_list[i].sport) + DEVIDE;

            rule_str += rule2addr(rule_list[i].daddr) +  "/" + rule2mask(rule_list[i].dmask) + DEVIDE;
            rule_str += rule2mask(rule_list[i].dport) + DEVIDE;

            rule_str += QString::number(rule_list[i].enabled_deep, 2);

            if(rule_list[i].enabled_deep == ENABLED){
                switch(rule_list[i].mtype){
                case CONNECT:
                    rule_str += DEVIDE + QString::number(rule_list[i].deep.connect.flag, 2);

                    if(rule_list[i].deep.connect.client_id != NULL)
                        rule_str += DEVIDE + *(rule_list[i].deep.connect.client_id);
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);

                    if(rule_list[i].deep.connect.username != NULL)
                        rule_str += DEVIDE + *(rule_list[i].deep.connect.username);
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);

                    if(rule_list[i].deep.connect.will_topic != NULL)
                        rule_str += DEVIDE + *(rule_list[i].deep.connect.will_topic);
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);

                    if(rule_list[i].deep.connect.will_message != NULL)
                        rule_str += DEVIDE + *(rule_list[i].deep.connect.will_message);
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);

                    break;
                case PUBLISH:
                    rule_str += DEVIDE + QString::number(rule_list[i].deep.publish.flag, 2);
                    if(rule_list[i].deep.publish.topic != NULL)
                        rule_str += DEVIDE + *(rule_list[i].deep.publish.topic);
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);
                    if(rule_list[i].deep.publish.keyword != NULL)
                        rule_str += DEVIDE + *(rule_list[i].deep.publish.keyword);
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);
                    break;
                case SUBSCRIBE:
                    if(rule_list[i].deep.subscribe.topic_filter != NULL){
                        rule_str += DEVIDE + *(rule_list[i].deep.subscribe.topic_filter);
                        rule_str += DEVIDE + QString::number(rule_list[i].deep.subscribe.rqos, 2);
                    }
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);
                    break;
                case UNSUBSCRIBE:
                    if(rule_list[i].deep.unsubscribe.topic_filter != NULL){
                       rule_str += DEVIDE + *(rule_list[i].deep.unsubscribe.topic_filter);
                    }
                    else
                        rule_str += DEVIDE + QString(ITEM_NULL);
                    break;
                default:
                    break;
                }
            }
            rule_str += "\n";
            RuleFile.write(rule_str.toStdString().data());
        }
    }
    RuleFile.close();
}

void MainWindow::on_action_Import_rule_file_triggered()
{

    //QMessageBox::StandardButton choose = QMessageBox::question(NULL, "清除规则", "是否清除当前规则表？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    //if(choose == QMessageBox::No){
        //return;
    //}
    ioctl(fd, MF_CLEAR_RULE);
    for(int i = 0; i < rule_list.size(); i++)
        free_qstring(i);
    rule_list.clear();


    QString filter = tr("规则文件(*.rule);;全部文件(*);;");
    QString FilePath = QFileDialog::getOpenFileName(this, "导入文件", DEFAULT_DIR, filter);
    if(FilePath.isEmpty()){
        return;
    }

    QFile RuleFile(FilePath);
    if(RuleFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        while(RuleFile.atEnd() == false){
            QString rule_str = RuleFile.readLine();
            rule_str = rule_str.trimmed();

            QStringList rule_items = rule_str.split(DEVIDE);
            struct RULE_ST rule;
            bool OK;

            rule.mtype = mtype2rule(rule_items[0]);
            rule.action = action2rule(rule_items[1]);
            rule.log = log2rule(rule_items[2]);
            addr2rule(rule_items[3], rule.saddr, rule.smask);
            rule.sport = port2rule(rule_items[4]);
            addr2rule(rule_items[5], rule.daddr, rule.dmask);
            rule.dport = port2rule(rule_items[6]);
            rule.enabled_deep = (u_int8_t)rule_items[7].toUInt(&OK, 2);

            if(rule.enabled_deep){
                switch(rule.mtype){
                case CONNECT:
                    rule.deep.connect.flag = (u_int8_t)rule_items[8].toUInt(&OK, 2);
                    if(rule_items[9] == QString(ITEM_NULL))
                        rule.deep.connect.client_id = NULL;
                    else
                        rule.deep.connect.client_id = new QString(rule_items[9]);

                    if(rule_items[10] == QString(ITEM_NULL))
                        rule.deep.connect.username = NULL;
                    else
                        rule.deep.connect.username = new QString(rule_items[10]);

                    if(rule_items[11] == QString(ITEM_NULL))
                        rule.deep.connect.will_topic = NULL;
                    else
                        rule.deep.connect.will_topic = new QString(rule_items[11]);

                    if(rule_items[12] == QString(ITEM_NULL))
                        rule.deep.connect.will_message = NULL;
                    else
                        rule.deep.connect.will_message = new QString(rule_items[12]);

                    break;
                case PUBLISH:
                    rule.deep.publish.flag = (u_int8_t)rule_items[8].toUInt(&OK, 2);

                    if(rule_items[9] == QString(ITEM_NULL))
                        rule.deep.publish.topic = NULL;
                    else
                        rule.deep.publish.topic = new QString(rule_items[9]);
                    if(rule_items[10] == QString(ITEM_NULL)){
                        rule.deep.publish.keyword = NULL;
                    }
                    else
                        rule.deep.publish.keyword = new QString(rule_items[10]);
                    break;

                case SUBSCRIBE:
                    if(rule_items[8] == QString(ITEM_NULL)){
                        rule.deep.subscribe.topic_filter = NULL;
                        rule.deep.subscribe.rqos = 0;
                    }
                    else{
                        rule.deep.subscribe.topic_filter = new QString(rule_items[8]);
                        rule.deep.subscribe.rqos = (u_int8_t)rule_items[9].toUInt(&OK, 2);
                    }
                    break;
                case UNSUBSCRIBE:
                    if(rule_items[8] == QString(ITEM_NULL))
                        rule.deep.unsubscribe.topic_filter = NULL;
                    else
                        rule.deep.unsubscribe.topic_filter = new QString(rule_items[8]);
                    break;
                default:
                    break;
                }
            }
            rule_list.append(rule);
        }
    }
    RuleFile.close();
    setRuleListToBuffer();
    ioctl(fd, MF_ADD_LIST, buf);

    getRuleFromKernel();
    showUserRuleList();
}

void MainWindow::setRuleListToBuffer(){
    QByteArray ba;
    char *pchar;
    u_int8_t *ptr = (u_int8_t *)buf;

    *((unsigned int *)ptr) = (unsigned int)rule_list.size();
    ptr = ptr + sizeof(unsigned int);

    for(int i = 0; i < rule_list.size(); i++){
        *ptr = rule_list[i].mtype; 	ptr += sizeof(u_int8_t);
        *ptr = rule_list[i].action; ptr += sizeof(u_int8_t);
        *ptr = rule_list[i].log; 	ptr += sizeof(u_int8_t);
        *((u_int32_t *)ptr) = (rule_list[i].saddr);	ptr += sizeof(u_int32_t);
        *((u_int32_t *)ptr) = (rule_list[i].smask);	ptr += sizeof(u_int32_t);
        *((u_int16_t *)ptr) = (rule_list[i].sport);	ptr += sizeof(u_int16_t);
        *((u_int32_t *)ptr) = (rule_list[i].daddr);	ptr += sizeof(u_int32_t);
        *((u_int32_t *)ptr) = (rule_list[i].dmask);	ptr += sizeof(u_int32_t);
        *((u_int16_t *)ptr) = (rule_list[i].dport);	ptr += sizeof(u_int16_t);
        *ptr = rule_list[i].enabled_deep; ptr += sizeof(u_int8_t);

        if(rule_list[i].enabled_deep == ENABLED){
            switch(rule_list[i].mtype){
            case CONNECT:
                *ptr = rule_list[i].deep.connect.flag;
                ptr += sizeof(u_int8_t);

                if(rule_list[i].deep.connect.client_id != NULL){
                    ba = rule_list[i].deep.connect.client_id->toLatin1();
                    pchar = ba.data();
                    strcpy((char *)ptr, pchar);
                    ptr += (strlen(pchar) + 1);
                }
                else{
                    *ptr = 0;
                    ptr += sizeof(u_int8_t);
                }

                if(rule_list[i].deep.connect.username != NULL){
                    ba = rule_list[i].deep.connect.username->toLatin1();
                    pchar = ba.data();
                    strcpy((char *)ptr, pchar);
                    ptr += (strlen(pchar) + 1);
                }
                else{
                    *ptr = 0;
                    ptr += sizeof(u_int8_t);
                }

                if(rule_list[i].deep.connect.will_topic != NULL){
                    ba = rule_list[i].deep.connect.will_topic->toLatin1();
                    pchar = ba.data();
                    strcpy((char *)ptr, pchar);
                    ptr += (strlen(pchar) + 1);
                }
                else{
                    *ptr = 0;
                    ptr += sizeof(u_int8_t);
                }

                if(rule_list[i].deep.connect.will_message != NULL){
                    ba = rule_list[i].deep.connect.will_message->toLatin1();
                    pchar = ba.data();
                    strcpy((char *)ptr, pchar);
                    ptr += (strlen(pchar) + 1);
                }
                else{
                    *ptr = 0;
                    ptr += sizeof(u_int8_t);
                }

                break;
            case PUBLISH:
                *ptr = rule_list[i].deep.publish.flag;
                ptr += sizeof(u_int8_t);

                if(rule_list[i].deep.publish.topic != NULL){
                    ba = rule_list[i].deep.publish.topic->toLatin1();
                    pchar = ba.data();
                    strcpy((char *)ptr, pchar);
                    ptr += (strlen(pchar) + 1);
                }
                else{
                    *ptr = 0;
                    ptr += sizeof(u_int8_t);
                }

                if(rule_list[i].deep.publish.keyword != NULL){
                    ba = rule_list[i].deep.publish.keyword->toLatin1();
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
                if(rule_list[i].deep.subscribe.topic_filter != NULL){
                    ba = rule_list[i].deep.subscribe.topic_filter->toLatin1();
                    pchar = ba.data();
                    strcpy((char *)ptr, pchar);
                    ptr += (strlen(pchar) + 1);
                    *ptr = rule_list[i].deep.subscribe.rqos;
                    ptr += sizeof(u_int8_t);
                }
                else{
                    *ptr = 0;
                    ptr += sizeof(u_int8_t);
                }
                break;
            case UNSUBSCRIBE:
                if(rule_list[i].deep.unsubscribe.topic_filter != NULL){
                    ba = rule_list[i].deep.unsubscribe.topic_filter->toLatin1();
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
        }
    }
}

void MainWindow::on_tableWidget_rule_itemEntered(QTableWidgetItem *item)
{
    QString str = item->text();
    QStringList strlist;

    for (int i = 0; i < str.length(); i += 64) {
        strlist << str.mid(i, 64);
    }

    QToolTip::showText(QCursor::pos(), strlist.join("\n"));
}

void MainWindow::on_tableWidget_rule_doubleClicked(const QModelIndex &index)
{
    QString str = ui->tableWidget_rule->item(index.row(),index.column())->text();
    QStringList strlist;

    for (int i = 0; i < str.length(); i += 64) {
        strlist << str.mid(i, 64);
    }

    QToolTip::showText(QCursor::pos(), strlist.join("\n"));
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::StandardButton choose = QMessageBox::question(NULL, "清除日志", "是否清除当前日志？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(choose == QMessageBox::No){
        return;
    }
    QFile logFile("../../data/log.txt");
    bool ok = logFile.remove();
    if(ok)
        ui->textBrowser->clear();
    else{
        QMessageBox::information(NULL, "错误", "删除日志文件失败！");
    }
}

void MainWindow::on_pushButton_update_connect_clicked()
{
    getConnectFromKernel();
}

void MainWindow::on_tableWidget_connect_itemEntered(QTableWidgetItem *item)
{
    QString str = item->text();
    QStringList strlist;

    for (int i = 0; i < str.length(); i += 64) {
        strlist << str.mid(i, 64);
    }

    QToolTip::showText(QCursor::pos(), strlist.join("\n"));
}
