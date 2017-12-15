//NOTE: I put Algostreaming and streaming in the same .h since streaming need to use the member function in algostreaming

/***********************Here below are file for algostreamingservice.h**************************/
/**
 * streamingservice.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef STREAMING_SERVICE_HPP
#define STREAMING_SERVICE_HPP

#include <iostream>
#include "soa.h"
#include "marketdataservice.h"
#include "pricingservice.h"

/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder
{

public:

    //ctor
    PriceStreamOrder(){};

    // ctor for an order
    PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side);

    // The side on this order
    PricingSide GetSide() const;

    // Get the price on this order
    double GetPrice() const;

    // operator overloadding
    friend ostream& operator << (ostream& os, const PriceStreamOrder& t) {
        os << "  Price: " << t.GetPrice() << endl;
        os << "  VisibleQuantity: " << t.GetVisibleQuantity() << endl;
        os << "  HiddenQuantity: " << t.GetHiddenQuantity() << endl;
        os << "  Side: " << (t.GetSide() == BID ? "BID" : "ASK") << endl;
        return os;
    }

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity on this order
    long GetHiddenQuantity() const;

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
template<typename T>
class PriceStream
{

public:

    //ctor
    PriceStream(){};

    // ctor
    PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);

    // Get the product
    const T& GetProduct() const;

    // Get the bid order
    const PriceStreamOrder& GetBidOrder() const;

    // Get the offer order
    const PriceStreamOrder& GetOfferOrder() const;

private:
    T product;
    PriceStreamOrder bidOrder;
    PriceStreamOrder offerOrder;

};





/*************************************************************************************/
//define member functions in class: PriceStreamOrder
PriceStreamOrder::PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side)
{
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    side = _side;
}

double PriceStreamOrder::GetPrice() const
{
    return price;
}

long PriceStreamOrder::GetVisibleQuantity() const
{
    return visibleQuantity;
}

long PriceStreamOrder::GetHiddenQuantity() const
{
    return hiddenQuantity;
}



//define member functions in class: PriceStream
template<typename T>
PriceStream<T>::PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder) :
        product(_product), bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

template<typename T>
const T& PriceStream<T>::GetProduct() const
{
    return product;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const
{
    return bidOrder;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const
{
    return offerOrder;
}

#endif