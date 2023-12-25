//
//  DataGenerator.hpp
//  TradingSystem
//
//  Created by Yumin Jiang
//

#ifndef DataGenerator_hpp
#define DataGenerator_hpp

#include <boost/date_time/gregorian/gregorian.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "utility.hpp"

using namespace std;
using namespace boost::gregorian;

// Defined as 1,000,000 as instrction
int DATASIZE = 10000;
const string dirPath = "Data/Input/";

void GeneratePrices() {
    const string filePath = dirPath + "prices.txt";
    ofstream file;
    file.open(filePath);
    
    if (!file.is_open()) {
        cerr << "Error: Unable to open file at " << filePath << endl;
        return;
    }
    const int orderSize = DATASIZE; // Number of prices per security
    const double minTick = 1 / 256.0;
    const double LOW_LIMIT = 99.0 + minTick * 2.0;
    const double UPPER_LIMIT = 101.0 - minTick * 2.0;

    for (const auto& [mat, bond] : bondMap) {
        // cout << "Generating prices for security " << bond.first << " ...\n";

        double central_price = LOW_LIMIT;
        bool up = true;
        thread_local random_device rd;
        thread_local mt19937_64 gen(rd());
        thread_local bernoulli_distribution d(0.5);

        for (int i = 0; i < orderSize; i++) {
            double ask = central_price + minTick;
            double bid = central_price - minTick;

            if (d(gen))
                ask += minTick;
            if (d(gen))
                bid -= minTick;

            central_price += up ? minTick : -minTick;
            if (central_price >= UPPER_LIMIT)
                up = false;
            if (central_price <= LOW_LIMIT)
                up = true;

            file << bond.first << "," << price2string(bid) << ","
                 << price2string(ask) << endl;

        }
    }

    file.close();
    cout << "prices.txt Generated!\n";
}



void GenerateMarketData() {
    const string filePath = dirPath + "marketdata.txt";
    ofstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file at " << filePath << endl;
        return;
    }

    const int orderSize = DATASIZE / 10;  // Number of order book updates per security
    const double minTick = 1.0 / 256.0;
    const double basePrice = 99.0;  // Base price for oscillation

    for (const auto& [mat, bond] : bondMap) {
        double price = basePrice;
        bool increasing = true;

        for (int i = 0; i < orderSize; ++i) {
            for (int level = 1; level <= 5; ++level) {
                double spread = level * minTick;
                double bidPrice = price - spread;
                double askPrice = price + spread;
                int size = level * 10000000;  // 10 million, 20 million, etc.

                file << bond.first << "," << price2string(bidPrice) << "," << size << ",BID" <<endl;
                file << bond.first << "," << price2string(askPrice) << "," << size << ",OFFER" << endl;
            }

            // Oscillate price
            if (price >= 101.0 - minTick) increasing = false;
            if (price <= 99.0 + minTick) increasing = true;
            price += increasing ? minTick : -minTick;
        }
    }

    file.close();
    cout << "marketdata.txt Generated!\n";
}

void GenerateInquiries() {
    const string filePath = dirPath + "inquiries.txt";
    ofstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file at " << filePath << endl;
        return;
    }
    
    thread_local random_device rd;
    thread_local mt19937_64 gen(rd());
    thread_local uniform_real_distribution<double> d(0.0, 1.0);
    const double minTick = 1 / 256.0;

    for (const auto& [mat, bond] : bondMap) {
        for (int i = 0; i < 10; ++i) {
            int _n = (int)(d(gen) * 512);
            double _price = 99.0 + minTick * (double)_n;
            
            string inquiryId = bond.first + "_INQ" + to_string(i);
            string side = (i % 2 == 0) ? "BUY" : "SELL";
            int quantity = ((i % 5) + 1) * 1000000;  // 1 million, 2 million, etc.
            
            file << inquiryId << "," << bond.first << "," << side << "," << quantity << "," << price2string(_price) << ",RECEIVED" << endl;
        }
    }

    file.close();
    cout << "inquiries.txt Generated!\n";
}

void GenerateTrades() {
    const string filePath = dirPath + "trades.txt";
    ofstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file at " << filePath << endl;
        return;
    }
    
    thread_local random_device rd;
    thread_local mt19937_64 gen(rd());
    thread_local uniform_real_distribution<double> d(0.0, 1.0);
    const double minTick = 1 / 256.0;

    for (const auto& [mat, bond] : bondMap) {
        for (int i = 0; i < 10; ++i) {
            string tradeId = bond.first + "_TRADE" + to_string(i);
            string side = (i % 2 == 0) ? "BUY" : "SELL";
            int quantity = ((i % 5) + 1) * 1000000;  // 1 million, 2 million, etc.
            
            int _market = (int)(d(gen) * 3) % 3 + 1;
            string _book_name = "TRSY" + to_string(_market);
            
            int _n = (int)(d(gen) * 512);
            double _price = 99.0 + minTick * (double)_n;
            
            file << bond.first << "," << tradeId << "," << price2string(_price) << ","<< _book_name <<"," << quantity << "," << side<< endl;
        }
    }

    file.close();
    cout << "trades.txt Generated!\n";
}

#endif /* DataGenerator_hpp */
