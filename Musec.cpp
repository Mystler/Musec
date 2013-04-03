#include <QtGui>
#include <QMediaPlayer>
#include <QFileDialog>
#include <stdint.h>
#include "Musec.h"

Musec::Musec(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    fPlayer = new QMediaPlayer(this);
    fTimer = new QTimer(this);
    fDir = QDir::homePath();

    connect(fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void Musec::loadSong(QString filename)
{
    fPlayer->setMedia(QUrl::fromLocalFile(filename));
    qint64 timeRange = fPlayer->duration() * 0.8f;
    qint64 startRange = fPlayer->duration() * 0.1f;
    fStartTime = (qrand() % timeRange) + startRange;
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

void Musec::on_btnPlay_clicked()
{
    playSong();
}

void Musec::on_btnNext_clicked()
{
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

    // Shuffle song list
    if (fSongs.isEmpty()) {
        lblInfo->setText(tr("No Songs found"));
        return;
    }
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for (size_t i = fSongs.size() - 1; i > 0; i--) {
        uint32_t random = qrand() % fSongs.count();
        QString str = fSongs[i];
        fSongs[i] = fSongs[random];
        fSongs[random] = str;
    }
    lblInfo->setText(QString().setNum(fSongs.size()) + " " + tr("Songs added"));

    // Load first song
    loadSong(fSongs.first());
}
