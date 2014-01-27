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

#include "NetMgr.h"
#include "Score.h"
#include "Global.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QMessageBox>

NetMgr::NetMgr(QWidget* parent) : fParent(parent)
{
    fAccessMgr = new QNetworkAccessManager(this);
}

NetMgr::~NetMgr()
{
    delete fAccessMgr;
}

void NetMgr::submitScore(const QString& user, const Score* score)
{
    // Check for existing entry via get request
    QNetworkRequest get(QUrl(QString(SCORE_URL "u/%1").arg(user)));
    QNetworkReply* checkReply = fAccessMgr->get(get);
    QEventLoop wait;
    connect(checkReply, &QNetworkReply::finished, &wait, &QEventLoop::quit);
    wait.exec();
    if (checkReply->error() == QNetworkReply::NoError) {
        // Is the existing entry better?
        quint32 oldScore = QString::fromUtf8(checkReply->readAll()).toUInt();
        if (oldScore > score->score()) {
            auto result = QMessageBox::warning(fParent, tr("Found existing entry!"),
                tr("There already is a better score for you.\nAre you sure you want to submit?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (result != QMessageBox::Yes)
                return;
        }
    }
    checkReply->close();

    // Submit via post request
    QNetworkRequest post(QUrl(SCORE_URL));
    post.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray postData;
    postData.append(QString("user=%1&").arg(user));
    postData.append(QString("avg=%1&").arg(score->average(), 0, 'f', 2));
    postData.append(QString("score=%1&").arg(score->score()));
    postData.append(QString("played=%1&").arg(score->played()));
    postData.append(QString("bingo=%1&").arg(score->correctSets()));
    postData.append(QString("streak=%1&").arg(score->longestStreak()));
    postData.append(QString("diff=%1").arg(score->averageDifficultyString()));
    QNetworkReply* submitReply = fAccessMgr->post(post, postData);
    connect(submitReply, &QNetworkReply::finished, &wait, &QEventLoop::quit);
    wait.exec();
    if (submitReply->error() == QNetworkReply::NoError)
        emit done(true, tr("Score submitted"));
    else
        emit done(false, tr("Network Error"));
    submitReply->close();
}
