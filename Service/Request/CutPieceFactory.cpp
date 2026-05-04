#include "CutPieceFactory.h"

std::vector<CutPiece>
CutPieceFactory::expand(const CutRequestItem &item) const {
    std::vector<CutPiece> pieces;
    pieces.reserve(item.getQuantity());

    for (int i = 0; i < item.getQuantity(); i++) {
        pieces.push_back(CutPiece::fromRequestItem(item, i));
    }

    return pieces;
}

std::vector<CutPiece>
CutPieceFactory::expandAll(const std::vector<CutRequestItem> &items) const {
    std::vector<CutPiece> allPieces;

    for (const auto &item : items) {
        auto pieces = expand(item);
        allPieces.insert(allPieces.end(), pieces.begin(), pieces.end());
    }

    return allPieces;
}