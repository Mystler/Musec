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

#define POINTS_TITLE 2
#define POINTS_ARTIST 2
#define POINTS_ALBUM 1

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
    fScore = 0;
    fSongsPlayed = 0;
    fIsActive = false;

    fExtensions << "*.mp3" << "*.m4a"; // These should contain meta data

    connect(fTimer, &QTimer::timeout, this, &Musec::timeout);
    connect(fPlayer, &QMediaPlayer::durationChanged, this, &Musec::durationChanged);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

void Musec::shuffleList()
{
    // Shuffle song list
    for (int i = fSongs.size() - 1; i > 0; i--) {
        int random = qrand() % fSongs.size();
        QString str = fSongs[i];
        fSongs[i] = fSongs[random];
        fSongs[random] = str;
    }

    // Load first song
    lblInfo->setText(QString::number(fSongs.size()) + tr(" Songs loaded (total)"));
    loadSong(fSongs.first());
}

void Musec::loadSong(QString filename)
{
    fStartTime = 0;
    fPlayer->setMedia(QUrl::fromLocalFile(filename));
}

void Musec::playSong()
{
    lblInfo->setText(QTime(0,0,0).addSecs(fStartTime).toString("mm:ss"));
    fPlayer->setPosition(fStartTime * 1000);
    fPlayer->play();
    fTimer->start(1000);
}

void Musec::evaluate()
{
    QString title = fPlayer->metaData(QMediaMetaData::Title).toString();
    QString artist = fPlayer->metaData(QMediaMetaData::Author).toString();
    QString album = fPlayer->metaData(QMediaMetaData::AlbumTitle).toString();

    if (match(edTitle->text(),  title)) {
        fScore += POINTS_TITLE;
        chkTitle->setChecked(true);
    }
    if (match(edArtist->text(),  artist)) {
        fScore += POINTS_ARTIST;
        chkArtist->setChecked(true);
    }
    if (match(edAlbum->text(),  album)) {
        fScore += POINTS_ALBUM;
        chkAlbum->setChecked(true);
    }

    edTitle->setText(title);
    edArtist->setText(artist);
    edAlbum->setText(album);

    fSongsPlayed++;
    quint32 maxScore = fSongsPlayed * (POINTS_TITLE +
            POINTS_ARTIST + POINTS_ALBUM);
    lblScore->setText(tr("Score: ") + QString::number(fScore) +
            " (" + QString::number(fScore * 100 / maxScore) + "%)");
}

bool Musec::match(QString str1, QString str2)
{
    // Cast to lower case
    str1 = str1.toLower();
    str2 = str2.toLower();
    // Remove (...), [...] and non-alphabetic characters
    QRegularExpression regex("(\\(.*\\))|(\\[.*\\])|[^a-zA-Z]");
    str1.remove(regex);
    str2.remove(regex);

    // Exact match
    if (str1 == str2)
        return true;
    return false;
}

void Musec::resetForm()
{
    fIsActive = false;
    btnPlay->setText(tr("Play"));
    btnPlay->setDisabled(true);
    btnNext->setDisabled(true);
    edTitle->setDisabled(true);
    edArtist->setDisabled(true);
    edAlbum->setDisabled(true);
}

void Musec::activateForm()
{
    fIsActive = true;
    edTitle->clear();
    edArtist->clear();
    edAlbum->clear();
    btnPlay->setText(tr("Play Again"));
    btnNext->setEnabled(true);
    edTitle->setEnabled(true);
    edArtist->setEnabled(true);
    edAlbum->setEnabled(true);
    chkTitle->setChecked(false);
    chkArtist->setChecked(false);
    chkAlbum->setChecked(false);
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
    if (!fIsActive)
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
