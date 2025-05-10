#pragma once

#include <QObject>

namespace Launcher
{
  Q_NAMESPACE

  enum class Endpoint
  {
    CLIENTVERSION,
    LAUNCHERVERSION,
    MEDIAREPOSITORY,
  };
  Q_ENUM_NS(Endpoint)
} // namespace Launcher
