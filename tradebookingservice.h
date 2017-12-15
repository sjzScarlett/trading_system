/**
 * tradebookingservice.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "soa.h"
#include "products.h"

// Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade
{

public:

    //ctor
    Trade(){};

    // ctor for a trade
    Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side);

    // Get the product
    const T& GetProduct() const;

    // Get the trade ID
    const string& GetTradeId() const;

    // Get the mid price
    double GetPrice() const;

    // Get the book
    const string& GetBook() const;

    // Get the quantity
    long GetQuantity() const;

    // Get the side
    Side GetSide() const;

    // we add an operator << as overloading
    friend ostream& operator << (ostream& os, const Trade<T>& trade){
        os<<"TradeId is: "<<trade.GetTradeId()<<std::endl; //get trade id
        os<<"Product is: "<<trade.GetProduct()<<std::endl; //get product
        os<<"Price is: "<<trade.GetPrice()<<std::endl; //get price
        os<<"Book is: "<<trade.GetBook()<<std::endl; //get book
        os<<"Quantity is: "<<trade.GetQuantity()<<std::endl; //get quantity

        //get side
        if(trade.GetSide() == BUY){
            os<<"Side is: BUY"<<std::endl;
        }
        else{
            os<<"Side is: SELL"<<std::endl;
        }
        return os;
    }

private:
    T product;
    string tradeId;
    double price;
    string book;
    long quantity;
    Side side;

};


/**
 * Trade Booking Service to book trades to a particular book.
 * As we consider only bond, we use Bond instead of using template.
 * Keyed on product identifier.
 */

class TradeBookingService : public Service<string,Trade <Bond> >
{

private:
    //define listener
    std::vector<ServiceListener<Trade<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Trade<Bond>> trade_data;

    //ctor
    TradeBookingService(){};

public:

    // Generate instance
    static TradeBookingService* Generate_Instance(){
        //we define this function to generate instance for class TradeBookingService.
        static TradeBookingService ins;
        return &ins;
    }

    // Book the trade
    void BookTrade(Trade<Bond> &trade);

    // pure virtual member functions in class Service.
    // Get data on our service given a key
    Trade<Bond>& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Trade<Bond> &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Trade<Bond>> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<Trade<Bond>>* >& GetListeners() const ;
};


/**
 * Connector is used to read the data in the .txt file into service
 * Keyed on product identifier.
 * Using Bond as our type.
 */

//The TradeBookingConnector is subscriber-only
class TradeBookingConnector: public Connector<Trade<Bond>>{

private:
    //create 2 pointers one for TradeBookingService, the other for BondProductService
    TradeBookingService* trade_book_service;
    BondProductService* bond_product_service;

    // ctor
    TradeBookingConnector(){
        trade_book_service=TradeBookingService::Generate_Instance();
        bond_product_service=BondProductService::Generate_Instance();
    }

public:

    // Generate Instance
    static TradeBookingConnector* Generate_Instance(){
        static TradeBookingConnector ins;
        return &ins;
    }

    //  member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(Trade<Bond>& data) ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();

    // GetService
    TradeBookingService* GetService();

};

//define constructor and member functions in class: Trade
template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side) :
        product(_product)
{
    tradeId = _tradeId;
    price = _price;
    book = _book;
    quantity = _quantity;
    side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
    return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
    return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const
{
    return price;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
    return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
    return side;
}


//define member functions in class: TradeBookingService
void TradeBookingService::BookTrade(Trade<Bond> &trade)
{
    //BookTrade is used for passing the trade data to the listener.
    std::cout<<"data goes from TradeBookingService -> listener."<<std::endl;
    for(auto& l:listeners){
        l->ProcessAdd(trade);
    }
}

Trade<Bond>& TradeBookingService::GetData(std::string key)
{
    //get data given a key
    return trade_data.at(key);
}

void TradeBookingService::OnMessage(Trade<Bond> &data) 
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    trade_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    BookTrade(data);
}

void TradeBookingService::AddListener(ServiceListener<Trade<Bond>> *listener) 
{
    //add a listener to the service
    listeners.push_back(listener);
}

const vector<ServiceListener<Trade<Bond>>* >& TradeBookingService::GetListeners() const 
{
    //get all listeners on the service
    return listeners;
}


//define member functions in TradeBookingConnector
void TradeBookingConnector::Publish(Trade<Bond> &data) 
{
    //no implementation inside of this function
}

void TradeBookingConnector::Subscribe()
{
    //define some strings we will use later
    std::string key, tradeid, book, price, quantity, side;

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
    auto PriceTranspose = [](std::string& s){
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
        value+=(temp1* 8 + temp2) /256.;
        return value;
    };

    //read the file and do subscribing
    ifstream iss("../input/trades.txt");
    std::string line;
    getline(iss,line);

    //if the file is not empty
    while(getline(iss, line)){

        //create a vector to contain the parts in each line
        std::vector<std::string> container=splitoneline(line);

        //using the string defined above as the name of different parts
        key=container[0];
        tradeid=container[1];
        book=container[2];
        price=container[3];
        quantity=container[4];
        side=container[5];

        //define trade
        //find the bond in order to define trade
        auto bond=bond_product_service->GetData(key);
        Trade<Bond> trade(bond, tradeid, PriceTranspose(price), book, std::stol(quantity), (side=="BUY" ? BUY :SELL));

        //using OnMessage to pass the data to TradeBookingService
        trade_book_service->OnMessage(trade);
    }

    std::cout<<"input/trade.txt -> TradeBookingService DONE!"<<std::endl;
}

TradeBookingService* TradeBookingConnector::GetService()
{
    return trade_book_service;
}

#endif