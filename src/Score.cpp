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

#include "Score.h"

#define POINTS_TITLE 3
#define POINTS_ARTIST 1
#define POINTS_ALBUM 2
#define MULTIPLIER_EASY 1
#define MULTIPLIER_MEDIUM 2
#define MULTIPLIER_HARD 4

Score::Score()
{
    fMultiplier = 0.f;
    fScore = 0;
    fPlayed = 0;
    fLastScore = 0;
    fCorrectTitles = 0;
    fCorrectArtists = 0;
    fCorrectAlbums = 0;
    fCorrectSets = 0;
    fDifficulty = kHard;

    for (quint8 i = 0; i < kNumDifficulties; i++)
        fDiffPlayed[i] = 0;
}

void Score::addScore(bool title, bool artist, bool album)
{
    fPlayed++;
    fDiffPlayed[fDifficulty]++;
    fLastScore = 0;

    if (title) {
        fLastScore += POINTS_TITLE;
        fCorrectTitles++;
    }
    if (artist) {
        fLastScore += POINTS_ARTIST;
        fCorrectArtists++;
    }
    if (album) {
        fLastScore += POINTS_ALBUM;
        fCorrectAlbums++;
    }
    if (title && artist && album)
        fCorrectSets++;

    fLastScore = fLastScore * fMultiplier + 0.5f;
    fScore += fLastScore;
}

void Score::updateMultiplier(quint8 difficulty, quint32 songs)
{
    fMultiplier = 1.f + songs / 200.f;

    // Multiplier for difficulty
    fDifficulty = difficulty;
    if (fDifficulty == kHard)
        fMultiplier *= MULTIPLIER_HARD;
    else if (fDifficulty == kMedium)
        fMultiplier *= MULTIPLIER_MEDIUM;
    else if (fDifficulty == kEasy)
        fMultiplier *= MULTIPLIER_EASY;
    else
        fMultiplier = 1.f;

    emit multiplierChanged(fMultiplier);
}

quint8 Score::averageDifficulty()
{
    quint8 result = 0;
    quint32 highest = 0;
    for (quint8 i = 0; i < kNumDifficulties; i++) {
        if (fDiffPlayed[i] > highest) {
            highest = fDiffPlayed[i];
            result = i;
        }
    }
    return result;
}

float Score::average()
{
    if (fPlayed == 0)
        return 0.f;
    return fScore / fPlayed;
}

float Score::percentPlayed(quint32 count)
{
    if (fPlayed == 0)
        return 0.f;
    return 100 * count / fPlayed;
}
