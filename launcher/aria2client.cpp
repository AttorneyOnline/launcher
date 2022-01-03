#include "aria2client.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>

// https://aria2.github.io/manual/en/html/aria2c.html#exit-status
const QMap<int, DownloadError> errorCodes {
        {0, NO_ERROR},
        {1, UNKNOWN_ERROR},
        {2, TIMEOUT},
        {3, NOT_FOUND},
        {6, NET_ERROR},
        {9, NO_SPACE},
        {11, ALREADY_DOWNLOADING},
        {13, ALREADY_EXISTS},
        {16, CANT_CREATE_FILE},
        {17, IO_ERROR},
        {19, DNS_FAILED},
        {22, BAD_RESPONSE},
        {28, BAD_ARGS},
        {29, SERVER_TEMP_UNAVAIL},
        {32, BAD_CHECKSUM},
};

const QMap<DownloadError, QString> errorCodeMessages {
        {NO_ERROR, "There was no error."},
        {UNKNOWN_ERROR, "An unknown error occurred."},
        {TIMEOUT, "The download timed out."},
        {NOT_FOUND, "The file to be downloaded was not found on the server."},
        {NET_ERROR, "There was a network error during the download."},
        {NO_SPACE, "There is not enough disk space available."},
        {ALREADY_DOWNLOADING, "A download for this file is already in progress."},
        {ALREADY_EXISTS, "The file to be downloaded already exists."},
        {CANT_CREATE_FILE, "There was an error creating the file to be downloaded."},
        {IO_ERROR, "There was an I/O error writing to the downloaded file."},
        {DNS_FAILED, "DNS resolution failed - check your Internet connection."},
        {BAD_RESPONSE, "There was a bad response from the server."},
        {BAD_ARGS, "Invalid download URL."},
        {SERVER_TEMP_UNAVAIL, "The server is temporarily unavailable. Please try again later."},
        {BAD_CHECKSUM, "The file was downloaded, but its checksum is invalid."},
};

const char *TEMP_LOG_FILE = "download.log";

Aria2Client::Aria2Client(QObject *parent) : QObject(parent)
{
    static bool library_inited = false;
    if (!library_inited) {
        aria2::libraryInit();
        library_inited = true;
    }

    config.downloadEventCallback = downloadCallback;
    config.userData = this;

    qRegisterMetaType<sys_time>();
    connect(this, &Aria2Client::downloadLoop,
            this, &Aria2Client::downloadLoopBody, Qt::QueuedConnection);
    connect(this, &Aria2Client::finished, [this](DownloadError err) {
        if (err) return;
        QFile(TEMP_LOG_FILE).remove();
    });
}

Aria2Client::~Aria2Client() = default;

void Aria2Client::download(const QString &uri, const QString &checksum,
                           const QString &outFile)
{
    session = aria2::sessionNew(aria2::KeyVals {
        { "log", TEMP_LOG_FILE },
        { "log-level", "info" },
        { "ca-certificate", "ca-certificates.crt" },
    }, config);

    QFileInfo fileInfo(outFile);
    aria2::KeyVals options {
            { "max-connection-per-server", "3" },
            { "check-integrity", "true" },
            { "dir", fileInfo.absolutePath().toStdString() },
            { "out", fileInfo.fileName().toStdString() },
    };

    if (!checksum.isEmpty())
        options.push_back({ "checksum",
                            QStringLiteral("sha-1=%1").arg(checksum).toStdString() });

    aria2::addUri(session, nullptr, { uri.toStdString() }, options);

    auto start = std::chrono::steady_clock::now();
    qDebug() << "aria2client: started download";
    emit downloadLoop(start);
}

void Aria2Client::downloadLoopBody(sys_time start)
{
    if (aria2::run(session, aria2::RUN_ONCE) < 1) {
        int err = aria2::sessionFinal(session);
        qDebug() << "aria2client: getting out of aria2 event loop - err code" << err;
        emit finished(errorCodes.value(err, UNKNOWN_ERROR));
        return;
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
            (now - start).count();
    if (elapsed >= 100) {
        start = now;
        std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session);
        for (const auto &gid : gids) {
            aria2::DownloadHandle *dh = aria2::getDownloadHandle(session, gid);
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

int Aria2Client::downloadCallback(aria2::Session *cbSession,
                                  aria2::DownloadEvent event, aria2::A2Gid gid,
                                  void *userData)
{
    auto *client = static_cast<Aria2Client *>(userData);

    // (Newline is printed on object destruction)
    QDebug debug = qDebug().nospace().noquote();
    debug << " ["
          << QString::fromStdString(aria2::gidToHex(gid))
          << "] ";

    aria2::DownloadHandle* dh = aria2::getDownloadHandle(cbSession, gid);

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

    switch (event) {
        case aria2::EVENT_ON_DOWNLOAD_START:
            debug << " started";
            break;
        case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
        case aria2::EVENT_ON_BT_DOWNLOAD_COMPLETE:
            debug << " finished";
            break;
        case aria2::EVENT_ON_DOWNLOAD_ERROR:
        case aria2::EVENT_ON_DOWNLOAD_STOP:
            debug << " stopped/had an error";
            break;
        default:
            qWarning() << "unknown aria2 event" << event;
            debug << " had an unknown event";
    }

    aria2::deleteDownloadHandle(dh);

    return 0;
}

QString Aria2Client::errorString(DownloadError error) {
    return errorCodeMessages.value(error, "Unknown error code");
}
