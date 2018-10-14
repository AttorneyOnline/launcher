#include "runtimeerror.h"

#include <QDebug>

RuntimeError::RuntimeError(const QString msg) {
    data = msg.toStdString();
}

const char *RuntimeError::what() const noexcept {
    qCritical() << data.c_str();
    return data.c_str();
}
