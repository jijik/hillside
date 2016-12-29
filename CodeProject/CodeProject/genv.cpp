#include "genv.h"

Model gModel;

Market gMarket;

double pi = 3.14159265358979323846;

double gBuySmoothWeight = 0.7;
double gSellSmoothWeight = 0.7;

double gIdealCurveMovingAvgHistoryNeighbors = 3.1; //double because of combinator (rounding down)
double gIdealCurveMovingAvgFutureNeighbors = 3.1;

AppMode::type gAppMode = AppMode::FindBest;

double gTotalProfit = 0.0;