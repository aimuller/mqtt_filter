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
    explicit CommonRuleDialog(QWidget *parent = 0);
    ~CommonRuleDialog();
    
private slots:
    void on_buttonBox_accepted();

private:
    int mode;
    Ui::CommonRuleDialog *ui;

signals:
    void addCommonRuleSignal(struct RULE_ST, unsigned int);
    void modCommonRuleSignal(struct RULE_ST, unsigned int);

};

#endif // COMMONRULEDIALOG_H
