 /**
 * execution.hpp
 * Defines the data type for executions.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef EXECUTION_HPP
#define EXECUTION_HPP

#include <string>
// #include "soa.hpp"
#include "marketdataservice.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template <typename T> class ExecutionOrder {

  public:
    // ctor for an order
    ExecutionOrder() = default;
    ExecutionOrder(const T& _product, PricingSide _side, string _orderId,
                   OrderType _orderType, double _price, double _visibleQuantity,
                   double _hiddenQuantity, string _parentOrderId,
                   bool _isChildOrder);

    // Get the product
    const T& GetProduct() const;

    // Get the pricing side
    PricingSide GetPricingSide() const;

    // Get the order ID
    const string& GetOrderId() const;

    // Get the order type on this order
    OrderType GetOrderType() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity
    long GetHiddenQuantity() const;

    // Get the parent order ID
    const string& GetParentOrderId() const;

    // Is child order?
    bool IsChildOrder() const;

    vector<string> PrintFunction() const;

  private:
    T product;
    PricingSide side;
    string orderId;
    OrderType orderType;
    double price;
    double visibleQuantity;
    double hiddenQuantity;
    string parentOrderId;
    bool isChildOrder;
};

template <typename T>
ExecutionOrder<T>::ExecutionOrder(const T& _product, PricingSide _side,
                                  string _orderId, OrderType _orderType,
                                  double _price, double _visibleQuantity,
                                  double _hiddenQuantity, string _parentOrderId,
                                  bool _isChildOrder)
    : product(_product) {
    side = _side;
    orderId = _orderId;
    orderType = _orderType;
    price = _price;
    visibleQuantity = static_cast<long>(_visibleQuantity);
    hiddenQuantity = static_cast<long>(_hiddenQuantity);
    parentOrderId = _parentOrderId;
    isChildOrder = _isChildOrder;
}

template <typename T> const T& ExecutionOrder<T>::GetProduct() const {
    return product;
}

template <typename T> PricingSide ExecutionOrder<T>::GetPricingSide() const {
    return side;
}

template <typename T> const string& ExecutionOrder<T>::GetOrderId() const {
    return orderId;
}

template <typename T> OrderType ExecutionOrder<T>::GetOrderType() const {
    return orderType;
}

template <typename T> double ExecutionOrder<T>::GetPrice() const {
    return price;
}

template <typename T> long ExecutionOrder<T>::GetVisibleQuantity() const {
    return visibleQuantity;
}

template <typename T> long ExecutionOrder<T>::GetHiddenQuantity() const {
    return hiddenQuantity;
}

template <typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const {
    return parentOrderId;
}

template <typename T> bool ExecutionOrder<T>::IsChildOrder() const {
    return isChildOrder;
}

template <typename T> vector<string> ExecutionOrder<T>::PrintFunction() const {
    string _product = product.GetProductId();
    string _side;
    _side = side == BID ? "BID" : "OFFER";
    string _orderId = orderId;
    string _orderType;
    if (orderType == FOK) {
        _orderType = "FOK";
    }
    if (orderType == IOC) {
        _orderType = "IOC";
    }
    if (orderType == MARKET) {
        _orderType = "MARKET";
    }
    if (orderType == LIMIT) {
        _orderType = "LIMIT";
    }
    if (orderType == STOP) {
        _orderType = "STOP";
    }

    string _price = price2string(price);
    string _visibleQuantity = to_string(visibleQuantity);
    _visibleQuantity =
        _visibleQuantity.substr(0, _visibleQuantity.find(".") + 1);
    string _hiddenQuantity = to_string(hiddenQuantity);
    _hiddenQuantity = _hiddenQuantity.substr(0, _hiddenQuantity.find(".") + 1);
    string _parentOrderId = parentOrderId;
    string _isChildOrder = isChildOrder ? "YES" : "NO";

    vector<string> _strings{_product,        _side,          _orderId,
                            _orderType,      _price,         _visibleQuantity,
                            _hiddenQuantity, _parentOrderId, _isChildOrder};
    return _strings;
}

#endif
