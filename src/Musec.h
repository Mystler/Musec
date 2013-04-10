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
class QTimer;

class Musec : public QWidget, private Ui::MusecMainWindow {
    Q_OBJECT

public:
    Musec(QWidget* parent = 0);

private:
    QMediaPlayer* fPlayer;
    QTimer* fTimer;
    QStringList fExtensions;
    QStringList fSongs;
    QString fDir;
    qint64 fStartTime;
    quint32 fScore;
    bool fIsActive;
    void shuffleList();
    void loadSong(QString filename);
    void playSong();
    void evaluate();
    bool match(QString str1, QString str2);
    void resetForm();
    void activateForm();

private slots:
    void timeout();
    void durationChanged(qint64 duration);
    void on_btnPlay_clicked();
    void on_btnNext_clicked();
    void on_btnAddDir_clicked();
    void on_btnAddFiles_clicked();
    void on_btnClear_clicked();
};
#endif
