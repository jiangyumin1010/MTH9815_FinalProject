/**
 * marketdataservice.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef MARKET_DATA_SERVICE_HPP
#define MARKET_DATA_SERVICE_HPP

#include "soa.hpp"
#include "utility.hpp"
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };

/**
 * A market data order with price, quantity, and side.
 */
class Order {

  public:
    // ctor for an order
    Order(double _price, long _quantity, PricingSide _side);
    Order() = default;

    // Get the price on the order
    double GetPrice() const;

    // Get the quantity on the order
    long GetQuantity() const;

    // Get the side on the order
    PricingSide GetSide() const;

  private:
    double price;
    long quantity;
    PricingSide side;
};

/**
 * Class representing a bid and offer order
 */
class BidOffer {

  public:
    // ctor for bid/offer
    BidOffer(const Order& _bidOrder, const Order& _offerOrder);
    BidOffer() = default;

    // Get the bid order
    const Order& GetBidOrder() const;

    // Get the offer order
    const Order& GetOfferOrder() const;

  private:
    Order bidOrder;
    Order offerOrder;
};

/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template <typename T> class OrderBook {

  public:
    // ctor for the order book
    OrderBook() = default;
    OrderBook(const T& _product, const vector<Order>& _bidStack,
              const vector<Order>& _offerStack);

    // Get the product
    const T& GetProduct() const;

    // Get the bid stack
    const vector<Order>& GetBidStack() const;

    // Get the offer stack
    const vector<Order>& GetOfferStack() const;

    // Get the best bid/offer order (the ones at the top)
    const BidOffer GetBidOffer() const;

  private:
    T product;
    vector<Order> bidStack;
    vector<Order> offerStack;
};

// Pre-declaration of connector
template <typename T> class MarketDataConnector;

/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template <typename T>
class MarketDataService : public Service<string, OrderBook<T>> {
  public:
    // ctor
    MarketDataService();

    ~MarketDataService();

    // Get data on our service given product id
    OrderBook<T>& GetData(string _key);

    // Call back function that a Connector should invoke for any new or updated
    // data
    void OnMessage(OrderBook<T>& _data);

    // Add listener to the service
    void AddListener(ServiceListener<OrderBook<T>>* listener);

    // Get all the listeners on the service
    const vector<ServiceListener<OrderBook<T>>*>& GetListeners() const;

    // Get the connector
    MarketDataConnector<T>* GetConnector();

    // Get the current orderbook depth
    int GetOrderBookDepth() const;

    // Get the best bid/offer order
    const BidOffer GetBestBidOffer(const string& productId);

    // Aggregate the order book
    const OrderBook<T>& AggregateDepth(const string& productId);

  private:
    map<string, OrderBook<T>> orderBooks;
    vector<ServiceListener<OrderBook<T>>*> listeners;
    MarketDataConnector<T>* connector;
    int bookDepth;
};

/**
 * Market Data Connector
 * Type T is the product type.
 */
template <typename T>
class MarketDataConnector : public Connector<OrderBook<T>> {
  public:
    // ctor
    MarketDataConnector(MarketDataService<T>* _service);

    // Publish data to the Connector
    void Publish(OrderBook<T>& _data);

    // Subscribe Data from the Connector
    void Subscribe(ifstream& data);

  private:
    MarketDataService<T>* service;
};

Order::Order(double _price, long _quantity, PricingSide _side) {
    price = _price;
    quantity = _quantity;
    side = _side;
}

double Order::GetPrice() const { return price; }

long Order::GetQuantity() const { return quantity; }

PricingSide Order::GetSide() const { return side; }

BidOffer::BidOffer(const Order& _bidOrder, const Order& _offerOrder)
    : bidOrder(_bidOrder), offerOrder(_offerOrder) {}

const Order& BidOffer::GetBidOrder() const { return bidOrder; }

const Order& BidOffer::GetOfferOrder() const { return offerOrder; }

template <typename T>
OrderBook<T>::OrderBook(const T& _product, const vector<Order>& _bidStack,
                        const vector<Order>& _offerStack)
    : product(_product), bidStack(_bidStack), offerStack(_offerStack) {}

template <typename T> const T& OrderBook<T>::GetProduct() const {
    return product;
}

template <typename T> const vector<Order>& OrderBook<T>::GetBidStack() const {
    return bidStack;
}

template <typename T> const vector<Order>& OrderBook<T>::GetOfferStack() const {
    return offerStack;
}

// Get the highest bid, lowest offer
template <typename T> const BidOffer OrderBook<T>::GetBidOffer() const {
    Order highest_bid(bidStack[0]);
    double h_bid_price = highest_bid.GetPrice();
    for (auto i = 1; i < bidStack.size(); i++) {
        Order curr_bid = bidStack[i];
        if (curr_bid.GetPrice() > h_bid_price) {
            highest_bid = curr_bid;
            h_bid_price = highest_bid.GetPrice();
        }
    }

    Order lowest_offer(offerStack[0]);
    double l_offer_price = lowest_offer.GetPrice();
    for (auto i = 1; i < offerStack.size(); i++) {
        Order curr_offer = offerStack[i];
        if (curr_offer.GetPrice() < l_offer_price) {
            lowest_offer = curr_offer;
            l_offer_price = lowest_offer.GetPrice();
        }
    }

    return BidOffer(highest_bid, lowest_offer);
}

template <typename T> MarketDataService<T>::MarketDataService() {
    orderBooks = map<string, OrderBook<T>>();
    listeners = vector<ServiceListener<OrderBook<T>>*>();
    connector = new MarketDataConnector<T>(this);
    bookDepth = 10;
}

template <typename T> MarketDataService<T>::~MarketDataService() {
    delete connector;
}

template <typename T> OrderBook<T>& MarketDataService<T>::GetData(string _key) {
    return orderBooks[_key];
}

template <typename T>
void MarketDataService<T>::OnMessage(OrderBook<T>& _data) {
    string product_id = _data.GetProduct().GetProductId();
    orderBooks[product_id] = _data;

    for (auto& listener : listeners) {
        listener->ProcessAdd(_data);
    }
}

template <typename T>
void MarketDataService<T>::AddListener(
    ServiceListener<OrderBook<T>>* listener) {
    listeners.push_back(listener);
}

template <typename T>
const vector<ServiceListener<OrderBook<T>>*>&
MarketDataService<T>::GetListeners() const {
    return listeners;
}

template <typename T>
MarketDataConnector<T>* MarketDataService<T>::GetConnector() {
    return connector;
}

template <typename T> int MarketDataService<T>::GetOrderBookDepth() const {
    return bookDepth;
}

// Get the best bid/offer order
template <typename T>
const BidOffer MarketDataService<T>::GetBestBidOffer(const string& _id) {
    return orderBooks[_id].GetBidOffer();
}

// Aggregate the order book
template <typename T>
const OrderBook<T>& MarketDataService<T>::AggregateDepth(const string& _id) {
    T& _product = orderBooks[_id].GetProduct();

    vector<Order>& origBidStack = orderBooks[_id].GetBidStack();
    vector<Order>& origOfferStack = orderBooks[_id].GetOfferStack();

    unordered_map<double, long> bidMap;
    unordered_map<double, long> offerMap;

    for (auto& bid : origBidStack) {
        double p = bid.GetPrice();
        double q = bid.GetQuantity();
        bidMap[p] += q;
    }
    vector<Order> newBidStack;
    for (auto& [price, quantity] : bidMap) {
        Order tmpBidOrder(price, quantity, BID);
        newBidStack.push_back(tmpBidOrder);
    }

    for (auto& offer : origOfferStack) {
        double p = offer.GetPrice();
        double q = offer.GetQuantity();
        offerMap[p] += q;
    }
    vector<Order> newOfferStack;
    for (auto& [price, quantity] : bidMap) {
        Order tmpOfferOrder(price, quantity, OFFER);
        newOfferStack.push_back(tmpOfferOrder);
    }

    return OrderBook<T>(_product, newBidStack, newOfferStack);
}

template <typename T>
MarketDataConnector<T>::MarketDataConnector(MarketDataService<T>* _service) {
    service = _service;
}

template <typename T>
void MarketDataConnector<T>::Publish(OrderBook<T>& _data) {}

template <typename T> void MarketDataConnector<T>::Subscribe(ifstream& _data) {
    // ready to process data
    int bookDepth = service->GetOrderBookDepth();
    int _thread = bookDepth * 2;
    long orderCount = 0; // keep track of total orders added

    vector<Order> bidStack;
    vector<Order> offerStack;

    string line;
    while (getline(_data, line)) {

        stringstream lineStream(line);
        string tmp;
        vector<string> vecs;
        while (getline(lineStream, tmp, ',')) {
            vecs.push_back(tmp);
        }

        // process data
        auto _productId = vecs[0];
        double _price = string2price(vecs[1]);
        long _quantity = stol(vecs[2]);

        // convert string to SIDE
        // assume no ill-shaped inputs
        PricingSide side = vecs[3] == "BID" ? BID : OFFER;

        // generate order
        Order order(_price, _quantity, side);
        if (side == BID) {
            bidStack.push_back(order);
        } else {
            offerStack.push_back(order);
        }
        orderCount++;

        // This will trigger the OnMessage updates
        // since both BID and ASK offers have been processed
        if (orderCount % _thread == 0) {
            T _product = GetBond(_productId);
            OrderBook<T> tmpOrderBook(_product, bidStack, offerStack);
            service->OnMessage(tmpOrderBook);

            // reset, empty the stacks.
            bidStack = vector<Order>();
            offerStack = vector<Order>();
        }
    }
}

#endif
