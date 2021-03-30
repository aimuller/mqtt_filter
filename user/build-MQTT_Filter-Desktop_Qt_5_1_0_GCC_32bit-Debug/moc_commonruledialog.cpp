/****************************************************************************
** Meta object code from reading C++ file 'commonruledialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MQTT_Filter/commonruledialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'commonruledialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CommonRuleDialog_t {
    QByteArrayData data[10];
    char stringdata[158];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CommonRuleDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CommonRuleDialog_t qt_meta_stringdata_CommonRuleDialog = {
    {
QT_MOC_LITERAL(0, 0, 16),
QT_MOC_LITERAL(1, 17, 19),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 7),
QT_MOC_LITERAL(4, 46, 19),
QT_MOC_LITERAL(5, 66, 21),
QT_MOC_LITERAL(6, 88, 34),
QT_MOC_LITERAL(7, 123, 5),
QT_MOC_LITERAL(8, 129, 19),
QT_MOC_LITERAL(9, 149, 7)
    },
    "CommonRuleDialog\0addCommonRuleSignal\0"
    "\0RULE_ST\0modCommonRuleSignal\0"
    "on_buttonBox_accepted\0"
    "on_comboBox_common_mtype_activated\0"
    "mtype\0on_checkBox_clicked\0checked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CommonRuleDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x05,
       4,    2,   44,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       5,    0,   49,    2, 0x08,
       6,    1,   50,    2, 0x08,
       8,    1,   53,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void CommonRuleDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CommonRuleDialog *_t = static_cast<CommonRuleDialog *>(_o);
        switch (_id) {
        case 0: _t->addCommonRuleSignal((*reinterpret_cast< RULE_ST(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->modCommonRuleSignal((*reinterpret_cast< RULE_ST(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->on_buttonBox_accepted(); break;
        case 3: _t->on_comboBox_common_mtype_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->on_checkBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CommonRuleDialog::*_t)(RULE_ST , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CommonRuleDialog::addCommonRuleSignal)) {
                *result = 0;
            }
        }
        {
            typedef void (CommonRuleDialog::*_t)(RULE_ST , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CommonRuleDialog::modCommonRuleSignal)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject CommonRuleDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CommonRuleDialog.data,
      qt_meta_data_CommonRuleDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *CommonRuleDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommonRuleDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CommonRuleDialog.stringdata))
        return static_cast<void*>(const_cast< CommonRuleDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int CommonRuleDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CommonRuleDialog::addCommonRuleSignal(RULE_ST _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CommonRuleDialog::modCommonRuleSignal(RULE_ST _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
