#include "launcherui.h"
#include "launcherqmlinterface.h"

#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <qqmlapplicationengine.h>
#include <qqmlcomponent.h>

using namespace Launcher;

LauncherUI::LauncherUI(QObject *parent)
    : QObject{parent}
    , display_engine{new QQmlEngine(this)}
{
  QQmlComponent component(display_engine, this);
  connect(&component, &QQmlComponent::statusChanged, this, [this](QQmlComponent::Status status) { qDebug() << status; });
  component.loadUrl(QUrl::fromLocalFile("D:/AO/AODEV/launcher/data/main.qml"));
  component.create(display_engine->rootContext());
}
