/**
 * pricingservice.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "soa.h"
#include "products.h"

/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price
{

public:

    // ctor for a price
    Price(const T &_product, double _mid, double _bidOfferSpread);

    // Get the product
    const T& GetProduct() const;

    // Get the mid price
    double GetMid() const;

    // Get the bid/offer spread around the mid
    double GetBidOfferSpread() const;

    // we add an operator << as overloading
    friend ostream& operator << (ostream& os, const Price<T>& pri){
        os<<"Product is: "<<pri.GetProduct()<<std::endl;
        os<<"Mid price is: "<<pri.GetMid()<<std::endl;
        os<<"Bid/offer spread is: "<<pri.GetBidOfferSpread()<<std::endl;
        return os;
    }
private:
    const T& product;
    double mid;
    double bidOfferSpread;

};

/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * We use exact type Bond instead of using template.
 */

class PricingService : public Service<string,Price <Bond> >
{

private:

    //define listener
    std::vector<ServiceListener<Price<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Price<Bond>> price_data;

    //ctor
    PricingService(){};

public:

    // Generate instance
    static PricingService* Generate_Instance(){
        //we define this function to generate instance for class PricingService.
        static PricingService ins;
        return &ins;
    }

    // pure virtual member functions in class Service.
    // Get data on our service given a key
    Price<Bond>& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Price<Bond> &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Price<Bond>> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<Price<Bond>>* >& GetListeners() const ;
};


/**
 * Connector is used to read the data in the .txt file into service
 * Keyed on product identifier.
 * Using Bond as our type.
 */

//The PricingServiceConnector is subscriber-only
class PricingServiceConnector: public Connector<Price<Bond>>{

private:
//create 2 pointers one for PricingService, the other for BondProductService
    PricingService* price_service;
    BondProductService* bond_product_service;

    //ctor
    PricingServiceConnector(){
        price_service=PricingService::Generate_Instance();
        bond_product_service=BondProductService::Generate_Instance();
    }

public:

    //Generate Instance
    static PricingServiceConnector* Generate_Instance(){
        static PricingServiceConnector ins;
        return &ins;
    }

    //  member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(Price<Bond>& data) ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();

    // GetService
    PricingService* GetService();

};


//define member functions in class: Price
template<typename T>
Price<T>::Price(const T &_product, double _mid, double _bidOfferSpread) :
        product(_product)
{
    mid = _mid;
    bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const
{
    return product;
}

template<typename T>
double Price<T>::GetMid() const
{
    return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
    return bidOfferSpread;
}



//define member functions in class: PricingService
Price<Bond>& PricingService::GetData(std::string key) 
{
    //get data given a key
    return price_data.at(key);
}

void PricingService::OnMessage(Price<Bond> &data) 
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    price_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    std::cout<<"data goes from PricingService -> listener."<<std::endl;
    for(auto& l:listeners){
        l->ProcessAdd(data);
    }
}

void PricingService::AddListener(ServiceListener<Price<Bond>> *listener) 
{
    //add a listener to the service
    listeners.push_back(listener);
}

const vector<ServiceListener<Price<Bond>>* >& PricingService::GetListeners() const 
{
    //get all listeners on the service
    return listeners;
}



//define member functions in class: PricingServiceConnector
void PricingServiceConnector::Publish(Price<Bond> &data) 
{
    //no implementation inside of this function
}

void PricingServiceConnector::Subscribe()
{
    //define some strings we will use later
    std::string key, mid, bid_offer;

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
    ifstream iss("../input/prices.txt");
    std::string line;
    getline(iss,line);

    //if the file is not empty
    while(getline(iss, line)){
        //create a vector to contain the parts in each line
        std::vector<std::string> container=splitoneline(line);

        //using the string defined above as the name of different parts
        key=container[0];
        mid=container[1];
        bid_offer=container[2];

        //define price
        //find the bond in order to define trade
        auto bond=bond_product_service->GetData(key);
        Price<Bond> price(bond, PriceTranspose(mid), PriceTranspose(bid_offer));

        //using OnMessage to pass the data to PricingService
        price_service->OnMessage(price);
    }

    std::cout<<"input/price.txt -> PricingService DONE!"<<std::endl;
}

PricingService* PricingServiceConnector::GetService()
{
    return price_service;
}

#endif
