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
#include "Score.h"
#include <QtGui>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QFileDialog>

#define TIME_EASY 5
#define TIME_MEDIUM 3
#define TIME_HARD 1

Musec::Musec(QMainWindow* parent) : QMainWindow(parent)
{
    setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
            Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint |
            Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    fScore = new Score();
    fPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    fPlaylist = new QMediaPlaylist();
    fPlayer->setPlaylist(fPlaylist);
    fTimer = new QTimer(this);
    fTimer->setSingleShot(true);
    fTimer->setInterval(TIME_HARD * 1000);
    fIsActive = false;

    fDir = getConfig("music/dir", QDir::homePath());
    fExtensions << "*.mp3" << "*.m4a"; // These should contain meta data

    connect(fScore, &Score::multiplierChanged, this, &Musec::multiplierChanged);
    connect(fTimer, &QTimer::timeout, fPlayer, &QMediaPlayer::stop);
    connect(fPlayer, &QMediaPlayer::mediaStatusChanged, this, &Musec::mediaStatusChanged);
    connect(fPlaylist, &QMediaPlaylist::loaded, this, &Musec::playlistLoaded);
    connect(fPlaylist, &QMediaPlaylist::loadFailed, this, &Musec::playlistLoadFailed);
    connect(slDifficulty, &QSlider::valueChanged, this, &Musec::difficultyChanged);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

void Musec::setConfig(const QString& key, const QString& value)
{
    QSettings("Mystler", "Musec").setValue(key, value);
}

QString Musec::getConfig(const QString& key, const QString& defaultVal)
{
    return QSettings("Mystler", "Musec").value(key, defaultVal).toString();
}

void Musec::loadNext()
{
    fPlaylist->removeMedia(fPlaylist->currentIndex());
    if (fPlaylist->isEmpty())
        statusbar->showMessage(tr("No songs left"));
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

    bool mTitle = false, mArtist = false, mAlbum = false;
    if (match(edTitle->text(),  title)) {
        mTitle = true;
        chkTitle->setChecked(true);
    }
    if (match(edArtist->text(),  artist)) {
        mArtist = true;
        chkArtist->setChecked(true);
    }
    if (match(edAlbum->text(),  album)) {
        mAlbum = true;
        chkAlbum->setChecked(true);
    }
    fScore->addScore(mTitle, mArtist, mAlbum);

    edTitle->setText(title);
    edArtist->setText(artist);
    edAlbum->setText(album);

    lblScoreVal->setText(QString::number(fScore->score()));
    lblAverageVal->setText(QString::number(fScore->average(), 'f', 2));
    lblLastVal->setText(QString::number(fScore->lastScore()));
}

bool Musec::match(QString str1, QString str2)
{
    // Cast to lower case
    str1 = str1.toLower();
    str2 = str2.toLower();
    // Remove (...), [...] and non-alphabetic characters
    QRegularExpression regex("\\(.*\\)|\\[.*\\]|\\(.*[^\\)]|\\[.*[^\\]]|[^a-zA-Z]");
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
    slDifficulty->setMinimum(kHard);
    slDifficulty->setValue(kHard);
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

void Musec::mediaStatusChanged(quint8 status)
{
    if (status == QMediaPlayer::InvalidMedia)
        loadNext();
    if (status != QMediaPlayer::LoadedMedia)
        return;

    // Refresh data
    qint64 duration = fPlayer->duration();
    fScore->updateMultiplier(slDifficulty->value(), fPlaylist->mediaCount());
    statusbar->showMessage(tr("Played: %1 (%2 in queue)").arg(
            fScore->played()).arg(fPlaylist->mediaCount()));

    // Skip if too short (<30s)
    if (duration <= 30000) {
        loadNext();
        return;
    }

    // Skip songs without all tags
    QString title = fPlayer->metaData(QMediaMetaData::Title).toString();
    QString artist = fPlayer->metaData(QMediaMetaData::Author).toString();
    QString album = fPlayer->metaData(QMediaMetaData::AlbumTitle).toString();
    if (title.isEmpty() || artist.isEmpty() || album.isEmpty()) {
        loadNext();
        return;
    }
    qDebug() << title;
    qDebug() << artist;
    qDebug() << album;

    // Generate start time
    duration /= 1000;
    qint64 startRange = duration * 0.1f;
    qint64 timeRange = duration * 0.8f;
    qint64 random = qrand() % timeRange;
    fStartTime = startRange + random;

    btnPlay->setEnabled(true);
}

void Musec::difficultyChanged(quint8 value)
{
    // Prevent difficulty cheating
    fPlayer->stop();

    // Set time
    switch (value) {
    case kEasy:
        fTimer->setInterval(TIME_EASY * 1000);
        lblDifficulty->setText(QString::number(TIME_EASY) + "s");
        break;
    case kMedium:
        fTimer->setInterval(TIME_MEDIUM * 1000);
        lblDifficulty->setText(QString::number(TIME_MEDIUM) + "s");
        break;
    default:
        fTimer->setInterval(TIME_HARD * 1000);
        lblDifficulty->setText(QString::number(TIME_HARD) + "s");
    }
    fScore->updateMultiplier(value, fPlaylist->mediaCount());
}

void Musec::multiplierChanged(float value)
{
    lblMultiplierVal->setText(QString::number(value, 'f', 2));
}

void Musec::playlistLoaded()
{
    fPlaylist->shuffle();
    fPlaylist->next();
}

void Musec::playlistLoadFailed()
{
    statusbar->showMessage(tr("Could not load playlist"));
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
    fPlayer->stop();
    evaluate();
    resetForm();
    loadNext();
}

void Musec::on_actAddDir_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Open dir and add music files to playlist
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
            fDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    QDirIterator it(dir, fExtensions, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        fPlaylist->addMedia(QUrl::fromLocalFile(it.next()));
    fDir = dir;
    setConfig("music/dir", fDir);
    if (fPlaylist->isEmpty()) {
        statusbar->showMessage(tr("No Songs found"));
        return;
    }

    fPlaylist->shuffle();
    fPlaylist->next();
}

void Musec::on_actAddPlaylist_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Add m3u playlist to playlist
    QString playlist = QFileDialog::getOpenFileName(this, tr("Select Playlist"),
            fDir, "Playlist (*.m3u)");
    if (playlist.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    fDir = QFileInfo(playlist).absolutePath();
    setConfig("music/dir", fDir);
    fPlaylist->load(QUrl::fromLocalFile(playlist));
}

void Musec::on_actAddFiles_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Add music files to playlist
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"),
            fDir, "Music (" + fExtensions.join(" ") + ")");
    if (files.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    fDir = QFileInfo(files[0]).absolutePath();
    setConfig("music/dir", fDir);
    for (int i = 0; i < files.size(); i++)
        fPlaylist->addMedia(QUrl::fromLocalFile(files.at(i)));

    fPlaylist->shuffle();
    fPlaylist->next();
}

void Musec::on_actClear_triggered()
{
    resetForm();
    fPlaylist->clear();
    statusbar->showMessage(tr("No songs left"));
}

void Musec::on_actStats_triggered()
{
    QStringList difficulties;
    difficulties << tr("Hard (%1s)").arg(TIME_HARD);
    difficulties << tr("Medium (%1s)").arg(TIME_MEDIUM);
    difficulties << tr("Easy (%1s)").arg(TIME_EASY);
    QMessageBox::information(this, tr("Statistics"),
            tr("Played: %1").arg(fScore->played()) + "\n" +
            tr("Score: %1").arg(fScore->score()) + "\n" +
            tr("Average: %1").arg(fScore->average(), 0, 'f', 2) + "\n\n" +
            tr("Titles: %1 (%2%)").arg(fScore->correctTitles()).arg(
                    fScore->percentTitles(), 0, 'f', 2) + "\n" +
            tr("Artists: %1 (%2%)").arg(fScore->correctArtists()).arg(
                    fScore->percentArtists(), 0, 'f', 2) + "\n" +
            tr("Albums: %1 (%2%)").arg(fScore->correctAlbums()).arg(
                    fScore->percentAlbums(), 0, 'f', 2) + "\n" +
            tr("Full: %1 (%2%)").arg(fScore->correctSets()).arg(
                    fScore->percentSets(), 0, 'f', 2) + "\n\n" +
            tr("Longest Streak: %1").arg(fScore->longestStreak()) + "\n" +
            tr("Most played difficulty: %1").arg(
                    difficulties.at(fScore->averageDifficulty())));
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

void Musec::on_actLangFr_triggered()
{
    setConfig("lang", "fr");
    QMessageBox::information(this, "Langue changée",
            "Vous devez redémarrer le programme pour que le changement prenne effet.");
}

void Musec::on_actAbout_triggered()
{
    QMessageBox::about(this, tr("About Musec"),
            "Musec\nVersion 1.0.0\n" +
            tr("Using Qt %1").arg(QT_VERSION_STR) + "\n" +
            tr("Created by") + " Florian Meißner 2013\n\n" +
            tr("Localization:") + "\n" +
            "(DE) Florian Meißner\n" +
            "(FR) Lyrositor\n\n" +
            "www.mystler.eu/musec");
}
