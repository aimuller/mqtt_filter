#include "commonruledialog.h"
#include "ui_commonruledialog.h"


CommonRuleDialog::CommonRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommonRuleDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
}

CommonRuleDialog::~CommonRuleDialog()
{
    delete ui;
}

void CommonRuleDialog::setMode(int arg){
    mode = arg;
    if(mode == ADD_RULE)
        ui->label_8->setText("位置(默认最后)");
    else if(mode == MOD_RULE){
        ui->label_8->setText("位置(默认原位置)");
    }
}
void CommonRuleDialog::setSourceRule(QString str, int col){
    QStringList strlist;
    switch(col){
    case 0:
        ui->comboBox_common_mtype->setCurrentText(str);
        if(str == "CONNECT")
            ui->stackedWidget->setCurrentWidget(ui->page_connect);
        else if(str == "PUBLISH")
            ui->stackedWidget->setCurrentWidget(ui->page_publish);
        else if(str == "SUBSCRIBE")
            ui->stackedWidget->setCurrentWidget(ui->page_subscribe);
        else if(str == "UNSUBSCRIBE")
            ui->stackedWidget->setCurrentWidget(ui->page_unsubscribe);
        break;
    case 1:
        ui->comboBox_common_action->setCurrentText(str);
        break;
    case 2:
        ui->comboBox_common_log->setCurrentText(str);
        break;
    case 3:
        strlist = str.split("/");
        ui->lineEdit_common_saddr->setText(strlist[0]);
        ui->lineEdit_common_smask->setText(strlist[1]);
        break;
    case 4:
        strlist = str.split("/");
        ui->lineEdit_common_daddr->setText(strlist[0]);
        ui->lineEdit_common_dmask->setText(strlist[1]);
        break;
    default:
        break;
    }

}

void CommonRuleDialog::setSourceRule(struct RULE_ST &rule){
    ui->comboBox_common_mtype->setCurrentText(rule2mtype(rule.mtype));
    ui->comboBox_common_action->setCurrentText(rule2action(rule.action));
    ui->comboBox_common_log->setCurrentText(rule2log(rule.log));
    ui->lineEdit_common_saddr->setText(rule2addr(rule.saddr));
    ui->lineEdit_common_smask->setText(rule2mask(rule.smask));
    ui->lineEdit_common_daddr->setText(rule2addr(rule.daddr));
    ui->lineEdit_common_dmask->setText(rule2mask(rule.dmask));

    switch(rule.mtype){
    case CONNECT:
        ui->stackedWidget->setCurrentWidget(ui->page_connect);
        ui->comboBox_connect_unf->setCurrentText(QString::number((rule.deep.connect.flag & 0x80) >> 7));
        ui->comboBox_connect_pf->setCurrentText(QString::number((rule.deep.connect.flag & 0x40) >> 6));
        ui->comboBox_connect_wr->setCurrentText(QString::number((rule.deep.connect.flag & 0x20) >> 5));
        ui->comboBox_connect_wqos->setCurrentText(QString::number((rule.deep.connect.flag & 0x18) >> 3));
        ui->comboBox_connect_wf->setCurrentText(QString::number((rule.deep.connect.flag & 0x04) >> 2));
        ui->comboBox_connect_cs->setCurrentText(QString::number((rule.deep.connect.flag & 0x02) >> 1));
        break;
    case PUBLISH:
        ui->stackedWidget->setCurrentWidget(ui->page_publish);
        ui->comboBox_publish_dup->setCurrentText(QString::number((rule.deep.publish.flag & 0x08) >> 3));
        ui->comboBox_publish_qos->setCurrentText(QString::number((rule.deep.publish.flag & 0x05) >> 1));
        ui->comboBox_publish_retain->setCurrentText(QString::number(rule.deep.publish.flag & 0x01));
        if(rule.deep.publish.topic != NULL)
            ui->lineEdit_publish_topic->setText(*(rule.deep.publish.topic));
        if(rule.deep.publish.keyword != NULL)
            ui->lineEdit_publish_keyword->setText(*(rule.deep.publish.keyword));
        break;
    case SUBSCRIBE:
        ui->stackedWidget->setCurrentWidget(ui->page_subscribe);
        if(rule.deep.subscribe.topic_filter != NULL){
            ui->lineEdit_subscribe_topicfilter->setText(*(rule.deep.subscribe.topic_filter));
            ui->comboBox_subscribe_rqos->setCurrentText(QString::number(rule.deep.subscribe.rqos));
        }
        break;
    case UNSUBSCRIBE:
        ui->stackedWidget->setCurrentWidget(ui->page_unsubscribe);
        if(rule.deep.unsubscribe.topic_filter != NULL)
            ui->lineEdit_unsubscribe_topicfilter->setText(*(rule.deep.unsubscribe.topic_filter));
        break;
    default:
        ui->stackedWidget->setCurrentWidget(ui->page_common);
        break;
    }

}


void CommonRuleDialog::on_buttonBox_accepted()
{
    RULE_ST rule;
    int pos = 0;

    rule.saddr  = addr2rule(ui->lineEdit_common_saddr->text());
    rule.smask  = mask2rule(ui->lineEdit_common_smask->text());
    rule.daddr  = addr2rule(ui->lineEdit_common_daddr->text());
    rule.dmask  = mask2rule(ui->lineEdit_common_dmask->text());
    rule.mtype  = mtype2rule(ui->comboBox_common_mtype->currentText());
    rule.log    = log2rule(ui->comboBox_common_log->currentText());
    rule.action = action2rule(ui->comboBox_common_action->currentText());
    pos = ui->lineEdit_add_pos->text().toInt();

    switch(rule.mtype){
    case CONNECT:
        rule.deep.connect.flag = conflag2rule();
        break;
    case PUBLISH:
        rule.deep.publish.flag = pubflag2rule();
        if(ui->lineEdit_publish_topic->text().isEmpty())
            rule.deep.publish.topic = NULL;
        else
            rule.deep.publish.topic = new QString(ui->lineEdit_publish_topic->text());
        if(ui->lineEdit_publish_keyword->text().isEmpty())
            rule.deep.publish.keyword = NULL;
        else
            rule.deep.publish.keyword = new QString(ui->lineEdit_publish_keyword->text());
        break;
    case SUBSCRIBE:
        if(ui->lineEdit_subscribe_topicfilter->text().isEmpty())
            rule.deep.subscribe.topic_filter = NULL;
        else{
            rule.deep.subscribe.topic_filter = new QString(ui->lineEdit_subscribe_topicfilter->text());
            rule.deep.subscribe.rqos = (u_int8_t)(ui->comboBox_subscribe_rqos->currentText().toUShort());
        }
        break;
    case UNSUBSCRIBE:
        if(ui->lineEdit_unsubscribe_topicfilter->text().isEmpty()){
            rule.deep.unsubscribe.topic_filter = NULL;
        }
        else
            rule.deep.unsubscribe.topic_filter = new QString(ui->lineEdit_unsubscribe_topicfilter->text());
        break;
    default:
        break;
    }

    if(mode == ADD_RULE)
        emit addCommonRuleSignal(rule, pos);
    else if(mode == MOD_RULE)
        emit modCommonRuleSignal(rule, pos);
}

/*
void CommonRuleDialog::on_comboBox_common_mtype_activated(int index)
{
    if(ui->comboBox_common_mtype->itemText(index) == "CONNECT")
        ui->stackedWidget->setCurrentWidget(ui->page_connect);
    else if(ui->comboBox_common_mtype->itemText(index) == "PUBLISH")
        ui->stackedWidget->setCurrentWidget(ui->page_publish);
    else if(ui->comboBox_common_mtype->itemText(index) == "SUBSCRIBE")
        ui->stackedWidget->setCurrentWidget(ui->page_subscribe);
    else if(ui->comboBox_common_mtype->itemText(index) == "UNSUBSCRIBE")
        ui->stackedWidget->setCurrentWidget(ui->page_unsubscribe);
    else
        ui->stackedWidget->setCurrentWidget(ui->page_common);
}
*/

u_int8_t CommonRuleDialog::conflag2rule(){
    unsigned short flag = 0;

    flag = (ui->comboBox_connect_unf->currentText().toUShort() << 7)
         | (ui->comboBox_connect_pf->currentText().toUShort() << 6)
         | (ui->comboBox_connect_wr->currentText().toUShort() << 5)
         | (ui->comboBox_connect_wqos->currentText().toUShort() << 3)
         | (ui->comboBox_connect_wf->currentText().toUShort() << 2)
         | (ui->comboBox_connect_cs->currentText().toUShort() << 1);

    //qDebug() << QString::number(flag, 2);
    return u_int8_t(flag);
}

u_int8_t CommonRuleDialog::pubflag2rule(){
    unsigned short flag = 0;

    flag = (ui->comboBox_publish_dup ->currentText().toUShort() << 3)
         | (ui->comboBox_publish_qos->currentText().toUShort() << 1)
         | (ui->comboBox_publish_retain->currentText().toUShort());

    //qDebug() << QString::number(flag, 2);
    return u_int8_t(flag);
}


void CommonRuleDialog::on_comboBox_common_mtype_activated(const QString &mtype)
{
    if(mtype == "CONNECT")
        ui->stackedWidget->setCurrentWidget(ui->page_connect);
    else if(mtype == "PUBLISH")
        ui->stackedWidget->setCurrentWidget(ui->page_publish);
    else if(mtype == "SUBSCRIBE")
        ui->stackedWidget->setCurrentWidget(ui->page_subscribe);
    else if(mtype == "UNSUBSCRIBE")
        ui->stackedWidget->setCurrentWidget(ui->page_unsubscribe);
    else
        ui->stackedWidget->setCurrentWidget(ui->page_common);
}
