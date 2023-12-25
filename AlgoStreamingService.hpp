/**
 * AlgoStreamingService.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef ALGO_STREAMING_SERVICE_HPP
#define ALGO_STREAMING_SERVICE_HPP

#include "streaming.hpp"
#include "pricingservice.hpp"

/**
* Definition of the algo stream class
*/
template<typename T>
class AlgoStream
{

public:
    // ctor
    AlgoStream() = default;
    AlgoStream(const T& _product, const PriceStreamOrder& _bidOrder, const PriceStreamOrder& _offerOrder);


    // Get the price stream
    PriceStream<T>* GetPriceStream() const;

private:
    PriceStream<T>* priceStream;
};

template<typename T>
AlgoStream<T>::AlgoStream(const T& _product, const PriceStreamOrder& _bidOrder, const PriceStreamOrder& _offerOrder)
{
    priceStream = new PriceStream<T>(_product, _bidOrder, _offerOrder);
}

template<typename T>
PriceStream<T>* AlgoStream<T>::GetPriceStream() const
{
    return priceStream;
}

// Pre-declearations to avoid errors.
template<typename T>
class AlgoStreamingServiceListener;


/**
* Service for Algo Streaming orders.
* Keyed on product id.
* Type T is the product type.
*/
template<typename T>
class AlgoStreamingService : public Service<string, AlgoStream<T>>
{
public:

    // Ctor
    AlgoStreamingService();
    ~AlgoStreamingService();

    // Get data on our service given a key
    AlgoStream<T>& GetData(string _key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoStream<T>& _data);

    // Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
    void AddListener(ServiceListener<AlgoStream<T>>* _listener);

    // Get all listeners on the Service
    const vector<ServiceListener<AlgoStream<T>>*>& GetListeners() const;

    // Get the listener of the service
    ServiceListener<Price<T>>* GetListener();

    // Publish two-way prices
    void AlgoPublishPrice(Price<T>& _price);

private:
    map<string, AlgoStream<T>> algoStreams;
    vector<ServiceListener<AlgoStream<T>>*> listeners;
    ServiceListener<Price<T>>* listener;
    long pricePublishCount;
};

template<typename T>
AlgoStreamingService<T>::AlgoStreamingService()
{
    algoStreams = map<string, AlgoStream<T>>();
    listeners = vector<ServiceListener<AlgoStream<T>>*>();
    listener = new AlgoStreamingServiceListener<T>(this);
    pricePublishCount = 0;
}

template<typename T>
AlgoStreamingService<T>::~AlgoStreamingService() {}

template<typename T>
AlgoStream<T>& AlgoStreamingService<T>::GetData(string _key)
{
    return algoStreams[_key];
}

template<typename T>
void AlgoStreamingService<T>::OnMessage(AlgoStream<T>& _data)
{
    algoStreams[_data.GetPriceStream()->GetProduct().GetProductId()] = _data;
}

template<typename T>
void AlgoStreamingService<T>::AddListener(ServiceListener<AlgoStream<T>>* _listener)
{
    listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<AlgoStream<T>>*>& AlgoStreamingService<T>::GetListeners() const
{
    return listeners;
}

template<typename T>
ServiceListener<Price<T>>* AlgoStreamingService<T>::GetListener()
{
    return listener;
}

template<typename T>
void AlgoStreamingService<T>::AlgoPublishPrice(Price<T>& _price)
{
//    T _product = _price.GetProduct();
//    string _productId = _product.GetProductId();
//
//    double midPrice = _price.GetMid();
//    double spread = _price.GetBidOfferSpread();
//    double _bidPrice = midPrice - spread / 2.0;
//    double _offerPrice = midPrice + spread / 2.0;
//    long _visibleQuantity = (pricePublishCount % 2 + 1) * 1000000;
//    long _hiddenQuantity = _visibleQuantity * 2;
//
//    pricePublishCount++;
//    PriceStreamOrder _bidOrder(_bidPrice, _visibleQuantity, _hiddenQuantity, BID);
//    PriceStreamOrder _offerOrder(_offerPrice, _visibleQuantity, _hiddenQuantity, OFFER);
//    AlgoStream<T> _algoStream(_product, _bidOrder, _offerOrder);
//    algoStreams[_productId] = _algoStream;
//
//    for (auto& l : listeners)
//    {
//        l->ProcessAdd(_algoStream);
//    }

}


/**
* Service Listener subscribing data on the BondPricingService
*/
template<typename T>
class AlgoStreamingServiceListener : public ServiceListener<Price<T>>
{

private:
    AlgoStreamingService<T>* service;

public:

    // ctor
    AlgoStreamingServiceListener(AlgoStreamingService<T>* _service);

    // Listener callback to process an add event to the Service
    void ProcessAdd(Price<T>& _data);

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Price<T>& _data);

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Price<T>& _data);

};

template<typename T>
AlgoStreamingServiceListener<T>::AlgoStreamingServiceListener(AlgoStreamingService<T>* _service)
{
    service = _service;
}

template<typename T>
void AlgoStreamingServiceListener<T>::ProcessAdd(Price<T>& _data)
{
    service->AlgoPublishPrice(_data);
}

template<typename T>
void AlgoStreamingServiceListener<T>::ProcessRemove(Price<T>& _data) {}

template<typename T>
void AlgoStreamingServiceListener<T>::ProcessUpdate(Price<T>& _data) {}


#endif
