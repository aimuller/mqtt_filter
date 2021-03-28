/********************************************************************************
** Form generated from reading UI file 'commonruledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.1.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMONRULEDIALOG_H
#define UI_COMMONRULEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommonRuleDialog
{
public:
    QGridLayout *gridLayout_8;
    QDialogButtonBox *buttonBox;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit_common_dmask;
    QLabel *label_3;
    QComboBox *comboBox_common_mtype;
    QLabel *label_8;
    QLabel *label_7;
    QLabel *label_5;
    QComboBox *comboBox_common_action;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *lineEdit_common_saddr;
    QLabel *label_4;
    QLineEdit *lineEdit_common_smask;
    QComboBox *comboBox_common_log;
    QLabel *label_6;
    QLineEdit *lineEdit_common_daddr;
    QLineEdit *lineEdit_add_pos;
    QStackedWidget *stackedWidget;
    QWidget *page_common;
    QWidget *page_connect;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_2;
    QComboBox *comboBox_connect_pf;
    QLabel *label_10;
    QLabel *label_14;
    QLabel *label_13;
    QComboBox *comboBox_connect_wqos;
    QLabel *label_16;
    QSpacerItem *horizontalSpacer_3;
    QComboBox *comboBox_connect_unf;
    QComboBox *comboBox_connect_wr;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_17;
    QComboBox *comboBox_connect_wf;
    QComboBox *comboBox_connect_cs;
    QLabel *label_18;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_9;
    QWidget *page_publish;
    QGridLayout *gridLayout_9;
    QSpacerItem *verticalSpacer_4;
    QGridLayout *gridLayout_4;
    QLabel *label_20;
    QLineEdit *lineEdit_publish_keyword;
    QLineEdit *lineEdit_publish_topic;
    QLabel *label_21;
    QGridLayout *gridLayout_3;
    QLabel *label_9;
    QComboBox *comboBox_publish_qos;
    QLabel *label_15;
    QLabel *label_19;
    QComboBox *comboBox_publish_retain;
    QComboBox *comboBox_publish_dup;
    QSpacerItem *verticalSpacer;
    QWidget *page_subscribe;
    QGridLayout *gridLayout_10;
    QLabel *label_11;
    QGridLayout *gridLayout_5;
    QLabel *label_22;
    QLineEdit *lineEdit_subscribe_topicfilter;
    QLabel *label_23;
    QComboBox *comboBox_subscribe_rqos;
    QSpacerItem *verticalSpacer_6;
    QSpacerItem *verticalSpacer_10;
    QWidget *page_unsubscribe;
    QGridLayout *gridLayout_11;
    QHBoxLayout *horizontalLayout;
    QLabel *label_25;
    QLineEdit *lineEdit_unsubscribe_topicfilter;
    QSpacerItem *verticalSpacer_7;
    QSpacerItem *verticalSpacer_8;
    QSpacerItem *verticalSpacer_5;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *CommonRuleDialog)
    {
        if (CommonRuleDialog->objectName().isEmpty())
            CommonRuleDialog->setObjectName(QStringLiteral("CommonRuleDialog"));
        CommonRuleDialog->resize(584, 329);
        gridLayout_8 = new QGridLayout(CommonRuleDialog);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        buttonBox = new QDialogButtonBox(CommonRuleDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_8->addWidget(buttonBox, 5, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        lineEdit_common_dmask = new QLineEdit(CommonRuleDialog);
        lineEdit_common_dmask->setObjectName(QStringLiteral("lineEdit_common_dmask"));

        gridLayout->addWidget(lineEdit_common_dmask, 3, 3, 1, 1);

        label_3 = new QLabel(CommonRuleDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 1, 1, 1);

        comboBox_common_mtype = new QComboBox(CommonRuleDialog);
        comboBox_common_mtype->setObjectName(QStringLiteral("comboBox_common_mtype"));

        gridLayout->addWidget(comboBox_common_mtype, 1, 0, 1, 1);

        label_8 = new QLabel(CommonRuleDialog);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 0, 3, 1, 1);

        label_7 = new QLabel(CommonRuleDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 0, 1, 1, 1);

        label_5 = new QLabel(CommonRuleDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 2, 3, 1, 1);

        comboBox_common_action = new QComboBox(CommonRuleDialog);
        comboBox_common_action->setObjectName(QStringLiteral("comboBox_common_action"));

        gridLayout->addWidget(comboBox_common_action, 1, 1, 1, 1);

        label = new QLabel(CommonRuleDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        label_2 = new QLabel(CommonRuleDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        lineEdit_common_saddr = new QLineEdit(CommonRuleDialog);
        lineEdit_common_saddr->setObjectName(QStringLiteral("lineEdit_common_saddr"));

        gridLayout->addWidget(lineEdit_common_saddr, 3, 0, 1, 1);

        label_4 = new QLabel(CommonRuleDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 2, 2, 1, 1);

        lineEdit_common_smask = new QLineEdit(CommonRuleDialog);
        lineEdit_common_smask->setObjectName(QStringLiteral("lineEdit_common_smask"));

        gridLayout->addWidget(lineEdit_common_smask, 3, 1, 1, 1);

        comboBox_common_log = new QComboBox(CommonRuleDialog);
        comboBox_common_log->setObjectName(QStringLiteral("comboBox_common_log"));

        gridLayout->addWidget(comboBox_common_log, 1, 2, 1, 1);

        label_6 = new QLabel(CommonRuleDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 0, 2, 1, 1);

        lineEdit_common_daddr = new QLineEdit(CommonRuleDialog);
        lineEdit_common_daddr->setObjectName(QStringLiteral("lineEdit_common_daddr"));

        gridLayout->addWidget(lineEdit_common_daddr, 3, 2, 1, 1);

        lineEdit_add_pos = new QLineEdit(CommonRuleDialog);
        lineEdit_add_pos->setObjectName(QStringLiteral("lineEdit_add_pos"));

        gridLayout->addWidget(lineEdit_add_pos, 1, 3, 1, 1);


        gridLayout_8->addLayout(gridLayout, 1, 0, 1, 1);

        stackedWidget = new QStackedWidget(CommonRuleDialog);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        page_common = new QWidget();
        page_common->setObjectName(QStringLiteral("page_common"));
        stackedWidget->addWidget(page_common);
        buttonBox->raise();
        page_connect = new QWidget();
        page_connect->setObjectName(QStringLiteral("page_connect"));
        gridLayout_7 = new QGridLayout(page_connect);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        comboBox_connect_pf = new QComboBox(page_connect);
        comboBox_connect_pf->setObjectName(QStringLiteral("comboBox_connect_pf"));

        gridLayout_2->addWidget(comboBox_connect_pf, 3, 2, 1, 1);

        label_10 = new QLabel(page_connect);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_2->addWidget(label_10, 0, 0, 1, 1);

        label_14 = new QLabel(page_connect);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_2->addWidget(label_14, 0, 4, 1, 1);

        label_13 = new QLabel(page_connect);
        label_13->setObjectName(QStringLiteral("label_13"));

        gridLayout_2->addWidget(label_13, 0, 2, 1, 1);

        comboBox_connect_wqos = new QComboBox(page_connect);
        comboBox_connect_wqos->setObjectName(QStringLiteral("comboBox_connect_wqos"));

        gridLayout_2->addWidget(comboBox_connect_wqos, 5, 0, 1, 1);

        label_16 = new QLabel(page_connect);
        label_16->setObjectName(QStringLiteral("label_16"));

        gridLayout_2->addWidget(label_16, 4, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 1, 1, 1);

        comboBox_connect_unf = new QComboBox(page_connect);
        comboBox_connect_unf->setObjectName(QStringLiteral("comboBox_connect_unf"));

        gridLayout_2->addWidget(comboBox_connect_unf, 3, 0, 1, 1);

        comboBox_connect_wr = new QComboBox(page_connect);
        comboBox_connect_wr->setObjectName(QStringLiteral("comboBox_connect_wr"));

        gridLayout_2->addWidget(comboBox_connect_wr, 3, 4, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 0, 3, 1, 1);

        label_17 = new QLabel(page_connect);
        label_17->setObjectName(QStringLiteral("label_17"));

        gridLayout_2->addWidget(label_17, 4, 4, 1, 1);

        comboBox_connect_wf = new QComboBox(page_connect);
        comboBox_connect_wf->setObjectName(QStringLiteral("comboBox_connect_wf"));

        gridLayout_2->addWidget(comboBox_connect_wf, 5, 2, 1, 1);

        comboBox_connect_cs = new QComboBox(page_connect);
        comboBox_connect_cs->setObjectName(QStringLiteral("comboBox_connect_cs"));

        gridLayout_2->addWidget(comboBox_connect_cs, 5, 4, 1, 1);

        label_18 = new QLabel(page_connect);
        label_18->setObjectName(QStringLiteral("label_18"));

        gridLayout_2->addWidget(label_18, 4, 2, 1, 1);


        gridLayout_7->addLayout(gridLayout_2, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer_2, 2, 0, 1, 1);

        verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer_9, 0, 0, 1, 1);

        stackedWidget->addWidget(page_connect);
        page_publish = new QWidget();
        page_publish->setObjectName(QStringLiteral("page_publish"));
        gridLayout_9 = new QGridLayout(page_publish);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer_4, 3, 0, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_20 = new QLabel(page_publish);
        label_20->setObjectName(QStringLiteral("label_20"));

        gridLayout_4->addWidget(label_20, 0, 0, 1, 1);

        lineEdit_publish_keyword = new QLineEdit(page_publish);
        lineEdit_publish_keyword->setObjectName(QStringLiteral("lineEdit_publish_keyword"));

        gridLayout_4->addWidget(lineEdit_publish_keyword, 1, 1, 1, 1);

        lineEdit_publish_topic = new QLineEdit(page_publish);
        lineEdit_publish_topic->setObjectName(QStringLiteral("lineEdit_publish_topic"));

        gridLayout_4->addWidget(lineEdit_publish_topic, 0, 1, 1, 1);

        label_21 = new QLabel(page_publish);
        label_21->setObjectName(QStringLiteral("label_21"));

        gridLayout_4->addWidget(label_21, 1, 0, 1, 1);


        gridLayout_9->addLayout(gridLayout_4, 2, 0, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_9 = new QLabel(page_publish);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_3->addWidget(label_9, 0, 1, 1, 1);

        comboBox_publish_qos = new QComboBox(page_publish);
        comboBox_publish_qos->setObjectName(QStringLiteral("comboBox_publish_qos"));

        gridLayout_3->addWidget(comboBox_publish_qos, 1, 2, 1, 1);

        label_15 = new QLabel(page_publish);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout_3->addWidget(label_15, 0, 3, 1, 1);

        label_19 = new QLabel(page_publish);
        label_19->setObjectName(QStringLiteral("label_19"));

        gridLayout_3->addWidget(label_19, 0, 2, 1, 1);

        comboBox_publish_retain = new QComboBox(page_publish);
        comboBox_publish_retain->setObjectName(QStringLiteral("comboBox_publish_retain"));

        gridLayout_3->addWidget(comboBox_publish_retain, 1, 3, 1, 1);

        comboBox_publish_dup = new QComboBox(page_publish);
        comboBox_publish_dup->setObjectName(QStringLiteral("comboBox_publish_dup"));

        gridLayout_3->addWidget(comboBox_publish_dup, 1, 1, 1, 1);


        gridLayout_9->addLayout(gridLayout_3, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer, 0, 0, 1, 1);

        stackedWidget->addWidget(page_publish);
        page_subscribe = new QWidget();
        page_subscribe->setObjectName(QStringLiteral("page_subscribe"));
        gridLayout_10 = new QGridLayout(page_subscribe);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        label_11 = new QLabel(page_subscribe);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_10->addWidget(label_11, 1, 1, 1, 1);

        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        label_22 = new QLabel(page_subscribe);
        label_22->setObjectName(QStringLiteral("label_22"));

        gridLayout_5->addWidget(label_22, 1, 0, 1, 1);

        lineEdit_subscribe_topicfilter = new QLineEdit(page_subscribe);
        lineEdit_subscribe_topicfilter->setObjectName(QStringLiteral("lineEdit_subscribe_topicfilter"));

        gridLayout_5->addWidget(lineEdit_subscribe_topicfilter, 0, 1, 1, 1);

        label_23 = new QLabel(page_subscribe);
        label_23->setObjectName(QStringLiteral("label_23"));

        gridLayout_5->addWidget(label_23, 0, 0, 1, 1);

        comboBox_subscribe_rqos = new QComboBox(page_subscribe);
        comboBox_subscribe_rqos->setObjectName(QStringLiteral("comboBox_subscribe_rqos"));

        gridLayout_5->addWidget(comboBox_subscribe_rqos, 1, 1, 1, 1, Qt::AlignLeft);


        gridLayout_10->addLayout(gridLayout_5, 1, 0, 1, 1);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_10->addItem(verticalSpacer_6, 2, 0, 1, 1);

        verticalSpacer_10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_10->addItem(verticalSpacer_10, 0, 0, 1, 1);

        stackedWidget->addWidget(page_subscribe);
        page_unsubscribe = new QWidget();
        page_unsubscribe->setObjectName(QStringLiteral("page_unsubscribe"));
        gridLayout_11 = new QGridLayout(page_unsubscribe);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_25 = new QLabel(page_unsubscribe);
        label_25->setObjectName(QStringLiteral("label_25"));

        horizontalLayout->addWidget(label_25);

        lineEdit_unsubscribe_topicfilter = new QLineEdit(page_unsubscribe);
        lineEdit_unsubscribe_topicfilter->setObjectName(QStringLiteral("lineEdit_unsubscribe_topicfilter"));

        horizontalLayout->addWidget(lineEdit_unsubscribe_topicfilter);


        gridLayout_11->addLayout(horizontalLayout, 1, 0, 1, 1);

        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_11->addItem(verticalSpacer_7, 2, 0, 1, 1);

        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_11->addItem(verticalSpacer_8, 0, 0, 1, 1);

        stackedWidget->addWidget(page_unsubscribe);

        gridLayout_8->addWidget(stackedWidget, 3, 0, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_5, 2, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_3, 0, 0, 1, 1);

        stackedWidget->raise();
        label_2->raise();
        buttonBox->raise();

        retranslateUi(CommonRuleDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CommonRuleDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CommonRuleDialog, SLOT(reject()));

        stackedWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(CommonRuleDialog);
    } // setupUi

    void retranslateUi(QDialog *CommonRuleDialog)
    {
        CommonRuleDialog->setWindowTitle(QApplication::translate("CommonRuleDialog", "Dialog", 0));
        label_3->setText(QApplication::translate("CommonRuleDialog", "\346\272\220\345\234\260\345\235\200\346\216\251\347\240\201", 0));
        comboBox_common_mtype->clear();
        comboBox_common_mtype->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "ANY", 0)
         << QApplication::translate("CommonRuleDialog", "CONNECT", 0)
         << QApplication::translate("CommonRuleDialog", "CONNACK", 0)
         << QApplication::translate("CommonRuleDialog", "PUBLISH", 0)
         << QApplication::translate("CommonRuleDialog", "PUBACK", 0)
         << QApplication::translate("CommonRuleDialog", "PUBREC", 0)
         << QApplication::translate("CommonRuleDialog", "PUBREL", 0)
         << QApplication::translate("CommonRuleDialog", "PUBCOMP", 0)
         << QApplication::translate("CommonRuleDialog", "SUBSCRIBE", 0)
         << QApplication::translate("CommonRuleDialog", "SUBACK", 0)
         << QApplication::translate("CommonRuleDialog", "UNSUBSCRIBE", 0)
         << QApplication::translate("CommonRuleDialog", "UNSUBACK", 0)
         << QApplication::translate("CommonRuleDialog", "PINGREQ", 0)
         << QApplication::translate("CommonRuleDialog", "PINGRESP", 0)
         << QApplication::translate("CommonRuleDialog", "DISCONNECT", 0)
        );
        label_8->setText(QApplication::translate("CommonRuleDialog", "\344\275\215\347\275\256 (\351\273\230\350\256\244\346\234\200\345\220\216)", 0));
        label_7->setText(QApplication::translate("CommonRuleDialog", "\345\212\250\344\275\234", 0));
        label_5->setText(QApplication::translate("CommonRuleDialog", "\347\233\256\347\232\204\345\234\260\345\235\200\346\216\251\347\240\201", 0));
        comboBox_common_action->clear();
        comboBox_common_action->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "DENY", 0)
         << QApplication::translate("CommonRuleDialog", "PERMIT", 0)
        );
        label->setText(QApplication::translate("CommonRuleDialog", "\346\272\220\345\234\260\345\235\200", 0));
        label_2->setText(QApplication::translate("CommonRuleDialog", "MQTT\347\261\273\345\236\213", 0));
        lineEdit_common_saddr->setText(QString());
        label_4->setText(QApplication::translate("CommonRuleDialog", "\347\233\256\347\232\204\345\234\260\345\235\200", 0));
        comboBox_common_log->clear();
        comboBox_common_log->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "YES", 0)
         << QApplication::translate("CommonRuleDialog", "NO", 0)
        );
        label_6->setText(QApplication::translate("CommonRuleDialog", "\346\227\245\345\277\227", 0));
        comboBox_connect_pf->clear();
        comboBox_connect_pf->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        label_10->setText(QApplication::translate("CommonRuleDialog", "UNF (User Name Flag )", 0));
        label_14->setText(QApplication::translate("CommonRuleDialog", "WR (Will Retain)      ", 0));
        label_13->setText(QApplication::translate("CommonRuleDialog", "PF (Password Flag ) ", 0));
        comboBox_connect_wqos->clear();
        comboBox_connect_wqos->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
         << QApplication::translate("CommonRuleDialog", "2", 0)
        );
        label_16->setText(QApplication::translate("CommonRuleDialog", "WQoS (Will QoS)", 0));
        comboBox_connect_unf->clear();
        comboBox_connect_unf->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        comboBox_connect_wr->clear();
        comboBox_connect_wr->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        label_17->setText(QApplication::translate("CommonRuleDialog", "CS (Clean Session)", 0));
        comboBox_connect_wf->clear();
        comboBox_connect_wf->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        comboBox_connect_cs->clear();
        comboBox_connect_cs->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        label_18->setText(QApplication::translate("CommonRuleDialog", "WF (Will Flag)", 0));
        label_20->setText(QApplication::translate("CommonRuleDialog", "\350\277\207\346\273\244\344\270\273\351\242\230", 0));
        label_21->setText(QApplication::translate("CommonRuleDialog", "\350\277\207\346\273\244\345\205\263\351\224\256\350\257\215", 0));
        label_9->setText(QApplication::translate("CommonRuleDialog", "DUP (\351\207\215\345\217\221\346\240\207\345\277\227)", 0));
        comboBox_publish_qos->clear();
        comboBox_publish_qos->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
         << QApplication::translate("CommonRuleDialog", "2", 0)
        );
        label_15->setText(QApplication::translate("CommonRuleDialog", "RETAIN (\344\277\235\347\225\231\346\240\207\345\277\227)", 0));
        label_19->setText(QApplication::translate("CommonRuleDialog", "QoS (\346\234\215\345\212\241\350\264\250\351\207\217\347\255\211\347\272\247)", 0));
        comboBox_publish_retain->clear();
        comboBox_publish_retain->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        comboBox_publish_dup->clear();
        comboBox_publish_dup->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
        );
        label_11->setText(QString());
        label_22->setText(QApplication::translate("CommonRuleDialog", "\346\234\215\345\212\241\350\264\250\351\207\217\350\246\201\346\261\202", 0));
        label_23->setText(QApplication::translate("CommonRuleDialog", "\344\270\273\351\242\230\350\277\207\346\273\244\345\231\250", 0));
        comboBox_subscribe_rqos->clear();
        comboBox_subscribe_rqos->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "0", 0)
         << QApplication::translate("CommonRuleDialog", "1", 0)
         << QApplication::translate("CommonRuleDialog", "2", 0)
        );
        label_25->setText(QApplication::translate("CommonRuleDialog", "\344\270\273\351\242\230\350\277\207\346\273\244\345\231\250", 0));
    } // retranslateUi

};

namespace Ui {
    class CommonRuleDialog: public Ui_CommonRuleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMONRULEDIALOG_H
