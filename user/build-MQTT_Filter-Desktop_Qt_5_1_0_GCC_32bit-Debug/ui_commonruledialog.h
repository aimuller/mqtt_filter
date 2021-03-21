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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommonRuleDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QComboBox *comboBox_common_action;
    QComboBox *comboBox_common_log;
    QLineEdit *lineEdit_common_smask;
    QLineEdit *lineEdit_common_daddr;
    QComboBox *comboBox_common_mtype;
    QLabel *label;
    QLineEdit *lineEdit_common_dmask;
    QLineEdit *lineEdit_common_saddr;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *lineEdit_add_pos;

    void setupUi(QDialog *CommonRuleDialog)
    {
        if (CommonRuleDialog->objectName().isEmpty())
            CommonRuleDialog->setObjectName(QStringLiteral("CommonRuleDialog"));
        CommonRuleDialog->resize(590, 357);
        buttonBox = new QDialogButtonBox(CommonRuleDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(170, 270, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        gridLayoutWidget = new QWidget(CommonRuleDialog);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(20, 60, 551, 131));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        comboBox_common_action = new QComboBox(gridLayoutWidget);
        comboBox_common_action->setObjectName(QStringLiteral("comboBox_common_action"));

        gridLayout->addWidget(comboBox_common_action, 3, 2, 1, 1);

        comboBox_common_log = new QComboBox(gridLayoutWidget);
        comboBox_common_log->setObjectName(QStringLiteral("comboBox_common_log"));

        gridLayout->addWidget(comboBox_common_log, 3, 1, 1, 1);

        lineEdit_common_smask = new QLineEdit(gridLayoutWidget);
        lineEdit_common_smask->setObjectName(QStringLiteral("lineEdit_common_smask"));

        gridLayout->addWidget(lineEdit_common_smask, 1, 1, 1, 1);

        lineEdit_common_daddr = new QLineEdit(gridLayoutWidget);
        lineEdit_common_daddr->setObjectName(QStringLiteral("lineEdit_common_daddr"));

        gridLayout->addWidget(lineEdit_common_daddr, 1, 2, 1, 1);

        comboBox_common_mtype = new QComboBox(gridLayoutWidget);
        comboBox_common_mtype->setObjectName(QStringLiteral("comboBox_common_mtype"));

        gridLayout->addWidget(comboBox_common_mtype, 3, 0, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_common_dmask = new QLineEdit(gridLayoutWidget);
        lineEdit_common_dmask->setObjectName(QStringLiteral("lineEdit_common_dmask"));

        gridLayout->addWidget(lineEdit_common_dmask, 1, 3, 1, 1);

        lineEdit_common_saddr = new QLineEdit(gridLayoutWidget);
        lineEdit_common_saddr->setObjectName(QStringLiteral("lineEdit_common_saddr"));

        gridLayout->addWidget(lineEdit_common_saddr, 1, 0, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 0, 1, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 0, 2, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 0, 3, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 2, 1, 1, 1);

        label_7 = new QLabel(gridLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 2, 2, 1, 1);

        label_8 = new QLabel(gridLayoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 2, 3, 1, 1);

        lineEdit_add_pos = new QLineEdit(gridLayoutWidget);
        lineEdit_add_pos->setObjectName(QStringLiteral("lineEdit_add_pos"));

        gridLayout->addWidget(lineEdit_add_pos, 3, 3, 1, 1);


        retranslateUi(CommonRuleDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CommonRuleDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CommonRuleDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CommonRuleDialog);
    } // setupUi

    void retranslateUi(QDialog *CommonRuleDialog)
    {
        CommonRuleDialog->setWindowTitle(QApplication::translate("CommonRuleDialog", "Dialog", 0));
        comboBox_common_action->clear();
        comboBox_common_action->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "\346\213\222\347\273\235", 0)
         << QApplication::translate("CommonRuleDialog", "\345\205\201\350\256\270", 0)
        );
        comboBox_common_log->clear();
        comboBox_common_log->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "\346\230\257", 0)
         << QApplication::translate("CommonRuleDialog", "\345\220\246", 0)
        );
        comboBox_common_mtype->clear();
        comboBox_common_mtype->insertItems(0, QStringList()
         << QApplication::translate("CommonRuleDialog", "CONNECT", 0)
         << QApplication::translate("CommonRuleDialog", "CONNACK ", 0)
         << QApplication::translate("CommonRuleDialog", "PUBLISH", 0)
         << QApplication::translate("CommonRuleDialog", "PUBACK", 0)
         << QApplication::translate("CommonRuleDialog", "PUBREC", 0)
         << QApplication::translate("CommonRuleDialog", "PUBREL", 0)
         << QApplication::translate("CommonRuleDialog", "PUBCOMP", 0)
         << QApplication::translate("CommonRuleDialog", "SUBSCRIBE ", 0)
         << QApplication::translate("CommonRuleDialog", "SUBACK", 0)
         << QApplication::translate("CommonRuleDialog", "UNSUBSCRIBE", 0)
         << QApplication::translate("CommonRuleDialog", "UNSUBACK ", 0)
         << QApplication::translate("CommonRuleDialog", "PINGREQ ", 0)
         << QApplication::translate("CommonRuleDialog", "PINGRESP  ", 0)
         << QApplication::translate("CommonRuleDialog", "DISCONNECT  ", 0)
        );
        label->setText(QApplication::translate("CommonRuleDialog", "\346\272\220\345\234\260\345\235\200", 0));
        lineEdit_common_saddr->setText(QString());
        label_2->setText(QApplication::translate("CommonRuleDialog", "MQTT\347\261\273\345\236\213", 0));
        label_3->setText(QApplication::translate("CommonRuleDialog", "\346\272\220\345\234\260\345\235\200\346\216\251\347\240\201", 0));
        label_4->setText(QApplication::translate("CommonRuleDialog", "\347\233\256\347\232\204\345\234\260\345\235\200", 0));
        label_5->setText(QApplication::translate("CommonRuleDialog", "\347\233\256\347\232\204\345\234\260\345\235\200\346\216\251\347\240\201", 0));
        label_6->setText(QApplication::translate("CommonRuleDialog", "\346\227\245\345\277\227", 0));
        label_7->setText(QApplication::translate("CommonRuleDialog", "\345\212\250\344\275\234", 0));
        label_8->setText(QApplication::translate("CommonRuleDialog", "\344\275\215\347\275\256 (\351\273\230\350\256\244\346\234\200\345\220\216)", 0));
    } // retranslateUi

};

namespace Ui {
    class CommonRuleDialog: public Ui_CommonRuleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMONRULEDIALOG_H
