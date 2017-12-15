/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include "soa.h"
#include "positionservice.h"

/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{

public:

    //ctor
    PV01(){};

    // ctor for a PV01 value
    PV01(const T &_product, double _pv01, long _quantity);

    // Get the product on this PV01 value
    const T& GetProduct() const;

    // Get the PV01 value
    double GetPV01() const;

    // Get the quantity that this risk value is associated with
    long GetQuantity() const;

    // AddQuant is used to add the quantity, similar to AddQuantity in class: Position
    void AddQuant(long quantity_new);

    void AddPV01(double pv01_new) {
        pv01 += pv01_new;
    }

private:
    T product;
    double pv01;
    long quantity;

};

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:

    // ctor for a bucket sector
    BucketedSector(const vector<T> &_products, string _name);

    // Get the products associated with this bucket
    const vector<T>& GetProducts() const;

    // Get the name of the bucket
    const string& GetName() const;

private:
    vector<T> products;
    string name;

};

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * We use the type Bond instead of using template T
 */

class RiskService : public Service<string,PV01 <Bond> >
{

private:

    //define listener
    std::vector<ServiceListener<PV01<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, PV01<Bond>> risk_data;

    //ctor
    RiskService(){};

public:

    // Generate instance
    static RiskService* Generate_Instance(){
        //we define this function to generate instance for class TradeBookingService.
        static RiskService ins;
        return &ins;
    }

    //Add PV01
    void AddRisk(PV01<Bond> &rd);

    // Add a position that the service will risk
    void AddPosition(Position<Bond> &position);

    // Get the bucketed risk for the bucket sector
    double GetBucketedRisk(const BucketedSector<Bond> &sector);

    // pure virtual member functions in class Service.
    // Get data on our service given a key
    PV01<Bond>& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PV01<Bond> &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PV01<Bond>> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<PV01<Bond>>* >& GetListeners() const ;

};


/**
 * Risk Service Listener callbacks to process add, remove and update events to the service
 * We use the specific type: Bond instead of using template T.
 */

class RiskServiceListener : public ServiceListener<Position<Bond>>{

private:

    //define a pointer of PositionService
    RiskService * risk_service;

    //ctor
    RiskServiceListener(){
        risk_service=RiskService::Generate_Instance();
    }

public:

    // Generate instance
    static RiskServiceListener* Generate_Instance(){
        //we define this function to generate instance for class PositionService.
        static RiskServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    //Link PositionService and RiskService through Service Listener
    // Listener callback to process an add event to the Service
    void ProcessAdd(Position<Bond> &data) ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Position<Bond> &data) ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Position<Bond> &data) ;

    // return position service
    RiskService* GetRiskService();
};



//define member functions in class: PV01
template<typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
        product(_product)
{
    pv01 = _pv01;
    quantity = _quantity;
}

template<typename T>
const T& PV01<T>::GetProduct() const
{
    return product;
}

template<typename T>
double PV01<T>::GetPV01() const
{
    return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const
{
    return quantity;
}

template <typename T>
void PV01<T>::AddQuant(long quantity_new)
{
    quantity+=quantity_new;
}


//define member functions in class: BucketSector
template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
        products(_products)
{
    name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
    return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
    return name;
}


//define member functions in class: RiskService
void RiskService::AddRisk(PV01<Bond> &rd)
{
    risk_data.insert(std::make_pair(rd.GetProduct().GetProductId(),rd));
}

void RiskService::AddPosition(Position<Bond> &position)
{
    // Add positions to the service

    //firstly, making the positions stored
    //Once a position made, we add the amount of positions in our risk analysis
    std::string productId=position.GetProduct().GetProductId();

    //define quantity of the position
    long quantity_of_position=position.GetAggregatePosition();
    //define new risk
    long new_risk = rand() % 100;

    //store the position
    if(risk_data.find(productId)->first==productId){
        //If we can find the key of risk_data, then just add the quantity
        risk_data[productId].AddPV01(new_risk);
        risk_data[productId].AddQuant(quantity_of_position);
    }

    //pass the trade data to listeners
    std::cout<<"data goes from RiskService -> listener."<<std::endl;
    PV01<Bond> pv=risk_data[productId];
    for(auto& l: listeners){
        l->ProcessAdd(pv);
    }
}

double RiskService::GetBucketedRisk(const BucketedSector<Bond> &sector)
{
    std::vector<std::string> cuips;
    double sum_pv01=0;

    //go through each product in bucket sector and add the risk together
    for(auto l:sector.GetProducts()){
        cuips.push_back(l.GetProductId());
    }

    for(auto& ss:cuips){
        auto it=risk_data[ss];

        //we want a positive total risk
        sum_pv01+=it.GetPV01()* (-it.GetQuantity()); //we use minus of PV01's quantity since if it is BUY, then quantity will be negative.
    }

    return sum_pv01;
}

PV01<Bond>& RiskService::GetData(std::string key) 
{
    return risk_data.at(key);
}

void RiskService::OnMessage(PV01<Bond> &data) 
{
    // no implememtation here
}

void RiskService::AddListener(ServiceListener<PV01<Bond>> *listener) 
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<PV01<Bond>>* >& RiskService::GetListeners() const 
{
    return listeners;
}


//define member functions in class: RiskServiceListener
void RiskServiceListener::ProcessAdd(Position<Bond> &data) 
{
    risk_service->AddPosition(data);
}

// Listener callback to process a remove event to the Service
void RiskServiceListener::ProcessRemove(Position<Bond> &data) 
{
    // no implementation here
}

// Listener callback to process an update event to the Service
void RiskServiceListener::ProcessUpdate(Position<Bond> &data) 
{
    // no implementation here
}

// return position service
RiskService* RiskServiceListener::GetRiskService()
{
    return risk_service;
}

#endif
