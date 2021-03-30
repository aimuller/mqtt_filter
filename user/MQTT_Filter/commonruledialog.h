#ifndef COMMONRULEDIALOG_H
#define COMMONRULEDIALOG_H

#include <QDialog>
#include "header.h"
#include "helper.h"

namespace Ui {
class CommonRuleDialog;
}

class CommonRuleDialog : public QDialog
{
    Q_OBJECT
    
public:
    void setMode(int arg);
    void setSourceRule(QString, int);
    void setSourceRule(struct RULE_ST &, int pos);
    explicit CommonRuleDialog(QWidget *parent = 0);
    ~CommonRuleDialog();
    
private slots:
    void on_buttonBox_accepted();

    //void on_comboBox_common_mtype_activated(int index);

    void on_comboBox_common_mtype_activated(const QString &mtype);

    void on_checkBox_clicked(bool checked);

private:
    int mode;
    u_int8_t conflag2rule();
    u_int8_t pubflag2rule();
    //u_int8_t subrqos2rule();
    Ui::CommonRuleDialog *ui;

signals:
    void addCommonRuleSignal(struct RULE_ST, int);
    void modCommonRuleSignal(struct RULE_ST, int);

};

#endif // COMMONRULEDIALOG_H
