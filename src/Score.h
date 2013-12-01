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

#ifndef Score_h
#define Score_h

#include <QObject>

enum Difficulty {
    kHard = 0,
    kMedium,
    kEasy,
    kNumDifficulties
};

class Score : public QObject {
    Q_OBJECT

public:
    Score();
    void reset();
    void addScore(bool title, bool artist, bool album);
    void updateMultiplier(quint8 difficulty, quint32 songs);
    QString difficultyString(quint8 difficulty) const;
    quint8 averageDifficulty() const;
    float average() const;

    float multiplier() const { return fMultiplier; }
    quint32 score() const { return fScore; }
    quint32 played() const { return fPlayed; }
    quint32 lastScore() const { return fLastScore; }
    quint32 correctTitles() const { return fCorrectTitles; }
    quint32 correctArtists() const { return fCorrectArtists; }
    quint32 correctAlbums() const { return fCorrectAlbums; }
    quint32 correctSets() const { return fCorrectSets; }
    quint32 longestStreak() const { return fLongestStreak; }
    float percentTitles() const { return percentPlayed(fCorrectTitles); }
    float percentArtists() const { return percentPlayed(fCorrectArtists); }
    float percentAlbums() const { return percentPlayed(fCorrectAlbums); }
    float percentSets() const { return percentPlayed(fCorrectSets); }
    QString averageDifficultyString() const { return difficultyString(averageDifficulty()); }

private:
    float fMultiplier;
    quint32 fScore;
    quint32 fPlayed;
    quint32 fLastScore;
    quint32 fCorrectTitles;
    quint32 fCorrectArtists;
    quint32 fCorrectAlbums;
    quint32 fCorrectSets;
    quint32 fLongestStreak;
    quint32 fCurrentStreak;
    quint32 fDiffPlayed[kNumDifficulties];
    quint8 fDifficulty;

    float percentPlayed(quint32 count) const;

signals:
    void multiplierChanged(float value);
};
#endif
