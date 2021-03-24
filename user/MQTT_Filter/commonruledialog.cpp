#include "commonruledialog.h"
#include "ui_commonruledialog.h"


CommonRuleDialog::CommonRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommonRuleDialog)
{
    ui->setupUi(this);
}

CommonRuleDialog::~CommonRuleDialog()
{
    delete ui;
}

void CommonRuleDialog::setMode(int arg){
    mode = arg;
    if(mode == ADD_RULE)
        ui->label_8->setText("位置(默认最后的位置)");
    else if(mode == MOD_RULE){
        ui->label_8->setText("位置(默认原来的位置)");
    }
}
void CommonRuleDialog::setSourceRule(QString str, int col){
    QStringList strlist;
    switch(col){
    case 0:
        strlist = str.split("/");
        ui->lineEdit_common_saddr->setText(strlist[0]);
        ui->lineEdit_common_smask->setText(strlist[1]);
        break;
    case 1:
        strlist = str.split("/");
        ui->lineEdit_common_daddr->setText(strlist[0]);
        ui->lineEdit_common_dmask->setText(strlist[1]);
        break;
    case 2:
        ui->comboBox_common_mtype->setCurrentText(str);
        break;
    case 3:
        ui->comboBox_common_log->setCurrentText(str);
        break;
    case 4:
        ui->comboBox_common_action->setCurrentText(str);
        break;
    default:
        break;
    }
}

void CommonRuleDialog::on_buttonBox_accepted()
{
    RULE_ST rule;
    unsigned int pos = 0;

    rule.saddr  = addr2rule(ui->lineEdit_common_saddr->text());
    rule.smask  = mask2rule(ui->lineEdit_common_smask->text());
    rule.daddr  = addr2rule(ui->lineEdit_common_daddr->text());
    rule.dmask  = mask2rule(ui->lineEdit_common_dmask->text());
    rule.mtype  = mtype2rule(ui->comboBox_common_mtype->currentText());
    rule.log    = log2rule(ui->comboBox_common_log->currentText());
    rule.action = action2rule(ui->comboBox_common_action->currentText());
    pos = ui->lineEdit_add_pos->text().toUInt();

    if(mode == ADD_RULE)
        emit addCommonRuleSignal(rule, pos);
    else if(mode == MOD_RULE)
        emit modCommonRuleSignal(rule, pos);
}
