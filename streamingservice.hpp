/**
 * streamingservice.hpp
 * Defines the Service and Listener for price streams.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef STREAMING_SERVICE_HPP
#define STREAMING_SERVICE_HPP

#include "soa.hpp"
#include "streaming.hpp"
#include "AlgoStreamingService.hpp"

// Pre-declearations to avoid errors.
template<typename T>
class StreamingServiceListener;

/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingService : public Service<string,PriceStream <T> >
{

public:

    // Ctor
    StreamingService();

    // Get data on our service given a key
    PriceStream<T>& GetData(string _key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PriceStream<T>& _data);

    // Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
    void AddListener(ServiceListener<PriceStream<T>>* _listener);

    // Get all listeners on the Service
    const vector<ServiceListener<PriceStream<T>>*>& GetListeners() const;

    // Get the listener of the service
    ServiceListener<AlgoStream<T>>* GetListener();

    // Publish two-way prices
    void PublishPrice(PriceStream<T>& _priceStream);

private:

    map<string, PriceStream<T>> priceStreams;
    vector<ServiceListener<PriceStream<T>>*> listeners;
    ServiceListener<AlgoStream<T>>* listener;
};

template<typename T>
StreamingService<T>::StreamingService()
{
    priceStreams = map<string, PriceStream<T>>();
    listeners = vector<ServiceListener<PriceStream<T>>*>();
    listener = new StreamingServiceListener<T>(this);
}

template<typename T>
PriceStream<T>& StreamingService<T>::GetData(string _key)
{
    return priceStreams[_key];
}

template<typename T>
void StreamingService<T>::OnMessage(PriceStream<T>& _data)
{
    priceStreams[_data.GetProduct().GetProductId()] = _data;
}

template<typename T>
void StreamingService<T>::AddListener(ServiceListener<PriceStream<T>>* _listener)
{
    listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<PriceStream<T>>*>& StreamingService<T>::GetListeners() const
{
    return listeners;
}

template<typename T>
ServiceListener<AlgoStream<T>>* StreamingService<T>::GetListener()
{
    return listener;
}

template<typename T>
void StreamingService<T>::PublishPrice(PriceStream<T>& _priceStream)
{
    for (auto& l : listeners)
    {
        l->ProcessAdd(_priceStream);
    }
}

/**
* Service Listener subscribing data from BondAlgoExecutionService
* Type T is the product type.
*/
template<typename T>
class StreamingServiceListener : public ServiceListener<AlgoStream<T>>
{
public:

    // Ctor
    StreamingServiceListener(StreamingService<T>* _service);
    ~StreamingServiceListener();

    // Listener callback to process an add event to the Service
    void ProcessAdd(AlgoStream<T>& _data);

    // Listener callback to process a remove event to the Service
    void ProcessRemove(AlgoStream<T>& _data);
    
    // Listener callback to process an update event to the Service
    void ProcessUpdate(AlgoStream<T>& _data);

private:

    StreamingService<T>* service;

};

template<typename T>
StreamingServiceListener<T>::StreamingServiceListener(StreamingService<T>* _service)
{
    service = _service;
}

template<typename T>
StreamingServiceListener<T>::~StreamingServiceListener() {}

template<typename T>
void StreamingServiceListener<T>::ProcessAdd(AlgoStream<T>& _data)
{
    PriceStream<T>* _priceStream = _data.GetPriceStream();
    service->OnMessage(*_priceStream);
    service->PublishPrice(*_priceStream);
}

template<typename T>
void StreamingServiceListener<T>::ProcessRemove(AlgoStream<T>& _data) {}

template<typename T>
void StreamingServiceListener<T>::ProcessUpdate(AlgoStream<T>& _data) {}


#endif