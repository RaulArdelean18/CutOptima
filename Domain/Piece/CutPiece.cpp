#include "CutPiece.h"
#include <stdexcept>
#include <utility>

CutPiece::CutPiece() : length(0.0) {}

CutPiece::CutPiece(std::string id, std::string requestItemId, double length)
    : id(std::move(id)), requestItemId(std::move(requestItemId)),
      length(length) {
    if (this->id.empty()) {
        throw std::invalid_argument("CutPiece id cannot be empty.");
    }
    if (this->requestItemId.empty()) {
        throw std::invalid_argument("CutPiece request item id cannot be empty.");
    }
    if (this->length <= 0) {
        throw std::invalid_argument("CutPiece length must be positive.");
    }
}

const std::string &CutPiece::getId() const { return id; }

const std::string &CutPiece::getRequestItemId() const { return requestItemId; }

double CutPiece::getLength() const { return length; }

CutPiece CutPiece::fromRequestItem(const CutRequestItem &item, int index) {
    if (index < 0) {
        throw std::invalid_argument("CutPiece index cannot be negative.");
    }
    if (index >= item.getQuantity()) {
        throw std::invalid_argument(
            "CutPiece index must be smaller than request item quantity.");
    }
    return CutPiece(item.getId() + "-" + std::to_string(index + 1),
                    item.getId(), item.getRequiredLength());
}
