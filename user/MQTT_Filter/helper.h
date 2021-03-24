#ifndef HELPRE_H
#define HELPRE_H
#include "header.h"
#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QFileDialog>
#include <QMap>
#define DEFAULT_DIR "../../rule/"

int open_mf_dev();
int close_mf_dev(int fd);

QString rule2ip(u_int32_t addr, u_int32_t mask);
QString rule2mtype(u_int8_t mtype);
QString rule2log(u_int8_t log);
QString rule2action(u_int8_t action);

u_int32_t addr2rule(QString str_addr);
u_int32_t mask2rule(QString str_mask);
u_int8_t mtype2rule(QString str_mtype);
u_int8_t log2rule(QString str_log);
u_int8_t action2rule(QString str_action);

#endif // HELPRE_H
