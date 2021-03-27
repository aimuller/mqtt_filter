#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QList>

#include "commonruledialog.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_add_rule_clicked();
    void addCommonRule(struct RULE_ST, unsigned int);
    void modCommonRule(struct RULE_ST, unsigned int);

    void on_pushButton_mod_rule_clicked();

    void on_pushButton_mf_open_clicked();

    void on_pushButton_mf_close_clicked();

    void on_pushButton_del_rule_clicked();

    void on_pushButton_clear_rule_clicked();

    void on_action_export_rule_file_triggered();

private:
    int fd;
    int active;
    QList<struct RULE_ST> rule_list;
    Ui::MainWindow *ui;
    CommonRuleDialog *addCommonRuleDialog;
    CommonRuleDialog *modCommonRuleDialog;

    void updateCommonRule();
    void getRuleFromKernel();
    void setRuleItem(struct RULE_ST *, int);

};

#endif // MAINWINDOW_H
