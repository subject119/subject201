#include "ScoreManager.h"
#include "GameManager.h"

ScoreManager::ScoreManager()
{
    this->score = 0;
    this->recording = false;
}

void ScoreManager::AddToScore(const int numResolved)
{
    if (this->recording == false || numResolved < 3) return;

    // simple calc, 3 resolve = 100, 4 == 200. 5 == 300, etc...
    int resScore = ((numResolved - 2) * 100);
    this->score += resScore;
}

int ScoreManager::GetScore()
{
    return this->score;
}

void ScoreManager::StartRecording()
{
    this->recording = true;
}

void ScoreManager::StopRecording()
{
    this->recording = false;
}