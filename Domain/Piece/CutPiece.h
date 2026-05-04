#pragma once

#include "Domain/CutRequest/CutRequestItem.h"
#include <string>

class CutPiece {
  private:
    std::string id;
    std::string requestItemId;
    double length;

  public:
    CutPiece();
    CutPiece(std::string id, std::string requestItemId, double length);

    /// Getter zone

    const std::string &getId() const;
    const std::string &getRequestItemId() const;
    double getLength() const;

    static CutPiece fromRequestItem(const CutRequestItem &item, int index);
};
