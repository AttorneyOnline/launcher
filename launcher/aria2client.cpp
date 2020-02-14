#include "aria2client.h"

#include <QCoreApplication>
#include <QDebug>

Aria2Client::Aria2Client(QObject *parent) : QObject(parent)
{
  aria2::libraryInit();
  session.reset(aria2::sessionNew(aria2::KeyVals(), config));
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
  while (true) {
    QCoreApplication::processEvents();
    if (aria2::run(session.get(), aria2::RUN_ONCE) < 1)
      break;

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
  }
}

int Aria2Client::downloadCallback(aria2::Session *session,
                                  aria2::DownloadEvent event, aria2::A2Gid gid,
                                  [[gnu::unused]] void *userData)
{
  switch (event) {
  case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
  case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
    emit finished(false);
    break;
  case aria2::EVENT_ON_DOWNLOAD_ERROR:
  case aria2::EVENT_ON_DOWNLOAD_STOP:
    emit finished(true);
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

  debug << " finished";

  aria2::deleteDownloadHandle(dh);

  return 0;
}

