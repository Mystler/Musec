/*==LICENSE==
This file is part of Musec.
Copyright (C) 2013 Florian Meißner

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
==LICENSE==*/

#include "Musec.h"
#include <QtGui>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QFileDialog>

Musec::Musec(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
            Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint |
            Qt::MSWindowsFixedSizeDialogHint);
    fPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    fTimer = new QTimer(this);
    fTimer->setSingleShot(true);
    fDir = QDir::homePath();

    fExtensions << "*.mp3" << "*.m4a"; // These should contain meta data

    connect(fTimer, &QTimer::timeout, this, &Musec::timeout);
    connect(fPlayer, &QMediaPlayer::durationChanged, this, &Musec::durationChanged);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

void Musec::shuffleList()
{
    // Shuffle song list
    for (int i = fSongs.size() - 1; i > 0; i--) {
        int random = qrand() % fSongs.count();
        QString str = fSongs[i];
        fSongs[i] = fSongs[random];
        fSongs[random] = str;
    }

    // Load first song
    lblInfo->setText(QString().setNum(fSongs.size()) + tr(" Songs loaded (total)"));
    loadSong(fSongs.first());
}

void Musec::loadSong(QString filename)
{
    fStartTime = 0;
    fPlayer->setMedia(QUrl::fromLocalFile(filename));
}

void Musec::playSong()
{
    lblInfo->setText(QTime(0,0).addSecs(fStartTime).toString("mm:ss"));
    fPlayer->setPosition(fStartTime * 1000);
    fPlayer->play();
    fTimer->start(1000);
}

void Musec::evaluate()
{

}

void Musec::resetForm()
{
    edTitle->clear();
    edArtist->clear();
    edAlbum->clear();
    btnPlay->setText(tr("Play"));
    btnPlay->setDisabled(true);
    btnNext->setDisabled(true);
    edTitle->setDisabled(true);
    edArtist->setDisabled(true);
    edAlbum->setDisabled(true);
}

void Musec::activateForm()
{
    btnPlay->setText(tr("Play Again"));
    btnNext->setEnabled(true);
    edTitle->setEnabled(true);
    edArtist->setEnabled(true);
    edAlbum->setEnabled(true);
}

void Musec::timeout()
{
    fPlayer->stop();
}

void Musec::durationChanged(qint64 duration)
{
    if (duration <= 0)
        return;
    duration /= 1000;
    qint64 startRange = duration * 0.1f;
    qint64 timeRange = duration * 0.8f;
    qint64 random = qrand() % timeRange;
    fStartTime = startRange + random;
    qDebug() << fPlayer->metaData(QMediaMetaData::Title).toString();
    qDebug() << fPlayer->metaData(QMediaMetaData::Author).toString();
    qDebug() << fPlayer->metaData(QMediaMetaData::AlbumTitle).toString();
    btnPlay->setEnabled(true);
}

void Musec::on_btnPlay_clicked()
{
    activateForm();
    playSong();
}

void Musec::on_btnNext_clicked()
{
    evaluate();
    resetForm();

    // Remove song from queue
    if (!fSongs.isEmpty())
        fSongs.pop_front();

    // Load next song
    if (!fSongs.isEmpty())
        loadSong(fSongs.first());
    else
        lblInfo->setText(tr("No more songs left"));
}

void Musec::on_btnAddDir_clicked()
{
    lblInfo->setText(tr("Loading..."));

    // Open dir and add music files to fSongs
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
            fDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        lblInfo->clear();
        return;
    }
    QDirIterator it(dir, fExtensions, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        fSongs << it.next();
    fDir = dir;
    if (fSongs.isEmpty()) {
        lblInfo->setText(tr("No Songs found"));
        return;
    }

    shuffleList();
}

void Musec::on_btnAddFiles_clicked()
{
    lblInfo->setText(tr("Loading..."));

    // Add music files to fSongs
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"),
            fDir, "Music (" + fExtensions.join(" ") + ")");
    if (files.isEmpty()) {
        lblInfo->clear();
        return;
    }
    fDir = QFileInfo(files[0]).absolutePath();
    fSongs += files;
    if (fSongs.isEmpty()) {
        lblInfo->setText(tr("No Songs found"));
        return;
    }

    shuffleList();
}

void Musec::on_btnClear_clicked()
{
    resetForm();
    fSongs.clear();
    lblInfo->setText(tr("No more songs left"));
}
