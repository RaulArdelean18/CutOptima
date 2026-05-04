#include "CutPiece.h"
#include "BarProfile.h"
#include "CutRequestItem.h"
#include "Material.h"
#include "StockBar.h"
#include <iostream>

int main() {
    Material aluminum("mat1", "Aluminum", 3.0, true);

    BarProfile profile("prof1", "Aluminum 40x20", "ALU-40-20", aluminum, 6000.0,
                       120.0);

    StockBar stockBar("stock1", "BARCODE-0001", profile, 6000.0, 6000.0);

    CutRequestItem requestItem("req1", 1200.0, 3, {"mat1"}, {"prof1"});

    CutPiece piece = CutPiece::fromRequestItem(requestItem, 0);

    std::cout << aluminum.getName() << '\n';
    std::cout << profile.getName() << '\n';
    std::cout << stockBar.getRemainingLength() << '\n';
    std::cout << piece.getLength() << '\n';

    return 0;
}