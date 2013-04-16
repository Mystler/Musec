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
    fScore = 0;
    fPlayed = 0;
    fLastScore = 0;
    fCorrectTitles = 0;
    fCorrectArtists = 0;
    fCorrectAlbums = 0;
    fCorrectSets = 0;
}

void Score::addScore(bool title, bool artist, bool album)
{
    fPlayed++;
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

void Score::updateMultiplier(int difficulty, quint32 songs)
{
    fMultiplier = 1.f + songs / 200.f;

    // Multiplier for difficulty
    if (difficulty == Difficulty::kHard)
        fMultiplier *= MULTIPLIER_HARD;
    else if (difficulty == Difficulty::kMedium)
        fMultiplier *= MULTIPLIER_MEDIUM;
    else if (difficulty == Difficulty::kEasy)
        fMultiplier *= MULTIPLIER_EASY;
    else
        fMultiplier = 1.f;

    emit multiplierChanged(fMultiplier);
}
