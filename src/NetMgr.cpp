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

#include "NetMgr.h"
#include "Score.h"
#include "Global.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

NetMgr::NetMgr()
{
    fAccessMgr = new QNetworkAccessManager(this);
    connect(fAccessMgr, &QNetworkAccessManager::finished, this, &NetMgr::netReply);
}

void NetMgr::submitScore(const QString& user, const Score* score)
{
    QUrl url(SCORE_URL "/submit.php");
    QByteArray postData;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    postData.append(QString("user=%1&").arg(user));
    postData.append(QString("avg=%1&").arg(score->average(), 0, 'f', 2));
    postData.append(QString("score=%1&").arg(score->score()));
    postData.append(QString("played=%1&").arg(score->played()));
    postData.append(QString("bingo=%1&").arg(score->correctSets()));
    postData.append(QString("streak=%1&").arg(score->longestStreak()));
    postData.append(QString("diff=%1").arg(score->averageDifficultyString()));
    fAccessMgr->post(request, postData);
}

void NetMgr::netReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
        emit replied(true, tr("Score submitted"));
    else
        emit replied(false, tr("Network Error"));
}
