/**
 * marketdataservice.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef MARKET_DATA_SERVICE_HPP
#define MARKET_DATA_SERVICE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "soa.h"
#include "products.h"

using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };

/**
 * A market data order with price, quantity, and side.
 */
class Order
{

public:

    //ctor
    Order(){};

    // ctor for an order
    Order(double _price, long _quantity, PricingSide _side);

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
class BidOffer
{

public:

    // ctor for bid/offer
    BidOffer(const Order &_bidOrder, const Order &_offerOrder);

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

template<typename T>
class OrderBook
{

public:

    //ctor
    OrderBook(){};

    // ctor for the order book
    OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack);

    // Get the product
    const T& GetProduct() const;

    // Get the bid stack
    const vector<Order>& GetBidStack() const;

    // Get the offer stack
    const vector<Order>& GetOfferStack() const;

    // we add an operator << as overloading
    friend ostream& operator << (ostream& os, const OrderBook<T>& od){
        os<<"Bid prices are: "<<'\t';
        for(auto& b: od.bidStack){
            os<<b.GetPrice()<<", ";
        }
        os<<std::endl;

        os<<"Asd prices are: "<<'\t';
        for(auto& a: od.offerStack){
            os<<a.GetPrice()<<", ";
        }
        os<<std::endl;
        return os;
    }

private:
    T product;
    vector<Order> bidStack;
    vector<Order> offerStack;

};


/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * We use type Bond instead of using template T
 */

class MarketDataService : public Service<string,OrderBook <Bond> >
{

private:

    //define listener
    std::vector<ServiceListener<OrderBook<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, OrderBook<Bond>> market_data;

    //ctor
    MarketDataService(){};

public:

    // Generate instance
    static MarketDataService* Generate_Instance(){
        //we define this function to generate instance for class MarketDataService.
        static MarketDataService ins;
        return &ins;
    }

    // pure virtual member functions in class Service.
    // Get data on our service given a key
    OrderBook<Bond>& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(OrderBook<Bond> &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<OrderBook<Bond>> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<OrderBook<Bond>>* >& GetListeners() const ;

    // Get the best bid/offer order
    const BidOffer& GetBestBidOffer(const string &productId);

    // Aggregate the order book
    const OrderBook<Bond>& AggregateDepth(const string &productId);

};

/**
 * Market Data Service Connector is used to read and deal with data in .txt file
 * Keyed on product identifier.
 * We use type Bond instead of using template T
 */

class MarketDataConnector : public Connector<OrderBook<Bond>>{

private:

    //create 2 pointers one for MarketDataService, the other for BondProductService
    MarketDataService* market_data_service;
    BondProductService* bond_product_service;

public:

    // ctor
    MarketDataConnector(){
        market_data_service=MarketDataService::Generate_Instance();
        bond_product_service=BondProductService::Generate_Instance();
    }

    // Generate Instance
    static MarketDataConnector* Generate_Instance(){
        static MarketDataConnector ins;
        return &ins;
    }

    //  member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(OrderBook<Bond>& data) ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();

    // GetService
    MarketDataService* GetService();
};



//define member functions in class: Order
Order::Order(double _price, long _quantity, PricingSide _side)
{
    price = _price;
    quantity = _quantity;
    side = _side;
}

double Order::GetPrice() const
{
    return price;
}

long Order::GetQuantity() const
{
    return quantity;
}

PricingSide Order::GetSide() const
{
    return side;
}


//define member functions in class: BidOffer
BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
        bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
    return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
    return offerOrder;
}


//define member fuctions in class: OrderBook
template<typename T>
OrderBook<T>::OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack) :
        product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template<typename T>
const T& OrderBook<T>::GetProduct() const
{
    return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const
{
    return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const
{
    return offerStack;
}


//define member functions in class: MarketDataService
OrderBook<Bond>& MarketDataService::GetData(std::string key) 
{
    return market_data.at(key);
}

void MarketDataService::OnMessage(OrderBook<Bond> &data) 
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    market_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    std::cout<<"data goes from MarketDataService -> listener."<<std::endl;
    for(auto& l:listeners){
        l->ProcessAdd(data);
    }
}

void MarketDataService::AddListener(ServiceListener<OrderBook<Bond>> *listener) 
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<OrderBook<Bond>>* >& MarketDataService::GetListeners() const 
{
    return listeners;
}

const BidOffer& MarketDataService::GetBestBidOffer(const string &productId)
{
    int index=0;
    auto& md=market_data.at(productId);

    //using loop to find the bid and offer with minimum spread: 1/128
    for(int i=0;i<md.GetBidStack().size();i++){
        if(md.GetOfferStack()[i].GetPrice()-md.GetBidStack()[i].GetPrice()==1/128.){
            index=i;
        }
    }

    const BidOffer& s = BidOffer(md.GetBidStack()[index], md.GetOfferStack()[index]);
    return s;
}

const OrderBook<Bond>& MarketDataService::AggregateDepth(const string &productId)
{
    auto& md=market_data.at(productId);
    const OrderBook<Bond> & s = OrderBook<Bond>(md.GetProduct(),md.GetBidStack(),md.GetOfferStack());
    return s;
}



//define member functions for class: MarketDataConnector
void MarketDataConnector::Publish(OrderBook<Bond>& data) 
{
    // no implemetation here
}

//********************************************************
void MarketDataConnector::Subscribe() {
    //define some strings we will use later
    std::string key, price, quantity;
    std::vector<Order> bid_container, offer_container;

    //lambda function working to split one line and return vector of parts
    auto splitoneline = [](std::string line){
        stringstream ss(line);
        std::string part; //define each part in the line
        std::vector<std::string> container; //define container to contain each part and return it

        while(getline(ss,part,',')){
            container.push_back(part); //using comma as our separate signal
        }

        return container;
    };

    //lambda function working to transposing price of the bond
    auto PriceTranspose = [](std::string s){
        size_t pos=s.find_first_of('-'); //position of the '-'

        //we use std::stoi to transpose string to double
        //firstly, calculate the value of the first part before pos '-'
        double value=std::stoi(s.substr(0,pos));

        //then, calculate the first value after '-', 2 char in the string
        int temp1=std::stoi(s.substr(pos+1,2));

        //at the same time, calculate the last value in s
        auto last=s[s.size()-1];
        if(last=='+'){
            last='4';
        }
        int temp2=last-'0';

        //finally, add them up and get the final value
        double final_value=value+(temp1/32.+temp2/256.);
        return final_value;
    };

    //read the file and do subscribing
    ifstream iss("../input/marketdata.txt");
    std::string line;
    getline(iss,line);

    for(int i=0;i<60;i++){
        getline(iss, line);
        //create a vector to contain the parts in each line
        std::vector<std::string> container=splitoneline(line);

        //using the string defined above as the name of different parts
        key=container[0];
        int index=1; //define an index in order to get position of quantity and bid/offer price

        //get price and quantity and define order
        //Bid
        for(int i=1;i<=5;++i){
            //get price
            price=container[index++];

            //get quantity
            quantity=container[index++];

            //define order
            Order o_bid(PriceTranspose(price),std::stol(quantity),BID);

            //push all bids in the bid_container
            bid_container.push_back(o_bid);
        }

        //Offer
        for(int i=1;i<=5;++i){
            //get price
            price=container[index++];

            //get quantity
            quantity=container[index++];

            //define order
            Order o_offer(PriceTranspose(price),std::stol(quantity),OFFER);
            //push all bids in the bid_container
            offer_container.push_back(o_offer);
        }

        //define OrderBook
        //find the bond in order to define OrderBook
        auto bond=bond_product_service->GetData(key);

        OrderBook<Bond> orderbook(bond, bid_container, offer_container);

        //using OnMessage to pass the data to MarketDataService
        market_data_service->OnMessage(orderbook);
    }

    std::cout<<"input/marketdata.txt -> MarketDataService DONE!"<<std::endl;
}
//****************************************************************************

// GetService
MarketDataService* MarketDataConnector::GetService()
{
    return market_data_service;
}
#endif