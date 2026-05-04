#pragma once

#include "Domain/Piece/CutPiece.h"
#include "Domain/Stock/StockBar.h"
#include <string>
#include <vector>

struct CutAssignment {
    std::string stockBarId;
    CutPiece piece;
};

struct BarCutPlan {
    StockBar stockBar;
    std::vector<CutPiece> pieces;
    double remainingLength;
};

struct CutPlan {
    std::vector<BarCutPlan> bars;
    std::vector<CutPiece> unassignedPieces;
    double totalWaste;
};
