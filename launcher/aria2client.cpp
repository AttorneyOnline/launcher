#include "aria2client.h"
#include <aria2api.h>
#include <QCoreApplication>
#include <QDebug>

Aria2Client::Aria2Client(QObject *parent) : QObject(parent)
{
  aria2::libraryInit();
  session.reset(aria2::sessionNew(aria2::KeyVals(), config));
  connect(this, &Aria2Client::downloadLoop,
          this, &Aria2Client::downloadLoopBody, Qt::QueuedConnection);
}

Aria2Client::~Aria2Client()
{
  aria2::sessionFinal(session.get());
  aria2::libraryDeinit();
}

void Aria2Client::download(const QString &uri, const QString &checksum,
                           const QString &outFile)
{
  aria2::KeyVals options {
    { "max-connection-per-server", "3" },
    { "file-allocation", "falloc" },
    { "check-integrity", "true" },
    { "out", outFile.toStdString() }
  };

  if (!checksum.isEmpty())
    options.push_back({ "checksum",
                        QStringLiteral("sha1=%1").arg(checksum).toStdString() });

  aria2::addUri(session.get(), nullptr, { uri.toStdString() }, options);

  auto start = std::chrono::steady_clock::now();
  emit downloadLoop(start);
}

void Aria2Client::downloadLoopBody(sys_time start)
{
  if (aria2::run(session.get(), aria2::RUN_ONCE) < 1)
    return;

  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
      (now - start).count();
  if (elapsed >= 100) {
    start = now;
    std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session.get());
    for (const auto &gid : gids) {
      aria2::DownloadHandle *dh = aria2::getDownloadHandle(session.get(), gid);
      if (!dh) continue;
      emit progress(dh->getCompletedLength(), dh->getTotalLength(),
                    dh->getDownloadSpeed());
      aria2::deleteDownloadHandle(dh);
    }
  }

  // This shouldn't cause an infinite loop as long as the signal delivery is
  // queued and not direct, which will force Qt to handle other events in its
  // own event loop before getting to the aria2 event loop.
  emit downloadLoop(start);
}

int Aria2Client::downloadCallback(aria2::Session *session,
                                  aria2::DownloadEvent event, aria2::A2Gid gid,
                                  [[gnu::unused]] void *userData)
{
  switch (event) {
  case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
  case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
    emit finished(NO_ERROR);
    break;
  case aria2::EVENT_ON_DOWNLOAD_ERROR:
  case aria2::EVENT_ON_DOWNLOAD_STOP:

    emit finished(CONNECTION_FAILED);
    break;
  default:
    qWarning() << "unknown aria2 event" << event;
    return 0;
  }

  QDebug debug = qDebug().nospace().noquote();
  debug << " ["
        << QString::fromStdString(aria2::gidToHex(gid))
        << "] ";

  aria2::DownloadHandle* dh = aria2::getDownloadHandle(session, gid);

  if (!dh) {
    qWarning() << "no aria2::DownloadHandle provided in event!";
    return 0;
  }

  if (dh->getNumFiles() > 0) {
    aria2::FileData f = dh->getFile(1);
    // Path may be empty if the file name has not been determined yet.
    if (f.path.empty() && !f.uris.empty()) {
      debug << QString::fromStdString(f.uris[0].uri);
    } else {
      debug << QString::fromStdString(f.path);
    }
  }

  // (Newline is printed on object destruction)
  debug << " finished";

  aria2::deleteDownloadHandle(dh);

  return 0;
}

