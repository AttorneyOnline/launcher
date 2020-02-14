#ifndef ARIA2CLIENT_H
#define ARIA2CLIENT_H

#include <memory>

#include <QObject>

#include <aria2/aria2.h>

enum DownloadError {
  NO_ERROR,
  CONNECTION_FAILED
};

class Aria2Client : public QObject
{
  Q_OBJECT
public:
  explicit Aria2Client(QObject *parent = nullptr);
  ~Aria2Client();

  void download(const QString &uri, const QString &checksum, const QString &outFile);
private:
  std::unique_ptr<aria2::Session> session;
  aria2::SessionConfig config;

  int downloadCallback(aria2::Session* session, aria2::DownloadEvent event,
                       aria2::A2Gid gid, void* userData);
signals:
  void progress(uint64_t current, uint64_t max, uint64_t speed);
  void finished(bool error);
};

#endif // ARIA2CLIENT_H
