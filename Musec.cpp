#include "Musec.h"
#include <QtGui>
#include <QMediaPlayer>
#include <QFileDialog>

Musec::Musec(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    fPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    fTimer = new QTimer(this);
    fTimer->setSingleShot(true);
    fDir = QDir::homePath();

    connect(fTimer, &QTimer::timeout, this, &Musec::timeout);
    connect(fPlayer, &QMediaPlayer::durationChanged, this, &Musec::durationChanged);
}

void Musec::loadSong(QString filename)
{
    fStartTime = 0;
    fPlayer->setMedia(QUrl::fromLocalFile(filename));
}

void Musec::playSong()
{
    fPlayer->setPosition(fStartTime);
    fPlayer->play();
    fTimer->start(1000);
}

void Musec::timeout()
{
    fPlayer->stop();
}

void Musec::durationChanged(qint64 duration)
{
    if (duration <= 0)
        return;
    qint64 timeRange = duration * 0.8f;
    qint64 startRange = duration * 0.1f;
    fStartTime = (qrand() % timeRange) + startRange;
}

void Musec::on_btnPlay_clicked()
{
    playSong();
}

void Musec::on_btnNext_clicked()
{
    lblInfo->setText("");

    // Remove last song
    if (!fSongs.isEmpty())
        fSongs.pop_front();

    // Load next
    if (!fSongs.isEmpty())
        loadSong(fSongs.first());
    else
        lblInfo->setText(tr("No more songs left"));
}

void Musec::on_btnBrowse_clicked()
{
    lblInfo->setText("");
    QStringList exts;
    exts << "*.mp3" << "*.m4a"; // These should contain meta data

    // Open dir and add music files to fSongs
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            fDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    lblInfo->setText(tr("Loading..."));
    QDirIterator it(dir, exts, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        fSongs << it.next();
    fDir = dir;
    if (fSongs.isEmpty()) {
        lblInfo->setText(tr("No Songs found"));
        return;
    }

    // Shuffle song list
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for (int i = fSongs.size() - 1; i > 0; i--) {
        int random = qrand() % fSongs.count();
        QString str = fSongs[i];
        fSongs[i] = fSongs[random];
        fSongs[random] = str;
    }

    // Load first song
    lblInfo->setText(QString().setNum(fSongs.size()) + " " + tr("Songs added"));
    loadSong(fSongs.first());
}
