/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MQTT_Filter/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[23];
    char stringdata[533];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 13),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 7),
QT_MOC_LITERAL(4, 34, 13),
QT_MOC_LITERAL(5, 48, 15),
QT_MOC_LITERAL(6, 64, 17),
QT_MOC_LITERAL(7, 82, 30),
QT_MOC_LITERAL(8, 113, 30),
QT_MOC_LITERAL(9, 144, 29),
QT_MOC_LITERAL(10, 174, 30),
QT_MOC_LITERAL(11, 205, 30),
QT_MOC_LITERAL(12, 236, 32),
QT_MOC_LITERAL(13, 269, 36),
QT_MOC_LITERAL(14, 306, 36),
QT_MOC_LITERAL(15, 343, 31),
QT_MOC_LITERAL(16, 375, 17),
QT_MOC_LITERAL(17, 393, 4),
QT_MOC_LITERAL(18, 398, 33),
QT_MOC_LITERAL(19, 432, 5),
QT_MOC_LITERAL(20, 438, 21),
QT_MOC_LITERAL(21, 460, 36),
QT_MOC_LITERAL(22, 497, 34)
    },
    "MainWindow\0addCommonRule\0\0RULE_ST\0"
    "modCommonRule\0updateSystemLog\0"
    "updateConnectList\0on_pushButton_add_rule_clicked\0"
    "on_pushButton_mod_rule_clicked\0"
    "on_pushButton_mf_open_clicked\0"
    "on_pushButton_mf_close_clicked\0"
    "on_pushButton_del_rule_clicked\0"
    "on_pushButton_clear_rule_clicked\0"
    "on_action_export_rule_file_triggered\0"
    "on_action_Import_rule_file_triggered\0"
    "on_tableWidget_rule_itemEntered\0"
    "QTableWidgetItem*\0item\0"
    "on_tableWidget_rule_doubleClicked\0"
    "index\0on_pushButton_clicked\0"
    "on_pushButton_update_connect_clicked\0"
    "on_tableWidget_connect_itemEntered\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   99,    2, 0x08,
       4,    2,  104,    2, 0x08,
       5,    0,  109,    2, 0x08,
       6,    0,  110,    2, 0x08,
       7,    0,  111,    2, 0x08,
       8,    0,  112,    2, 0x08,
       9,    0,  113,    2, 0x08,
      10,    0,  114,    2, 0x08,
      11,    0,  115,    2, 0x08,
      12,    0,  116,    2, 0x08,
      13,    0,  117,    2, 0x08,
      14,    0,  118,    2, 0x08,
      15,    1,  119,    2, 0x08,
      18,    1,  122,    2, 0x08,
      20,    0,  125,    2, 0x08,
      21,    0,  126,    2, 0x08,
      22,    1,  127,    2, 0x08,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, QMetaType::QModelIndex,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->addCommonRule((*reinterpret_cast< RULE_ST(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->modCommonRule((*reinterpret_cast< RULE_ST(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->updateSystemLog(); break;
        case 3: _t->updateConnectList(); break;
        case 4: _t->on_pushButton_add_rule_clicked(); break;
        case 5: _t->on_pushButton_mod_rule_clicked(); break;
        case 6: _t->on_pushButton_mf_open_clicked(); break;
        case 7: _t->on_pushButton_mf_close_clicked(); break;
        case 8: _t->on_pushButton_del_rule_clicked(); break;
        case 9: _t->on_pushButton_clear_rule_clicked(); break;
        case 10: _t->on_action_export_rule_file_triggered(); break;
        case 11: _t->on_action_Import_rule_file_triggered(); break;
        case 12: _t->on_tableWidget_rule_itemEntered((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 13: _t->on_tableWidget_rule_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 14: _t->on_pushButton_clicked(); break;
        case 15: _t->on_pushButton_update_connect_clicked(); break;
        case 16: _t->on_tableWidget_connect_itemEntered((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
