#pragma once

#include "Domain/Piece/CutPiece.h"
#include "Domain/CutRequest/CutRequestItem.h"
#include <vector>

class CutPieceFactory {
  public:
    std::vector<CutPiece> expand(const CutRequestItem &item) const;
    std::vector<CutPiece>
    expandAll(const std::vector<CutRequestItem> &items) const;
};
