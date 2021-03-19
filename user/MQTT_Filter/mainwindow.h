#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "commonruledialog.h"
#include "header.h"



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
    void addCommonRule(struct RULE_ST);
    void modCommonRule(struct RULE_ST);

    void on_pushButton_mod_rule_clicked();

    void on_pushButton_mf_open_clicked();

    void on_pushButton_mf_close_clicked();

private:
    Ui::MainWindow *ui;
    CommonRuleDialog *addCommonRuleDialog;
    CommonRuleDialog *modCommonRuleDialog;
};

#endif // MAINWINDOW_H
