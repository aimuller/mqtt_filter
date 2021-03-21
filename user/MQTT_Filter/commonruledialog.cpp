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
}
void CommonRuleDialog::setOriginRule(RULE_ST rule){

}

void CommonRuleDialog::on_buttonBox_accepted()
{
    RULE_ST rule;
    unsigned int pos = 0;

    rule.saddr  = addr2rule(ui->lineEdit_common_saddr->text());
    rule.smask  = mask2rule(ui->lineEdit_common_saddr->text());
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
