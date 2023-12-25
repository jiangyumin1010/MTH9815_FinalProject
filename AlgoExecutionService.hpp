/**
 * AlgoExecutionService.hpp
 * Defines the data types and Service for algo executions
 *
 * @author Yumin Jiang
 */

#ifndef ALGO_EXECUTION_SERVICE_HPP
#define ALGO_EXECUTION_SERVICE_HPP

#include "execution.hpp"
#include "marketdataservice.hpp"
using namespace std;
/**
 * Definition of the algo execution class
 */
template <typename T> class AlgoExecution {
  public:
    // ctor
    AlgoExecution() = default;
    AlgoExecution(const T& _product, PricingSide _side, string _orderId,
                  OrderType _orderType, double _price, long _visibleQuantity,
                  long _hiddenQuantity, string _parentOrderId,
                  bool _isChildOrder);

    // Get the order
    ExecutionOrder<T>* GetExecutionOrder() const;

  private:
    ExecutionOrder<T>* executionOrder;
};

template <typename T>
AlgoExecution<T>::AlgoExecution(const T& _product, PricingSide _side,
                                string _orderId, OrderType _orderType,
                                double _price, long _visibleQuantity,
                                long _hiddenQuantity, string _parentOrderId,
                                bool _isChildOrder) {
    executionOrder = new ExecutionOrder<T>(
        _product, _side, _orderId, _orderType, _price, _visibleQuantity,
        _hiddenQuantity, _parentOrderId, _isChildOrder);
}

template <typename T>
ExecutionOrder<T>* AlgoExecution<T>::GetExecutionOrder() const {
    return executionOrder;
}

// Pre-declearations to avoid errors.
template <typename T> class AlgoExecutionServiceListener;

/**
 * Service for Algo Execution orders.
 * Keyed on product id.
 * Type T is the product type.
 */
template <typename T>
class AlgoExecutionService : public Service<string, AlgoExecution<T>> {
  public:
    // ctor
    AlgoExecutionService();
    ~AlgoExecutionService();

    // Get data on our service given a key
    AlgoExecution<T>& GetData(string _key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoExecution<T>& _data);

    // Add a listener to the Service for callbacks
    void AddListener(ServiceListener<AlgoExecution<T>>* _listener);

    // Get all listeners on the Service
    const vector<ServiceListener<AlgoExecution<T>>*>& GetListeners() const;

    // Get the ServiceListener on the BondMarketDataService
    AlgoExecutionServiceListener<T>* GetListener();

    // Execute algo based on an order on a market, called by listener when
    // adding process
    void AlgoExecutionTrade(OrderBook<T>& _orderBook);

  private:
    map<string, AlgoExecution<T>> algoExecutions;
    vector<ServiceListener<AlgoExecution<T>>*> listeners;
    AlgoExecutionServiceListener<T>* listener;
    long executionCount;
};

template <typename T> AlgoExecutionService<T>::AlgoExecutionService() {
    algoExecutions = map<string, AlgoExecution<T>>();
    listeners = vector<ServiceListener<AlgoExecution<T>>*>();
    listener = new AlgoExecutionServiceListener<T>(this);
    executionCount = 0;
}

template <typename T> AlgoExecutionService<T>::~AlgoExecutionService() {}

template <typename T>
AlgoExecution<T>& AlgoExecutionService<T>::GetData(string _id) {
    return algoExecutions[_id];
}

template <typename T>
void AlgoExecutionService<T>::OnMessage(AlgoExecution<T>& _data) {
    algoExecutions[_data.GetExecutionOrder()->GetProduct().GetProductId()] =
        _data;
}

template <typename T>
void AlgoExecutionService<T>::AddListener(
    ServiceListener<AlgoExecution<T>>* _listener) {
    listeners.push_back(_listener);
}

template <typename T>
const vector<ServiceListener<AlgoExecution<T>>*>&
AlgoExecutionService<T>::GetListeners() const {
    return listeners;
}

template <typename T>
AlgoExecutionServiceListener<T>* AlgoExecutionService<T>::GetListener() {
    return listener;
}

// Only trade when the spread is at 1/128th to reduce the cost of crossing the
// spread
template <typename T>
void AlgoExecutionService<T>::AlgoExecutionTrade(OrderBook<T>& _orderBook) {
    // T _product = _orderBook.GetProduct();
    string _productId = _orderBook.GetProduct().GetProductId();
    PricingSide _side;
    string _orderId = "AlgoExec" + to_string(executionCount);
    double _price;
    long _quantity;

    Order bid_order = _orderBook.GetBidOffer().GetBidOrder();
    Order offer_order = _orderBook.GetBidOffer().GetOfferOrder();

    double bid_price = bid_order.GetPrice();
    long bid_quantity = bid_order.GetQuantity();
    double offer_price = offer_order.GetPrice();
    long offer_quantity = offer_order.GetQuantity();

    // Only trade when the spread <= 1/128
    if (offer_price - bid_price <= 1.0 / 128.0) {
        if (executionCount % 2) {
            _price = bid_price;
            _quantity = bid_quantity;
            _side = BID;
        } else {
            _price = offer_price;
            _quantity = offer_quantity;
            _side = OFFER;
        }
        executionCount++;

        AlgoExecution<T> algoOrder(_orderBook.GetProduct(), _side, _orderId,
                                   MARKET, _price, _quantity, 0,
                                   "PARENT_ORDER_ID", false);
        algoExecutions[_productId] = algoOrder;

        // notify the listners
        for (auto& l : listeners) {
            l->ProcessAdd(algoOrder);
        }
    }
}

/**
 * Service listener connection algoexecution to BondMarketDataService
 */
template <typename T>
class AlgoExecutionServiceListener : public ServiceListener<OrderBook<T>> {
  public:
    // ctor
    AlgoExecutionServiceListener(AlgoExecutionService<T>* _service);

    // Listener callback to process an add event to the Service
    void ProcessAdd(OrderBook<T>& _data);

    // Listener callback to process a remove event to the Service
    void ProcessRemove(OrderBook<T>& _data);

    // Listener callback to process an update event to the Service
    void ProcessUpdate(OrderBook<T>& _data);

  private:
    AlgoExecutionService<T>* service;
};

template <typename T>
AlgoExecutionServiceListener<T>::AlgoExecutionServiceListener(
    AlgoExecutionService<T>* _service) {
    service = _service;
}

template <typename T>
void AlgoExecutionServiceListener<T>::ProcessAdd(OrderBook<T>& _data) {
    // Excute the trade
    service->AlgoExecutionTrade(_data);
}

// No implementation
template <typename T>
void AlgoExecutionServiceListener<T>::ProcessRemove(OrderBook<T>& _data) {}

template <typename T>
void AlgoExecutionServiceListener<T>::ProcessUpdate(OrderBook<T>& _data) {}

#endif
