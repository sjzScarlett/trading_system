/**
 * executionservice.hpp
 * Defines the data types and Service for executions.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */

#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include "soa.h"
#include "marketdataservice.h"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{

public:

    //ctor
    ExecutionOrder(){};

    // ctor for an order
    ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

    // Get the product
    const T& GetProduct() const;

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

    // we add an operator << as overloading
    friend ostream& operator << (ostream& os, const ExecutionOrder& eo){
        os<<"PricingSide is: "<<(eo.side==BID ? "BID, " : "OFFER, ") ;
        os<<"OrderID is: "<<eo.GetOrderId()<<", ";

        //order type
        if(eo.orderType==FOK){
            os<<"OrderType is: "<<"FOK, ";
        }
        else if(eo.orderType==MARKET){
            os<<"OrderType is: "<<"MARKET, ";
        }
        else if(eo.orderType==LIMIT){
            os<<"OrderType is: "<<"LIMIT, ";
        }
        else if(eo.orderType==STOP){
            os<<"OrderType is: "<<"STOP, ";
        }
        else if(eo.orderType==IOC){
            os<<"OrderType is: "<<"IOC, ";
        }
        else{
            os<<"OTHERS!";
        }

        os<<"Price is: "<<eo.GetPrice()<<", ";
        os<<"VisibleQuantity is : "<<eo.GetVisibleQuantity()<<", ";
        os<<"HiddenQuantity is :"<<eo.GetHiddenQuantity()<<", ";
        os<<"ParentOrderId is :"<<eo.GetParentOrderId()<<", ";
        os<<"isChildOrder ? : "<<std::boolalpha<<eo.IsChildOrder()<<", ";

        return os;
    }

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

/**
 * Service for executing orders on an exchange.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class ExecutionService : public Service<string,ExecutionOrder <T> >
{

public:

    // Execute an order on a market
    virtual void ExecuteOrder(const ExecutionOrder<T>& order, Market market) = 0;

};



/*************************************************************************************/
//define member functions in class: ExecutionOrder
template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
        product(_product)
{
    side = _side;
    orderId = _orderId;
    orderType = _orderType;
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    parentOrderId = _parentOrderId;
    isChildOrder = _isChildOrder;
}



//define member functions in class: ExecutionOrder
template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
    return product;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
    return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
    return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
    return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
    return static_cast<long>(visibleQuantity);
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
    return static_cast<long>(hiddenQuantity);
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
    return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
    return isChildOrder;
}

#endif
