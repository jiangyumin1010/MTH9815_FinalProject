/**
 * streaming.hpp
 * Defines the data types price streams.
 *
 * @author Breman Thuraisingham
 * @coauthor Yumin Jiang
 */
#ifndef STREAMING_HPP
#define STREAMING_HPP

#include "marketdataservice.hpp"

/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder {

  public:
    // ctor for an order
    PriceStreamOrder() = default;
    PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity,
                     PricingSide _side);

    // The side on this order
    PricingSide GetSide() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity on this order
    long GetHiddenQuantity() const;

    vector<string> PrintFunction() const;

  private:
    double price;
    long visibleQuantity;
    long hiddenQuantity;
    PricingSide side;
};

/**
 * Price Stream with a two-way market.
 * Type T is the product type.
 */
template <typename T> class PriceStream {

  public:
    // ctor
    PriceStream() = default;
    PriceStream(const T& _product, const PriceStreamOrder& _bidOrder,
                const PriceStreamOrder& _offerOrder);

    // Get the product
    const T& GetProduct() const;

    // Get the bid order
    const PriceStreamOrder& GetBidOrder() const;

    // Get the offer order
    const PriceStreamOrder& GetOfferOrder() const;

    vector<string> PrintFunction() const;

  private:
    T product;
    PriceStreamOrder bidOrder;
    PriceStreamOrder offerOrder;
};

PriceStreamOrder::PriceStreamOrder(double _price, long _visibleQuantity,
                                   long _hiddenQuantity, PricingSide _side) {
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    side = _side;
}

double PriceStreamOrder::GetPrice() const { return price; }

long PriceStreamOrder::GetVisibleQuantity() const { return visibleQuantity; }

long PriceStreamOrder::GetHiddenQuantity() const { return hiddenQuantity; }

vector<string> PriceStreamOrder::PrintFunction() const {
    string _price = price2string(price);
    string _visibleQuantity = to_string(visibleQuantity);
    string _hiddenQuantity = to_string(hiddenQuantity);
    string _side = side == BID ? "BID" : "OFFER";
    vector<string> _strings{_price, _visibleQuantity, _hiddenQuantity, _side};
    return _strings;
}

template <typename T>
PriceStream<T>::PriceStream(const T& _product,
                            const PriceStreamOrder& _bidOrder,
                            const PriceStreamOrder& _offerOrder)
    : product(_product), bidOrder(_bidOrder), offerOrder(_offerOrder) {}

template <typename T> const T& PriceStream<T>::GetProduct() const {
    return product;
}

template <typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const {
    return bidOrder;
}

template <typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const {
    return offerOrder;
}

template <typename T> vector<string> PriceStream<T>::PrintFunction() const {
    string _product = product.GetProductId();
    vector<string> _bidOrder = bidOrder.PrintFunction();
    vector<string> _offerOrder = offerOrder.PrintFunction();

    // concat the bid and offer information
    vector<string> priceStreamString;
    priceStreamString.push_back(_product);
    priceStreamString.insert(priceStreamString.end(), _bidOrder.begin(),
                             _bidOrder.end());
    priceStreamString.insert(priceStreamString.end(), _offerOrder.begin(),
                             _offerOrder.end());
    return priceStreamString;
}

#endif
