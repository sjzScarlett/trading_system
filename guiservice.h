/**GuiService is used to publish the price in the price stream to the user
 * guiservice.hpp
 * guiservice.hpp
 *
 * @author Breman Thuraisingham, Sijia Zhang
 *
 */

#ifndef GUI_SERVICE_HPP
#define GUI_SERVICE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <time.h>
#include <thread>
#include "pricingservice.h"

using namespace std;
/**
 * Service for processing and persisting data to store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class GuiService : Service<string,T>
{

public:

    // Persist data to a store
    virtual void PersistData(string persistKey, T& data) = 0;

};



class BondGuiServiceConnector : public Connector<Price<Bond>>{

private:

    //ctor
    BondGuiServiceConnector(){}

public:

    //Generate Instance
    static BondGuiServiceConnector* Generate_Instance(){
        static BondGuiServiceConnector ins;
        return &ins;
    }

    // member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(Price<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();
};


/**
 * Bond Gui Service connects the data in it and listener
 * We use the type Bond instead of using template T
 */
class BondGuiService : public GuiService<Price<Bond>>{

private:
    // timer
    std::chrono::time_point<std::chrono::system_clock> time;

    //define listener
    std::vector<ServiceListener<Price<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Price<Bond>> gui_data;

    //define Bond Historical Execution Connector
    BondGuiServiceConnector* bond_gui_connector;

    //ctor
    BondGuiService(){
        time = std::chrono::system_clock::now();
        bond_gui_connector=BondGuiServiceConnector::Generate_Instance();
    }

public:

    // Generate instance
    static BondGuiService* Generate_Instance(){
        //we define this function to generate instance for class BondGuiService
        static BondGuiService ins;
        return &ins;
    }

    // Get Time
    std::chrono::time_point<std::chrono::system_clock> getTime(){
        return time;
    }

    // Change Time
    void changeTime(std::chrono::time_point<std::chrono::system_clock>& time_new) {
        time = time_new;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    Price<Bond>& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Price<Bond> &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Price<Bond>> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<Price<Bond>>* >& GetListeners() const  ;

    // make the data out
    void PersistData(string persistKey, Price<Bond>& data)  ;
};


/**
 * Bond Gui Service Listener will wirte the data out
 * We use the type Bond instead of using template T
 */

class BondGuiServiceListener : public ServiceListener<Price<Bond>>{

private:

    BondGuiService * bond_gui_service;

    //ctor
    BondGuiServiceListener(){
        bond_gui_service=BondGuiService::Generate_Instance();
    }

public:

    // Generate instance
    static BondGuiServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondGuiService
        static BondGuiServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(Price<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Price<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Price<Bond> &data)  ;

};





/**************************************************/
//define member functions in class: BondGuiServiceConnector
void BondGuiServiceConnector::Publish(Price<Bond>& data)
{
    //create a new .txt and put the data
    ofstream of;
    of.open("../output/gui.txt" ,ios::app);

    //put the data in
    if(of.is_open()){
        std::string ss="Product: " + data.GetProduct().GetProductId() + ", Mid_price: "
        + std::to_string(data.GetMid());

        of<<ss<<std::endl;
    }
}


void BondGuiServiceConnector::Subscribe()
{
    //no implementation
}



//define member functions in class: BondGuiService
Price<Bond>& BondGuiService::GetData(std::string key)
{
    return gui_data.at(key);
}

void BondGuiService::OnMessage(Price<Bond> &data)
{

    //record new time
    auto time1 = std::chrono::system_clock::now();

    //calculate the differece of time passing in are dealing time
    std::chrono::duration<double> diff = time1-getTime();

    if (diff.count() >= 3e-1) {

        //store the newly or updated data
        auto key=data.GetProduct().GetProductId(); //get key
        gui_data.insert(std::make_pair(key,data));

        //then, pass the updated data to listener
        //pass the trade data to listeners
        std::cout<<"data goes from BondGuiService -> listener."<<'\n'<<std::endl;
        for(auto& l: listeners){
            l->ProcessAdd(data);
        }

        //update new time and record recursionly
        changeTime(time1);


        //publish data
        auto bond_gui_service=BondGuiService::Generate_Instance();
        bond_gui_service->PersistData(data.GetProduct().GetProductId(),data);
    }
}

void BondGuiService::AddListener(ServiceListener<Price<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<Price<Bond>>* >& BondGuiService::GetListeners() const
{
    return listeners;
}

void BondGuiService::PersistData(string persistKey, Price<Bond>& data)
{
    bond_gui_connector->Publish(data);
}



//define member functions in class BondHistoricalStreamingServiceListener
void BondGuiServiceListener::ProcessAdd(Price<Bond> &data)
{
    bond_gui_service->OnMessage(data);
}

void BondGuiServiceListener::ProcessRemove(Price<Bond> &data)  {};

void BondGuiServiceListener::ProcessUpdate(Price<Bond> &data)  {};



#endif