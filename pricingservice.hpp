/**
 * pricingservice.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#define _CRT_SECURE_NO_WARNINGS 1

#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include "utility.hpp"
#include <string>

/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template <typename T> class Price {

  public:
    // ctor
    Price() = default;
    Price(const T& _product, double _mid, double _bidOfferSpread);

    // Get the product
    const T& GetProduct() const;

    // Get the mid price
    double GetMid() const;

    // Get the bid/offer spread around the mid
    double GetBidOfferSpread() const;

    // Change attributes to strings
    vector<string> ToStrings() const;

    vector<string> PrintFunction() const;

  private:
    T product;
    double mid;
    double bidOfferSpread;
};

template <typename T>
Price<T>::Price(const T& _product, double _mid, double _bidOfferSpread)
    : product(_product) {
    mid = _mid;
    bidOfferSpread = _bidOfferSpread;
}

template <typename T> const T& Price<T>::GetProduct() const { return product; }

template <typename T> double Price<T>::GetMid() const { return mid; }

template <typename T> double Price<T>::GetBidOfferSpread() const {
    return bidOfferSpread;
}

template <typename T> vector<string> Price<T>::PrintFunction() const {
    string _product = product.GetProductId();
    string _mid = price2string(mid);
    string _bidOfferSpread = price2string(bidOfferSpread);

    vector<string> _strings;
    _strings.push_back(_product);
    _strings.push_back(_mid);
    _strings.push_back(_bidOfferSpread);
    return _strings;
}

// Pre-declearations to avoid errors.
template <typename T> class PricingConnector;

/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template <typename T> class PricingService : public Service<string, Price<T>> {
  public:
    PricingService();
    ~PricingService();

    // Get data on our service given product id
    Price<T>& GetData(string _key);

    // Call back function that a Connector should invoke for any new or updated
    // data
    void OnMessage(Price<T>& _data);

    // Add listener to the service
    void AddListener(ServiceListener<Price<T>>* listener);

    // Get all the listeners on the service
    const vector<ServiceListener<Price<T>>*>& GetListeners() const;

    // Get the connector
    PricingConnector<T>* GetConnector();

  private:
    std::map<string, Price<T>> prices;
    vector<ServiceListener<Price<T>>*> listeners;
    PricingConnector<T>* connector;
};

template <typename T> class PricingConnector : public Connector<Price<T>> {
  public:
    // Ctor
    PricingConnector(PricingService<T>* _service);

    // Publish data to the Connector
    void Publish(Price<T>& _data);

    // Subscribe data from the Connector
    void Subscribe(ifstream& _data);

  private:
    PricingService<T>* service;
};

template <typename T>
PricingService<T>::PricingService() : prices(), listeners(), connector() {
    prices = map<string, Price<T>>();
    listeners = vector<ServiceListener<Price<T>>*>();
    connector = new PricingConnector<T>(this);
}

template <typename T> PricingService<T>::~PricingService() {}

template <typename T> Price<T>& PricingService<T>::GetData(string _key) {
    return prices[_key];
}

template <typename T> void PricingService<T>::OnMessage(Price<T>& _data) {
    string id = _data.GetProduct().GetProductId();
    // update the price map
    if (prices.find(id) != prices.end()) {
        prices.erase(id);
    }
    prices.insert(pair<string, Price<Bond>>(id, _data));

    // flow the data to listeners
    for (auto& listener : listeners) {
        listener->ProcessAdd(_data);
    }
}

template <typename T>
void PricingService<T>::AddListener(ServiceListener<Price<T>>* _listener) {
    listeners.push_back(_listener);
}

template <typename T>
const vector<ServiceListener<Price<T>>*>&
PricingService<T>::GetListeners() const {
    return listeners;
}

template <typename T> PricingConnector<T>* PricingService<T>::GetConnector() {
    return connector;
}

template <typename T>
PricingConnector<T>::PricingConnector(PricingService<T>* _service) {
    service = _service;
}

template <typename T> void PricingConnector<T>::Publish(Price<T>& _data) {}

// Read from "price.txt" and process the data
template <typename T> void PricingConnector<T>::Subscribe(ifstream& _data) {

    string line;
    while (getline(_data, line)) {

        stringstream lineStream(line);
        string tmp;
        vector<string> vecs;
        while (getline(lineStream, tmp, ',')) {
            vecs.push_back(tmp);
        }

        // read and split the corresponding data features
        string _productId = vecs[0];
        T _product = GetBond(_productId);
        double bid_price = string2price(vecs[1]);
        double offer_price = string2price(vecs[2]);
        double mid_price = (bid_price + offer_price) / 2.0;
        double spread = offer_price - bid_price;

        Price<T> _price(_product, mid_price, spread);

        service->OnMessage(_price);
    }
}

#endif
