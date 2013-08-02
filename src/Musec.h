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

#ifndef Musec_h
#define Musec_h

#include "ui_musec.h"

class QMediaPlayer;
class QMediaPlaylist;
class QTimer;
class QTranslator;
class Score;
class NetMgr;

class Musec : public QMainWindow, private Ui::MusecMainWindow {
    Q_OBJECT

public:
    Musec(QMainWindow* parent = nullptr);
    static void setConfig(const QString& key, const QString& value);
    static QString getConfig(const QString& key, const QString& defaultVal);

private:
    QTranslator* fTranslator;
    Score* fScore;
    NetMgr* fNetMgr;
    QMediaPlayer* fPlayer;
    QMediaPlaylist* fPlaylist;
    QTimer* fTimer;
    QStringList fExtensions;
    qint64 fStartTime;
    bool fIsActive;
    void loadNext();
    void playSong();
    void evaluate();
    bool match(QString str1, QString str2);
    void resetForm();
    void activateForm();
    void loadLanguage(const QString& lang);
    void changeEvent(QEvent* event);

private slots:
    void scoreSubmitted(bool success, QString msg);
    void mediaStatusChanged(quint8 status);
    void difficultyChanged(quint8 value);
    void multiplierChanged(float value);
    void playlistLoaded();
    void playlistLoadFailed();
    void on_btnPlay_clicked();
    void on_btnNext_clicked();
    void on_actAddDir_triggered();
    void on_actAddPlaylist_triggered();
    void on_actAddFiles_triggered();
    void on_actClear_triggered();
    void on_actStats_triggered();
    void on_actSubmit_triggered();
    void on_actLangEn_triggered();
    void on_actLangDe_triggered();
    void on_actLangFr_triggered();
    void on_actAbout_triggered();
};
#endif
