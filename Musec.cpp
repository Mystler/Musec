#include <QtGui>
#include <QMediaPlayer>
#include <QFileDialog>
#include <stdint.h>
#include "Musec.h"

Musec::Musec(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    fPlayer = new QMediaPlayer(this);
}

void Musec::loadSong(QString filename)
{

}

void Musec::on_btnPlay_clicked()
{

}

void Musec::on_btnNext_clicked()
{

}

void Musec::on_btnBrowse_clicked()
{
    QStringList exts;
    exts << "*.mp3" << "*.wav" << "*.ogg" << "*.m4a";

    //Open dir and add music files to fSongs
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDirIterator it(dir, exts, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        fSongs << it.next();
    }

    //Shuffle SongList
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for (size_t i = fSongs.count() - 1 ; i > 0 ; --i ) {
        uint32_t random = qrand() % fSongs.count();
        QString str = fSongs[i];
        fSongs[i] = fSongs[random];
        fSongs[random] = str;
    }
}
