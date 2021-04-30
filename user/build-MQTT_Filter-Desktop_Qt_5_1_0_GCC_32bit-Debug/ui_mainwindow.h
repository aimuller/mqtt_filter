/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.1.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Import_rule_file;
    QAction *action_export_rule_file;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_mf_open;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_mf_close;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_mf_state;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *tab_rule;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_add_rule;
    QPushButton *pushButton_mod_rule;
    QPushButton *pushButton_del_rule;
    QPushButton *pushButton_clear_rule;
    QGridLayout *gridLayout_3;
    QTableWidget *tableWidget_rule;
    QWidget *tab_connect;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_update_connect;
    QGridLayout *gridLayout_7;
    QTableWidget *tableWidget_connect;
    QWidget *tab_log;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *pushButton;
    QTextBrowser *textBrowser;
    QGridLayout *gridLayout;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(791, 565);
        action_Import_rule_file = new QAction(MainWindow);
        action_Import_rule_file->setObjectName(QStringLiteral("action_Import_rule_file"));
        action_export_rule_file = new QAction(MainWindow);
        action_export_rule_file->setObjectName(QStringLiteral("action_export_rule_file"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_3 = new QVBoxLayout(centralWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_mf_open = new QPushButton(centralWidget);
        pushButton_mf_open->setObjectName(QStringLiteral("pushButton_mf_open"));

        horizontalLayout->addWidget(pushButton_mf_open);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton_mf_close = new QPushButton(centralWidget);
        pushButton_mf_close->setObjectName(QStringLiteral("pushButton_mf_close"));

        horizontalLayout->addWidget(pushButton_mf_close);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        label_mf_state = new QLabel(centralWidget);
        label_mf_state->setObjectName(QStringLiteral("label_mf_state"));

        horizontalLayout->addWidget(label_mf_state);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout_3->addLayout(horizontalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_rule = new QWidget();
        tab_rule->setObjectName(QStringLiteral("tab_rule"));
        gridLayout_2 = new QGridLayout(tab_rule);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        pushButton_add_rule = new QPushButton(tab_rule);
        pushButton_add_rule->setObjectName(QStringLiteral("pushButton_add_rule"));

        horizontalLayout_2->addWidget(pushButton_add_rule);

        pushButton_mod_rule = new QPushButton(tab_rule);
        pushButton_mod_rule->setObjectName(QStringLiteral("pushButton_mod_rule"));

        horizontalLayout_2->addWidget(pushButton_mod_rule);

        pushButton_del_rule = new QPushButton(tab_rule);
        pushButton_del_rule->setObjectName(QStringLiteral("pushButton_del_rule"));

        horizontalLayout_2->addWidget(pushButton_del_rule);

        pushButton_clear_rule = new QPushButton(tab_rule);
        pushButton_clear_rule->setObjectName(QStringLiteral("pushButton_clear_rule"));

        horizontalLayout_2->addWidget(pushButton_clear_rule);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        tableWidget_rule = new QTableWidget(tab_rule);
        if (tableWidget_rule->columnCount() < 6)
            tableWidget_rule->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_rule->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_rule->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget_rule->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget_rule->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget_rule->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget_rule->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        tableWidget_rule->setObjectName(QStringLiteral("tableWidget_rule"));

        gridLayout_3->addWidget(tableWidget_rule, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout_3, 1, 0, 1, 1);

        tabWidget->addTab(tab_rule, QString());
        tab_connect = new QWidget();
        tab_connect->setObjectName(QStringLiteral("tab_connect"));
        verticalLayout = new QVBoxLayout(tab_connect);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        pushButton_update_connect = new QPushButton(tab_connect);
        pushButton_update_connect->setObjectName(QStringLiteral("pushButton_update_connect"));

        horizontalLayout_7->addWidget(pushButton_update_connect);


        verticalLayout->addLayout(horizontalLayout_7);

        gridLayout_7 = new QGridLayout();
        gridLayout_7->setSpacing(6);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        tableWidget_connect = new QTableWidget(tab_connect);
        if (tableWidget_connect->columnCount() < 8)
            tableWidget_connect->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(1, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(2, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(3, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(4, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(5, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(6, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidget_connect->setHorizontalHeaderItem(7, __qtablewidgetitem13);
        tableWidget_connect->setObjectName(QStringLiteral("tableWidget_connect"));

        gridLayout_7->addWidget(tableWidget_connect, 0, 0, 1, 1);


        verticalLayout->addLayout(gridLayout_7);

        tabWidget->addTab(tab_connect, QString());
        tab_log = new QWidget();
        tab_log->setObjectName(QStringLiteral("tab_log"));
        gridLayout_6 = new QGridLayout(tab_log);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        pushButton = new QPushButton(tab_log);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_6->addWidget(pushButton);


        gridLayout_6->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        textBrowser = new QTextBrowser(tab_log);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        gridLayout_6->addWidget(textBrowser, 1, 0, 1, 1);

        tabWidget->addTab(tab_log, QString());

        verticalLayout_2->addWidget(tabWidget);


        verticalLayout_3->addLayout(verticalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));

        verticalLayout_3->addLayout(gridLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 791, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menu->addSeparator();
        menu->addSeparator();
        menu->addSeparator();
        menu->addAction(action_Import_rule_file);
        menu->addAction(action_export_rule_file);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        action_Import_rule_file->setText(QApplication::translate("MainWindow", "\345\257\274\345\205\245\350\247\204\345\210\231", 0));
        action_export_rule_file->setText(QApplication::translate("MainWindow", "\345\257\274\345\207\272\350\247\204\345\210\231", 0));
        pushButton_mf_open->setText(QApplication::translate("MainWindow", "\345\274\200\345\220\257\350\277\207\346\273\244", 0));
        pushButton_mf_close->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255\350\277\207\346\273\244", 0));
        label_mf_state->setText(QApplication::translate("MainWindow", " TextLabel", 0));
        pushButton_add_rule->setText(QApplication::translate("MainWindow", "\346\217\222\345\205\245\350\247\204\345\210\231", 0));
        pushButton_mod_rule->setText(QApplication::translate("MainWindow", "\344\277\256\346\224\271\350\247\204\345\210\231", 0));
        pushButton_del_rule->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\350\247\204\345\210\231", 0));
        pushButton_clear_rule->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\350\247\204\345\210\231", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_rule->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "MQTT\347\261\273\345\236\213", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_rule->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "\345\212\250\344\275\234", 0));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget_rule->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "\346\227\245\345\277\227", 0));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget_rule->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "\346\272\220IP", 0));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget_rule->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "\347\233\256\347\232\204IP", 0));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget_rule->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "\345\205\266\344\273\226", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_rule), QApplication::translate("MainWindow", "\350\247\204\345\210\231\347\256\241\347\220\206", 0));
        pushButton_update_connect->setText(QApplication::translate("MainWindow", "\345\210\267\346\226\260", 0));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget_connect->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "\346\272\220\345\234\260\345\235\200", 0));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidget_connect->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "\347\233\256\347\232\204\345\234\260\345\235\200", 0));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidget_connect->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QApplication::translate("MainWindow", "\350\277\236\346\216\245\346\240\207\345\277\227", 0));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidget_connect->horizontalHeaderItem(3);
        ___qtablewidgetitem9->setText(QApplication::translate("MainWindow", "Keep Alive", 0));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidget_connect->horizontalHeaderItem(4);
        ___qtablewidgetitem10->setText(QApplication::translate("MainWindow", "\345\256\242\346\210\267\347\253\257\346\240\207\350\257\206\347\254\246", 0));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidget_connect->horizontalHeaderItem(5);
        ___qtablewidgetitem11->setText(QApplication::translate("MainWindow", "\347\224\250\346\210\267\345\220\215", 0));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidget_connect->horizontalHeaderItem(6);
        ___qtablewidgetitem12->setText(QApplication::translate("MainWindow", "\351\201\227\345\230\261\344\270\273\351\242\230", 0));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidget_connect->horizontalHeaderItem(7);
        ___qtablewidgetitem13->setText(QApplication::translate("MainWindow", "\351\201\227\345\230\261\346\266\210\346\201\257", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_connect), QApplication::translate("MainWindow", "\350\277\236\346\216\245\346\237\245\347\234\213", 0));
        pushButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\227\245\345\277\227", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_log), QApplication::translate("MainWindow", "\346\227\245\345\277\227\345\256\241\346\237\245", 0));
        menu->setTitle(QApplication::translate("MainWindow", "\346\226\207\344\273\266", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
