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

#ifndef NetMgr_h
#define NetMgr_h

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class Score;

class NetMgr : public QObject {
    Q_OBJECT

public:
    NetMgr();
    void submitScore(const QString& user, const Score* score);

private:
    QNetworkAccessManager* fAccessMgr;

private slots:
    void netReply(QNetworkReply* reply);

signals:
    void replied(bool success, QString msg);
};
#endif
