//
//  main.cpp
//  TradingSystem
//
//  Created by Yumin Jiang
//

#include "AlgoExecutionService.hpp"
#include "AlgoStreamingService.hpp"
#include "DataGenerator.hpp"
#include "GUIservice.hpp"
#include "executionservice.hpp"
#include "historicaldataservice.hpp"
#include "inquiryservice.hpp"
#include "marketdataservice.hpp"
#include "positionservice.hpp"
#include "pricingservice.hpp"
#include "products.hpp"
#include "riskservice.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"
#include "tradebookingservice.hpp"
#include <iostream>
#include <random>

int main() {
    // Step 1: Generate all the data needed
    GeneratePrices();
    GenerateTrades();
    GenerateInquiries();
    GenerateMarketData();
    std::cout << "====== Data Genrated. ======" << std::endl;

    // Step 2: Use Bond as the productType, register all the service
    MarketDataService<Bond> BondMarketDataService;
    PricingService<Bond> BondPricingService;
    TradeBookingService<Bond> BondTradeBookingService;
    PositionService<Bond> BondPositionService;
    RiskService<Bond> BondRiskService;
    AlgoExecutionService<Bond> BondAlgoExecutionService;
    AlgoStreamingService<Bond> BondAlgoStreamingService;
    ExecutionService<Bond> BondExecutionService;
    StreamingService<Bond> BondStreamingService;
    InquiryService<Bond> BondInquiryService;
    GUIService<Bond> BondGUIService;
    HistoricalDataService<Position<Bond>> BondHistoricalPositionService(
        POSITION);
    HistoricalDataService<PV01<Bond>> BondHistoricalRiskService(RISK);
    HistoricalDataService<ExecutionOrder<Bond>> BondHistoricalExecutionService(
        EXECUTION);
    HistoricalDataService<PriceStream<Bond>> BondHistoricalStreamingService(STREAMING);
    HistoricalDataService<Inquiry<Bond>> BondHistoricalInquiryService(INQUIRY);
    std::cout << "====== Services initialized! ======\n";

    // Step 3: Link corresponding service

    BondPricingService.AddListener(BondGUIService.GetListener());
    BondPricingService.AddListener(BondAlgoStreamingService.GetListener());
    BondAlgoStreamingService.AddListener(BondStreamingService.GetListener());
    BondStreamingService.AddListener(
        BondHistoricalStreamingService.GetServiceListener());
    BondMarketDataService.AddListener(BondAlgoExecutionService.GetListener());
    BondAlgoExecutionService.AddListener(BondExecutionService.GetListener());
    BondExecutionService.AddListener(
        BondHistoricalExecutionService.GetServiceListener());
    BondExecutionService.AddListener(BondTradeBookingService.GetListener());
    BondTradeBookingService.AddListener(BondPositionService.GetListener());
    BondPositionService.AddListener(BondRiskService.GetListener());
    BondPositionService.AddListener(
        BondHistoricalPositionService.GetServiceListener());
    BondRiskService.AddListener(BondHistoricalRiskService.GetServiceListener());
    BondInquiryService.AddListener(
        BondHistoricalInquiryService.GetServiceListener());
    std::cout << "====== Services linked. ======" << std::endl;

    // Step 4: Read data and write to output
    const string dirPath = "Data/Input/";

    ifstream priceData(dirPath + "prices.txt");
    BondPricingService.GetConnector()->Subscribe(priceData);
    std::cout << "Price processed!" << std::endl;

    ifstream marketData(dirPath + "marketdata.txt");
    BondMarketDataService.GetConnector()->Subscribe(marketData);
    std::cout << "Market data processed!" << std::endl;

    ifstream tradeData(dirPath + "trades.txt");
    BondTradeBookingService.GetConnector()->Subscribe(tradeData);
    std::cout << "Trade processed!" << std::endl;

    ifstream inquiryData(dirPath + "inquiries.txt");
    BondInquiryService.GetConnector()->Subscribe(inquiryData);
    std::cout << "Inquiry data processed!" << std::endl;

    std::cout << "====== All Finished! ======" << std::endl;

    return 0;
}
