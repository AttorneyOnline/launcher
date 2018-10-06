#include "runtimeerror.h"

RuntimeError::RuntimeError(const QString &msg) {
    data = msg.toLocal8Bit().constData();
}

const char *RuntimeError::what() const noexcept {
    return data;
}
