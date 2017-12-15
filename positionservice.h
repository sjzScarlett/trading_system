/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <iostream>
#include <string>
#include <map>
#include "soa.h"
#include "tradebookingservice.h"

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:

    //ctor
    Position(){};

    // ctor for a position
    Position(const T &_product);

    // Get the product
    const T& GetProduct() const;

    // Get the position quantity
    long GetPosition(string &book);

    // Get the aggregate position
    long GetAggregatePosition();

    // AddQuantity
    // This member function is used in class: PositionServiceListener
    void AddQuantity(std::string book, long quantity);

private:
    T product;
    map<string,long> positions;

};

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * We use the specific type: Bond instead of using template T.
 */

class PositionService : public Service<string,Position <Bond> >
{

private:

    //define listener
    std::vector<ServiceListener<Position<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Position<Bond>> position_data;

    //ctor
    PositionService(){};

public:

    // Generate instance
    static PositionService* Generate_Instance(){
        //we define this function to generate instance for class PositionService.
        static PositionService ins;
        return &ins;
    }

    //Add position
    void Addpos(Position<Bond>& ps);

    // Add a trade to the service
    void AddTrade(const Trade<Bond> &trade);

    // pure virtual member functions in class Service.
    // Get data on our service given a key
    Position<Bond>& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Position<Bond> &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Position<Bond>> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<Position<Bond>>* >& GetListeners() const ;
};


/**
 * Position Service Listener callbacks to process add, remove and update events to the service
 * We use the specific type: Bond instead of using template T.
 */


//The BondTradeBookingService should be linked to a BondPositionService via a ServiceListener.
class PositionServiceListener : public ServiceListener<Trade<Bond>>{

private:

    //define a pointer of PositionService
    PositionService * position_service;

    //ctor
    PositionServiceListener(){
        position_service=PositionService::Generate_Instance();
    }

public:

    // Generate instance
    static PositionServiceListener* Generate_Instance(){
        //we define this function to generate instance for class PositionServiceListener.
        static PositionServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    //Link TradeBookingService and PositionService through Service Listener
    // Listener callback to process an add event to the Service
    void ProcessAdd(Trade<Bond> &data) ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Trade<Bond> &data) ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Trade<Bond> &data) ;

    // return position service
    PositionService* GetService();
};


//define member functions in class: Position
template<typename T>
Position<T>::Position(const T &_product) :
        product(_product)
{
    //initialize positions
    //since all positions are covered by books: TRSY1, TRSY2, TRSY3, we can use loop to set up initial value
    std::string s="TRSY";
    for(int i=1;i<=3;i++){
        positions[s+std::to_string(i)]=0;
    }
}

template<typename T>
const T& Position<T>::GetProduct() const
{
    return product;
}

template<typename T>
long Position<T>::GetPosition(string &book)
{
    return positions[book];
}

template<typename T>
long Position<T>::GetAggregatePosition()
{
    long value=0;

    for(auto& l: positions){
        value+=l.second;
    }

    return value;
}

template<typename T>
void Position<T>::AddQuantity(std::string book, long quantity)
{
    positions[book]+=quantity;
}



//define member functions in class: PositionService
void PositionService::Addpos(Position<Bond>& ps)
{
    position_data.insert(std::make_pair(ps.GetProduct().GetProductId(),ps));
}

void PositionService::AddTrade(const Trade<Bond> &trade)
{
    // Add a trade to the service

    //firstly, making the trade stored
    //Once a booking made, we add the amount of booking of the product in the given position
    std::string productId=trade.GetProduct().GetProductId();

    //define quantity of the trade
    long quantity_of_trade=0;
    if(trade.GetSide()==BUY){
        quantity_of_trade=trade.GetQuantity();
    }
    else{
        quantity_of_trade=-trade.GetQuantity();
    }

    position_data[productId].AddQuantity(trade.GetBook(),quantity_of_trade);

    //pass the trade data to listeners
    std::cout<<"data goes from PositionService -> listener."<<std::endl;
    Position<Bond> pos=position_data[productId];
    for(auto& l: listeners){
        l->ProcessAdd(pos);
    }
};

Position<Bond>& PositionService::GetData(std::string key) 
{
    return position_data.at(key);
}

void PositionService::OnMessage(Position<Bond> &data) {
    // no implememtation here
}

void PositionService::AddListener(ServiceListener<Position<Bond>> *listener) 
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<Position<Bond>>* >& PositionService::GetListeners() const 
{
    return listeners;
}


//define member functions in class: PositionServiceListener
void PositionServiceListener::ProcessAdd(Trade<Bond> &data) 
{
    position_service->AddTrade(data);
}

void PositionServiceListener::ProcessRemove(Trade<Bond> &data) 
{
    // no implementation here
}

void PositionServiceListener::ProcessUpdate(Trade<Bond> &data) 
{
    // no implementation here
}

PositionService* PositionServiceListener::GetService(){
    return position_service;
}
#endif
