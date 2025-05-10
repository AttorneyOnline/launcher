#pragma once

#include <QString>

namespace Launcher
{
  static QString organisation()
  {
    return QStringLiteral("@LAUNCHER_ORGANISATION");
  }

  static QString name()
  {
    return QStringLiteral("AOLauncher");
  }

  static QString version()
  {
    return QStringLiteral("0.1.0");
  }

  static QString userAgent()
  {
    return QStringLiteral("AOLauncher/0.1.0");
  }
}; // namespace Launcher
