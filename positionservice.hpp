/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include "tradebookingservice.hpp"
#include <map>
#include <string>

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template <typename T> class Position {

  public:
    // ctor for a position
    Position() = default;
    Position(const T& _product);

    // Get the product
    const T& GetProduct() const;

    // Get the position quantity
    long GetPosition(string& book);

    // Get the positions books
    map<string, long> GetPositions();

    // Add position quantity to a book
    void AddPosition(string& _book, long _position);

    // Get the aggregate position
    long GetAggregatePosition();

    // Write to file
    vector<string> PrintFunction() const;

  private:
    T product;
    map<string, long> positions;
};

template <typename T>
Position<T>::Position(const T& _product) : product(_product) {}

template <typename T> const T& Position<T>::GetProduct() const {
    return product;
}

template <typename T> long Position<T>::GetPosition(string& book) {
    return positions[book];
}

template <typename T> map<string, long> Position<T>::GetPositions() {
    return positions;
}

template <typename T>
void Position<T>::AddPosition(string& _book, long _position) {
    positions[_book] += _position;
}

template <typename T> long Position<T>::GetAggregatePosition() {
    long aggregatePosition = 0;
    for (auto& p : positions) {
        aggregatePosition += p.second;
    }
    return aggregatePosition;
}

template <typename T> vector<string> Position<T>::PrintFunction() const {
    string _product = product.GetProductId();
    vector<string> _positions;

    // storing the market and corresponding positions
    for (auto& p : positions) {
        string _book = p.first;
        string _position = to_string(p.second);
        _positions.push_back(_book);
        _positions.push_back(_position);
    }

    vector<string> _strings;
    _strings.push_back(_product);
    _strings.insert(_strings.end(), _positions.begin(), _positions.end());
    return _strings;
}

// Pre-declearations
template <typename T> class PositionServiceListener;

// -------------------- TradeBookingService --------------------------

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template <typename T>
class PositionService : public Service<string, Position<T>> {

  public:
    // Ctor
    PositionService();

    // Get data on our service on the given id
    Position<T>& GetData(string _key);

    // Call back function that a Connector should invoke for any new or updated
    // data
    void OnMessage(Position<T>& _data);

    // Add a listener to the Service
    void AddListener(ServiceListener<Position<T>>* _listener);

    // Get all listeners on the Service
    const vector<ServiceListener<Position<T>>*>& GetListeners() const;

    // Get the listener of the service
    PositionServiceListener<T>* GetListener();

    // Add a trade to the service
    virtual void AddTrade(const Trade<T>& _trade);

  private:
    map<string, Position<T>> positions;
    vector<ServiceListener<Position<T>>*> listeners;
    PositionServiceListener<T>* listener;
};

template <typename T> PositionService<T>::PositionService() {
    positions = map<string, Position<T>>();
    listeners = vector<ServiceListener<Position<T>>*>();
    listener = new PositionServiceListener<T>(this);
}

template <typename T> Position<T>& PositionService<T>::GetData(string _key) {
    return positions[_key];
}

template <typename T> void PositionService<T>::OnMessage(Position<T>& _data) {
    string _id = _data.GetProduct().GetProductId();
    positions[_id] = _data;
}

template <typename T>
void PositionService<T>::AddListener(ServiceListener<Position<T>>* _listener) {
    listeners.push_back(_listener);
}

template <typename T>
PositionServiceListener<T>* PositionService<T>::GetListener() {
    return listener;
}

template <typename T>
const vector<ServiceListener<Position<T>>*>&
PositionService<T>::GetListeners() const {
    return listeners;
}

// Add a trade to the system
template <typename T>
void PositionService<T>::AddTrade(const Trade<T>& _trade) {
    T _product = _trade.GetProduct();
    string _productId = _product.GetProductId();
    string _book = _trade.GetBook();
    long _quantity = _trade.GetQuantity();
    Side _side = _trade.GetSide();

    Position<T> _positionTo(_product);
    if (_side == BUY) {
        _positionTo.AddPosition(_book, _quantity);
    } else if (_side == SELL) {
        _positionTo.AddPosition(_book, -_quantity);
    }

    Position<T> _positionFrom = positions[_productId];
    map<string, long> _positionMap = _positionFrom.GetPositions();
    for (auto& p : _positionMap) {
        _book = p.first;
        _quantity = p.second;
        _positionTo.AddPosition(_book, _quantity);
    }
    positions[_productId] = _positionTo;

    // flow to the listeners
    for (auto& l : listeners) {
        l->ProcessAdd(_positionTo);
    }
}

// -------------------- PositionServiceListener --------------------------

/**
 * Service Listener
 * from BondTradeBookingService  to BondPositionService .
 * Type T is the product type.
 */
template <typename T>
class PositionServiceListener : public ServiceListener<Trade<T>> {

  private:
    PositionService<T>* service;

  public:
    // ctor
    PositionServiceListener(PositionService<T>* _service);

    // Listener callback to process an add event to the Service
    void ProcessAdd(Trade<T>& _data);

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Trade<T>& _data);

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Trade<T>& _data);
};

template <typename T>
PositionServiceListener<T>::PositionServiceListener(
    PositionService<T>* _service) {
    service = _service;
}

template <typename T>
void PositionServiceListener<T>::ProcessAdd(Trade<T>& _data) {
    service->AddTrade(_data);
}

template <typename T>
void PositionServiceListener<T>::ProcessRemove(Trade<T>& _data) {}

template <typename T>
void PositionServiceListener<T>::ProcessUpdate(Trade<T>& _data) {}

#endif
