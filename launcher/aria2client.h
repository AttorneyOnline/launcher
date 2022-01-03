#ifndef ARIA2CLIENT_H
#define ARIA2CLIENT_H

#include <memory>

#include <QObject>
#include <QMap>

#include <aria2/aria2.h>
#include <QTemporaryFile>

enum DownloadError {
    NO_ERROR = 0,
    UNKNOWN_ERROR,
    TIMEOUT,
    NOT_FOUND,
    NET_ERROR,
    NO_SPACE,
    ALREADY_DOWNLOADING,
    ALREADY_EXISTS,
    CANT_CREATE_FILE,
    IO_ERROR,
    DNS_FAILED,
    BAD_RESPONSE,
    BAD_ARGS,
    SERVER_TEMP_UNAVAIL,
    BAD_CHECKSUM,
};

using sys_time = std::chrono::time_point<std::chrono::steady_clock,
        std::chrono::nanoseconds>;

Q_DECLARE_METATYPE(sys_time)

class Aria2Client : public QObject
{
Q_OBJECT
public:
    explicit Aria2Client(QObject *parent = nullptr);
    ~Aria2Client() override;

    void download(const QString &uri, const QString &checksum, const QString &outFile);
    static QString errorString(DownloadError error);
private:
    aria2::Session *session;
    aria2::SessionConfig config;
    QTemporaryFile *caRootFile;

    static int downloadCallback(aria2::Session* cbSession, aria2::DownloadEvent event,
                                aria2::A2Gid gid, void* userData);
signals:
    void downloadLoop(sys_time from);
    void progress(uint64_t current, uint64_t max, uint64_t speed);
    void finished(DownloadError error);
private slots:
    void downloadLoopBody(sys_time from);
};

#endif // ARIA2CLIENT_H