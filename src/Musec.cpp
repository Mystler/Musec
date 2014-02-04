/*==LICENSE==
This file is part of Musec.
Copyright (C) 2013 Florian Meißner

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
==LICENSE==*/

#include "Musec.h"
#include "Score.h"
#include "NetMgr.h"
#include "Global.h"
#include <QtGui>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

Musec::Musec(QMainWindow* parent) : QMainWindow(parent)
{
    setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    fTranslator = new QTranslator(this);
    fScore = new Score();
    fNetMgr = new NetMgr(this);
    fPlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    fPlaylist = new QMediaPlaylist(this);
    fPlayer->setPlaylist(fPlaylist);
    fTimer = new QTimer(this);
    fTimer->setSingleShot(true);
    fTimer->setInterval(TIME_HARD * 1000);
    fStartTime = 0;
    fDiffLock = kHard;
    fIsActive = false;
    fDragging = false;

    loadLanguage(getConfig("lang", QLocale::system().name()));
    fExtensions << "*.mp3" << "*.m4a"; // These should contain meta data

    btnMenuMusic->setMenu(menuMusic);
    btnMenuInfo->setMenu(menuInfo);
    btnMenuLanguage->setMenu(menuLanguage);
    btnMenuHelp->setMenu(menuHelp);

    connect(fScore, &Score::multiplierChanged, this, &Musec::multiplierChanged);
    connect(fNetMgr, &NetMgr::done, this, &Musec::scoreSubmitted);
    connect(fTimer, &QTimer::timeout, fPlayer, &QMediaPlayer::stop);
    connect(fPlayer, &QMediaPlayer::mediaStatusChanged, this, &Musec::mediaStatusChanged);
    connect(fPlaylist, &QMediaPlaylist::loaded, this, &Musec::playlistLoaded);
    connect(fPlaylist, &QMediaPlaylist::loadFailed, this, &Musec::playlistLoadFailed);
    connect(slDifficulty, &QSlider::valueChanged, this, &Musec::difficultyChanged);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

Musec::~Musec()
{
    delete fTimer;
    delete fPlaylist;
    delete fPlayer;
    delete fNetMgr;
    delete fScore;
    delete fTranslator;
}

void Musec::setConfig(const QString& key, const QString& value)
{
    QSettings(SETTINGS_AUTHOR, SETTINGS_TITLE).setValue(key, value);
}

QString Musec::getConfig(const QString& key, const QString& defaultVal)
{
    return QSettings(SETTINGS_AUTHOR, SETTINGS_TITLE).value(key, defaultVal).toString();
}

void Musec::loadNext()
{
    fPlaylist->removeMedia(fPlaylist->currentIndex());
    if (fPlaylist->isEmpty()) {
        statusbar->showMessage(tr("No songs left"));
        btnPlay->setDisabled(true);
    }
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

    updateLabels();
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

void Musec::updateLabels()
{
    lblScoreVal->setText(QString::number(fScore->score()));
    lblAverageVal->setText(QString::number(fScore->average(), 'f', 2));
    lblLastVal->setText(QString::number(fScore->lastScore()));
}

void Musec::resetForm()
{
    btnPlay->setText(tr("Play"));
    btnPlay->setDisabled(true);
    btnNext->setDisabled(true);
    edTitle->setDisabled(true);
    edArtist->setDisabled(true);
    edAlbum->setDisabled(true);

    // Reset difficulty
    fDiffLock = kHard;
    slDifficulty->setMinimum(kHard);
    slDifficulty->setValue(kHard);

    fIsActive = false;
}

void Musec::activateForm()
{
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

    fIsActive = true;
}

void Musec::loadLanguage(const QString& lang)
{
    qApp->removeTranslator(fTranslator);
    fTranslator->load(QLocale(lang), "musec", "_", QApplication::applicationDirPath().append("/locales"));
    setConfig("lang", lang);
    qApp->installTranslator(fTranslator);
}

void Musec::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi(this);
}

void Musec::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && event->pos().x() <= headwidget->width() &&
        event->pos().y() <= headwidget->height()) {
        // Enter window move mode
        fDragging = true;
        fDragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Musec::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Leave window move mode
        fDragging = false;
        event->accept();
    }
}

void Musec::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && fDragging) {
        move(event->globalPos() - fDragPos);
        event->accept();
    }
}

void Musec::scoreSubmitted(bool success, QString msg)
{
    if (success) {
        fScore->reset();
        fScore->updateMultiplier(slDifficulty->value(), fPlaylist->mediaCount());
        updateLabels();
    }
    statusbar->showMessage(msg);
}

void Musec::mediaStatusChanged(quint8 status)
{
    qDebug() << fPlayer->mediaStatus();
    if (status == QMediaPlayer::InvalidMedia) {
        loadNext();
        return;
    }
    if (status != QMediaPlayer::LoadedMedia)
        return;

    // Refresh data
    qint64 duration = fPlayer->duration();
    fScore->updateMultiplier(slDifficulty->value(), fPlaylist->mediaCount());
    statusbar->showMessage(tr("Played: %1 (%2 in queue)").arg(
            fScore->played()).arg(fPlaylist->mediaCount()));

    // Skip if too short (<30s)
    if (duration <= 30000) {
        qDebug() << "SKIP: Too short!";
        loadNext();
        return;
    }

    // Skip songs without all tags
    QString title = fPlayer->metaData(QMediaMetaData::Title).toString();
    QString artist = fPlayer->metaData(QMediaMetaData::Author).toString();
    QString album = fPlayer->metaData(QMediaMetaData::AlbumTitle).toString();
    if (title.isEmpty() || artist.isEmpty() || album.isEmpty()) {
        qDebug() << "SKIP: No tags!";
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

    // You shall not move the slider backwards
    if (value < fDiffLock) {
        slDifficulty->setValue(fDiffLock);
        return;
    }

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
    fDiffLock = slDifficulty->value();

    playSong();
}

void Musec::on_btnNext_clicked()
{
    fPlayer->stop();
    resetForm();
    evaluate();
    loadNext();
}

void Musec::on_actAddDir_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Open dir and add music files to playlist
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
            getConfig("music/dir", QDir::homePath()), QFileDialog::ShowDirsOnly |
            QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    QDirIterator it(dir, fExtensions, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        fPlaylist->addMedia(QUrl::fromLocalFile(it.next()));
    setConfig("music/dir", dir);
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
            getConfig("music/dir", QDir::homePath()), "Playlist (*.m3u)");
    if (playlist.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    setConfig("music/dir", QFileInfo(playlist).absolutePath());
    fPlaylist->load(QUrl::fromLocalFile(playlist));
}

void Musec::on_actAddFiles_triggered()
{
    statusbar->showMessage(tr("Loading..."));

    // Add music files to playlist
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Files"),
            getConfig("music/dir", QDir::homePath()), "Music (" + fExtensions.join(" ") + ")");
    if (files.isEmpty()) {
        statusbar->clearMessage();
        return;
    }
    setConfig("music/dir", QFileInfo(files[0]).absolutePath());
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
            tr("Most played difficulty: %1").arg(fScore->averageDifficultyString()));
}

void Musec::on_actSubmit_triggered()
{
    if (fScore->played() < 10) {
        QMessageBox::critical(this, tr("Error"), tr("You need to play at least 10 songs to submit your score!"));
        return;
    }
    bool ok;
    QString username = QInputDialog::getText(this, tr("Submit your score"),
            tr("Your score will be submitted under the following username:"),
            QLineEdit::Normal, getConfig("username", QDir::home().dirName()), &ok);
    if (ok && !username.isEmpty()) {
        setConfig("username", username);
        fNetMgr->submitScore(username, fScore);
    }
}

void Musec::on_actViewScoreboard_triggered()
{
    QDesktopServices::openUrl(QUrl(SCORE_URL));
}

void Musec::on_actLangEn_triggered()
{
    loadLanguage("en");
}

void Musec::on_actLangDe_triggered()
{
    loadLanguage("de");
}

void Musec::on_actLangFr_triggered()
{
    loadLanguage("fr");
}

void Musec::on_actAbout_triggered()
{
    QMessageBox::about(this, tr("About Musec"),
            QString("Musec %1\n").arg(MUSEC_VERSION) +
            tr("Using Qt %1").arg(QT_VERSION_STR) + "\n" +
            tr("Created by") + " Florian Meißner\n\n" +
            tr("Localization:") + "\n" +
            "(DE) Florian Meißner\n" +
            "(FR) Lyrositor\n\n" +
            "www.mystler.eu/musec");
}
