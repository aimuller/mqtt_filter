#ifndef HELPRE_H
#define HELPRE_H
#include "header.h"
#include <QString>
#include <QDebug>

int open_mf_dev();
int close_mf_dev(int fd);

QString byte2ip(u_int32_t addr);
QString byte2mtype(u_int8_t mtype);
QString byte2log(u_int8_t log);
QString byte2action(u_int8_t action);

#endif // HELPRE_H
