/**
 * inquiryservice.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef INQUIRY_SERVICE_HPP
#define INQUIRY_SERVICE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "soa.h"
#include "tradebookingservice.h"

// Various inqyury states
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

/**
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{

public:

    // ctor for an inquiry
    Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state);

    // Get the inquiry ID
    const string& GetInquiryId() const;

    // Get the product
    const T& GetProduct() const;

    // Get the side on the inquiry
    Side GetSide() const;

    // Get the quantity that the client is inquiring for
    long GetQuantity() const;

    // Get the price that we have responded back with
    double GetPrice() const;

    // Get the current state on the inquiry
    InquiryState GetState() const;

    //define a change state member function
    void ChangeState(InquiryState ss){
        state=ss;
    }

    //define a change price member function
    void ChangePrice(double pri){
        price=pri;
    }

    friend ostream& operator << (ostream& os, const Inquiry<T>& inq) {
        string state;
        switch(inq.GetState()) {
            case RECEIVED : state = "RECEIVED"; break;
            case QUOTED : state = "QUOTED"; break;
            case DONE : state = "DONE"; break;
            case REJECTED : state = "REJECTED"; break;
            case CUSTOMER_REJECTED : state = "CUSTOMER_REJECTED";
        }

        //print out
        os << "InquiryID : " << inq.GetInquiryId()
           << ", side: " << (inq.GetSide() == BUY ? "BUY" : "SELL")
           << ", quantity: " << inq.GetQuantity()
           << ", price: " << inq.GetPrice()
           << ", state: " << state;

        return os;
    }

private:
    string inquiryId;
    T product;
    Side side;
    long quantity;
    double price;
    InquiryState state;

};

/**
 * Service for customer inquirry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since each inquiry must be unique).
 * Type T is the product type.
 */
template<typename T>
class InquiryService : public Service<string,Inquiry <T> >
{

public:

    // Send a quote back to the client
    virtual void SendQuote(const string &inquiryId, double price) = 0;

    // Reject an inquiry from the client
    virtual void RejectInquiry(const string &inquiryId) = 0;

};


/**
 * BondInquiryService to deal with inquiry pass from connector
 * We use type Bond instead of using template T
 */
class BondInquiryService : public InquiryService<Bond>{

private:

    //define listener
    std::vector<ServiceListener<Inquiry<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Inquiry<Bond>> inquiry_data;

    //ctor
    BondInquiryService(){};

public:

    // Generate instance
    static BondInquiryService* Generate_Instance(){
        //we define this function to generate instance for class BondInquiryService.
        static BondInquiryService ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    Inquiry<Bond>& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Inquiry<Bond> &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Inquiry<Bond>> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<Inquiry<Bond>>* >& GetListeners() const  ;

    // Send a quote back to the client
    void SendQuote(const string &inquiryId, double price)  ;

    // Reject an inquiry from the client
    void RejectInquiry(const string &inquiryId)  ;

};


/**
 * BondInquiryServiceConnector should transition the inquiry to the QUOTED state and send it back to the BondInquiryService via the OnMessage method with the supplied price
 * We use type Bond instead of using template T
 */

class BondInquiryServiceConnector : public Connector<Inquiry<Bond>>{

private:

    BondProductService * bond_product_service;
    BondInquiryService * bond_inquiry_service;

    //ctor
    BondInquiryServiceConnector(){
        bond_inquiry_service=BondInquiryService::Generate_Instance();
        bond_product_service=BondProductService::Generate_Instance();
    }

public:

    // Generate instance
    static BondInquiryServiceConnector* Generate_Instance(){
        //we define this function to generate instance for class BondInquiryServiceConnector.
        static BondInquiryServiceConnector ins;
        return &ins;
    }

    //   member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(Inquiry<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();

    // GetService
    InquiryService<Bond>* GetService();


};
/***********************************************************************************************/
//define member functions in class: Inquiry
template<typename T>
Inquiry<T>::Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state) :
        product(_product)
{
    inquiryId = _inquiryId;
    side = _side;
    quantity = _quantity;
    price = _price;
    state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
    return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
    return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
    return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
    return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
    return state;
}



//define member functions in class BondInquiryService
Inquiry<Bond>& BondInquiryService::GetData(std::string key)  
{
    return inquiry_data.at(key);
}

void BondInquiryService::OnMessage(Inquiry<Bond> &data)  
{
    //first, we judge whether the type of inquiry is : QUOTED
    if(data.GetState()==QUOTED){
        auto it=BondInquiryServiceConnector::Generate_Instance();
        data.ChangeState(DONE);
        it->Publish(data);
        inquiry_data.insert(std::make_pair(data.GetInquiryId(),data));

        //transform the data to listener
        std::cout<<"data goes from BondInquiryService -> listener."<<std::endl;
        inquiry_data.insert(std::make_pair(data.GetInquiryId(),data));
        for(auto& l: listeners){
            l->ProcessAdd(data);
        }
    }

    else {
        //send back nothing
        //transform the data to listener
        std::cout<<"data goes from BondInquiryService -> listener."<<std::endl;
        for(auto& l: listeners){
            l->ProcessAdd(data);
        }
    }
}

void BondInquiryService::AddListener(ServiceListener<Inquiry<Bond>> *listener)  
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<Inquiry<Bond>>* >& BondInquiryService::GetListeners() const  
{
    return listeners;
}

void BondInquiryService::SendQuote(const string &inquiryId, double price)  
{
    // don't use this function
}

// Reject an inquiry from the client
void BondInquiryService::RejectInquiry(const string &inquiryId)  
{
    // no implementation here
}



//define member functions in class BondInquiryServiceConnector
void BondInquiryServiceConnector::Publish(Inquiry<Bond>& data)  
{
    data.ChangePrice(100.00);
}

void BondInquiryServiceConnector::Subscribe()
{
    //define some strings we will use later
    std::string key, side, quantity, price, state;

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
    ifstream iss("../input/inquiries.txt");
    std::string line;
    getline(iss,line);

    //if the file is not empty
    while(getline(iss, line)){

        //define inquiry id
        static int inquiryID=0;
        ++inquiryID;

        //create a vector to contain the parts in each line
        std::vector<std::string> container=splitoneline(line);

        //using the string defined above as the name of different parts
        key=container[0];
        side=container[1];
        quantity=container[2];
        price=container[3];
        state=container[4];

        InquiryState real_state;
        if(state=="RECEIVED"){
            real_state==RECEIVED;
        }
        //define an Inquiry
        auto bond=bond_product_service->GetData(key);
        Inquiry<Bond> inb("INQ"+std::to_string(inquiryID), bond, (side == "BUY" ? BUY : SELL),static_cast<long>(std::stol(quantity)),std::stoi(price),real_state);

        //send back a quote
        bond_inquiry_service->SendQuote(std::to_string(inquiryID), std::stoi(price));

        //set the state to QUOTE
        inb.ChangeState(QUOTED);
        bond_inquiry_service->OnMessage(inb);
    }

    std::cout<<"input/inquiries.txt -> BondInquiryService DONE!"<<std::endl;
}

// GetService
InquiryService<Bond>* BondInquiryServiceConnector::GetService()
{
    return bond_inquiry_service;
}

#endif

