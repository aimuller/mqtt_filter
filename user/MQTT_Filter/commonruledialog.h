#ifndef COMMONRULEDIALOG_H
#define COMMONRULEDIALOG_H

#include <QDialog>
#include "header.h"

namespace Ui {
class CommonRuleDialog;
}

class CommonRuleDialog : public QDialog
{
    Q_OBJECT
    
public:
    void setMode(int arg);
    explicit CommonRuleDialog(QWidget *parent = 0);
    ~CommonRuleDialog();
    
private slots:
    void on_buttonBox_accepted();

private:
    int mode;
    Ui::CommonRuleDialog *ui;

signals:
    void addCommonRuleSignal(struct RULE_ST);
    void modCommonRuleSignal(struct RULE_ST);

};

#endif // COMMONRULEDIALOG_H
