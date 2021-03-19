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

void CommonRuleDialog::on_buttonBox_accepted()
{
    RULE_ST rule;
    rule.action = 1;
    rule.daddr = 2;
    rule.dmask = 3;
    if(mode == ADD_RULE)
        emit addCommonRuleSignal(rule);
    else if(mode == MOD_RULE)
        emit modCommonRuleSignal(rule);
}
