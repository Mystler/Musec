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
#include <QMessageBox>
#include <QFileDialog>

#define POINTS_TITLE 3
#define POINTS_ARTIST 1
#define POINTS_ALBUM 2
#define TIME_EASY 5
#define TIME_MEDIUM 3
#define TIME_HARD 1
#define MULTIPLIER_EASY 1
#define MULTIPLIER_MEDIUM 2
#define MULTIPLIER_HARD 5

Musec::Musec(QMainWindow* parent) : QMainWindow(parent)
{
    setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
            Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint |
            Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    fPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    fTimer = new QTimer(this);
    fTimer->setSingleShot(true);
    fScore = 0;
    fSongsPlayed = 0;
    fIsActive = false;

    fDir = getConfig("music/dir", QDir::homePath());
    fExtensions << "*.mp3" << "*.m4a"; // These should contain meta data

    connect(fTimer, &QTimer::timeout, this, &Musec::timeout);
    connect(fPlayer, &QMediaPlayer::durationChanged, this, &Musec::durationChanged);
    connect(slDifficulty, &QSlider::valueChanged, this, &Musec::difficultyChanged);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

void Musec::setConfig(const QString& key, const QString& value)
{
    QSettings("Mystler", "Musec").setValue(key, value);
}

QString Musec::getConfig(const QString& key, const QString& default)
{
    return QSettings("Mystler", "Musec").value(key, default).toString();
}

void Musec::shuffleList()
{
    // Shuffle song list
    for (int i = fSongs.size() - 1; i >= 0; i--) {
        int random = qrand() % fSongs.size();
        QString str = fSongs[i];
        fSongs[i] = fSongs[random];
        fSongs[random] = str;
    }

    // Load first song
    loadSong(fSongs.first());
}

void Musec::loadSong(const QString& filename)
{
    fStartTime = 0;
    fPlayer->setMedia(QUrl::fromLocalFile(filename));
    updateMultiplier();
    statusbar->showMessage(tr("Played: %1 (%2 in queue)").arg(
            QString::number(fSongsPlayed)).arg(QString::number(fSongs.size())));
}

void Musec::playSong()
{
    statusbar->showMessage(QTime(0,0,0).addSecs(fStartTime).toString("mm:ss"));
    fPlayer->setPosition(fStartTime * 1000);
    fPlayer->play();
    fTimer->start();
}

void Musec::evaluate()
{
    QString title = fPlayer->metaData(QMediaMetaData::Title).toString();
    QString artist = fPlayer->metaData(QMediaMetaData::Author).toString();
    QString album = fPlayer->metaData(QMediaMetaData::AlbumTitle).toString();

    quint32 score = 0;
    if (match(edTitle->text(),  title) || title.isEmpty()) {
        score += POINTS_TITLE;
        chkTitle->setChecked(true);
    }
    if (match(edArtist->text(),  artist) || artist.isEmpty()) {
        score += POINTS_ARTIST;
        chkArtist->setChecked(true);
    }
    if (match(edAlbum->text(),  album) || album.isEmpty()) {
        score += POINTS_ALBUM;
        chkAlbum->setChecked(true);
    }
    score = score * fMultiplier + 0.5f;
    fScore += score;

    edTitle->setText(title);
    edArtist->setText(artist);
    edAlbum->setText(album);

    fSongsPlayed++;
    lblScore->setText(tr("Score: %1").arg(QString::number(fScore)));
    float avg = fScore/fSongsPlayed;
    lblAverage->setText(tr("Average: %1").arg(QString::number(avg, 'f', 2)));
    lblLast->setText(tr("Last Score: %1").arg(QString::number(score)));
}

bool Musec::match(QString str1, QString str2)
{
    // Cast to lower case
    str1 = str1.toLower();
    str2 = str2.toLower();
    // Remove (..., [... and non-alphabetic characters
    QRegularExpression regex("(\\(.*\\))|(\\[.*\\])|[^a-zA-Z]");
    str1.remove(regex);
    str2.remove(regex);

    // Exact match
    if (str1 == str2)
        return true;

    // Allow one mistake every 5 letters
    int tolerance = str1.length()/5;
    int i = 0, j = 0, diff = 0, mismatches = 0;
    while (i < str1.length() && j < str2.length()) {
        if (str1.at(i) == str2.at(j)) {
            diff = i - j;
            i++;
            j++;
        } else {
            if (diff == i - j) {
                mismatches++;
                diff--;
                i++;
            } else {
                j++;
            }
        }
    }
    mismatches += str1.length() - i;
    mismatches += str2.length() - j;
    if (mismatches <= tolerance)
        return true;

    return false;
}

void Musec::updateMultiplier()
{
    // Multiplier for number of songs in queue
    fMultiplier = 1 + fSongs.size()/200;

    // Multiplier for difficulty
    switch (slDifficulty->value()) {
    case 3:
        fMultiplier *= MULTIPLIER_EASY;
        fTimer->setInterval(TIME_EASY * 1000);
        lblDifficulty->setText(QString::number(TIME_EASY) + "s");
        break;
    case 2:
        fMultiplier *= MULTIPLIER_MEDIUM;
        fTimer->setInterval(TIME_MEDIUM * 1000);
        lblDifficulty->setText(QString::number(TIME_MEDIUM) + "s");
        break;
    default:
        fMultiplier *= MULTIPLIER_HARD;
        fTimer->setInterval(TIME_HARD * 1000);
        lblDifficulty->setText(QString::number(TIME_HARD) + "s");
    }

    lblMultiplier->setText(tr("Multiplier: %1").arg(
            QString::number(fMultiplier, 'f', 2)));
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

    // Reset difficulty
    slDifficulty->setMinimum(1);
    slDifficulty->setValue(1);
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

void Musec::difficultyChanged(int value)
{
    updateMultiplier();
}

void Musec::on_btnPlay_clicked()
{
    // Activate input
    if (!fIsActive)
        activateForm();
    // Prevent difficulty cheating
    slDifficulty->setMinimum(slDifficulty->value());
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
        statusbar->showMessage(tr("No more songs left"));
}

void Musec::on_actAddDir_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Open dir and add music files to fSongs
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
            fDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    QDirIterator it(dir, fExtensions, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        fSongs << it.next();
    fDir = dir;
    setConfig("music/dir", fDir);
    if (fSongs.isEmpty()) {
        statusbar->showMessage(tr("No Songs found"));
        return;
    }

    shuffleList();
}

void Musec::on_actAddFiles_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Add music files to fSongs
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"),
            fDir, "Music (" + fExtensions.join(" ") + ")");
    if (files.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    fDir = QFileInfo(files[0]).absolutePath();
    setConfig("music/dir", fDir);
    fSongs += files;
    if (fSongs.isEmpty()) {
        statusbar->showMessage(tr("No Songs found"));
        return;
    }

    shuffleList();
}

void Musec::on_actClear_triggered()
{
    resetForm();
    fSongs.clear();
    statusbar->showMessage(tr("No more songs left"));
}

void Musec::on_actStats_triggered()
{
    // TODO
}

void Musec::on_actLangEn_triggered()
{
    setConfig("lang", "en");
    QMessageBox::information(this, "Language changed",
            "You have to restart the program for the change to take effect.");
}

void Musec::on_actLangDe_triggered()
{
    setConfig("lang", "de");
    QMessageBox::information(this, "Sprache geändert",
            "Sie müssen das Programm neustarten, damit die Änderung aktiv ist.");
}

void Musec::on_actAbout_triggered()
{
    QMessageBox::about(this, tr("About Musec"),
            "Musec\nVersion: 1.0.0\n" +
            tr("Created by") + " Florian Meißner 2013\n\n" +
            "www.mystler.eu/musec");
}

void Musec::on_actAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}
