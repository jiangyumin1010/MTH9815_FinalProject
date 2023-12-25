//
//  utility.hpp
//  TradingSystem
//
//  Created by Yumin Jiang
//

#ifndef utility_hpp
#define utility_hpp

#include "boost/date_time/posix_time/posix_time.hpp"
#include "products.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <time.h>
using boost::posix_time::microsec_clock;
using boost::posix_time::ptime;

using namespace std;
using namespace boost::gregorian;

const map<int, pair<string, date>>
    bondMap({{2, {"91282CJL6", date(2025, Nov, 30)}},
             {3, {"91282CJK8", date(2026, Nov, 15)}},
             {5, {"91282CJN2", date(2028, Nov, 30)}},
             {7, {"91282CJM4", date(2030, Nov, 30)}},
             {10, {"91282CJJ1", date(2033, Nov, 15)}},
             {20, {"912810TW8", date(2043, Nov, 30)}},
             {30, {"912810TV0", date(2053, Nov, 15)}}});

const map<string, int> bondId({{"91282CJL6", 2},
                               {"91282CJK8", 3},
                               {"91282CJN2", 5},
                               {"91282CJM4", 7},
                               {"91282CJJ1", 10},
                               {"912810TW8", 20},
                               {"912810TV0", 30}});

// The map between bond id and their coupons
const map<string, double> bondCoupon({{"91282CJL6", 0.04875},
                                      {"91282CJK8", 0.04625},
                                      {"91282CJN2", 0.04375},
                                      {"91282CJM4", 0.04375},
                                      {"91282CJJ1", 0.04500},
                                      {"912810TW8", 0.04750},
                                      {"912810TV0", 0.04750}});

// Convert fractional notation to decimal
double string2price(const std::string& fractional) {
    size_t dashPos = fractional.find('-');
    if (dashPos == std::string::npos) {
        throw std::invalid_argument("Invalid fractional notation");
    }

    double basePrice = std::stod(fractional.substr(0, dashPos));
    int xy = std::stoi(fractional.substr(dashPos + 1, 2));
    char zChar = fractional[dashPos + 3];
    int z = (zChar == '+') ? 4 : zChar - '0';

    if (xy < 0 || xy > 31 || z < 0 || z > 7) {
        throw std::invalid_argument("Invalid fractional components");
    }

    return basePrice + (xy / 32.0) + (z / 256.0);
}

// Convert decimal to fractional notation
std::string price2string(double decimal) {
    int basePrice = static_cast<int>(decimal);
    double fractionalPart = decimal - basePrice;
    int xy = static_cast<int>(fractionalPart * 32);
    int z = static_cast<int>((fractionalPart * 256)) % 8;
    char zChar = (z == 4) ? '+' : '0' + z;
    return std::to_string(basePrice) + "-" + (xy < 10 ? "0" : "") +
           std::to_string(xy) + zChar;
}

Bond GetBond(int maturity) {
    string id = bondMap.at(maturity).first;
    string ticker = "US" + to_string(maturity) + "Y";
    return Bond(id, CUSIP, ticker, bondId.at(id), bondMap.at(maturity).second);
}

Bond GetBond(string _id) {
    int _mat = bondId.at(_id);
    return GetBond(_mat);
}

#endif /* utility_hpp */
