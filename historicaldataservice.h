/**
 * historicaldataservice.hpp
 * historicaldataservice.hpp
 *
 * @author Breman Thuraisingham, Sijia Zhang
 * Defines the data types and Service for historical data.
 *
 * @author Breman Thuraisingham
 */
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include "positionservice.h"
#include "riskservice.h"
#include "bondexecutionservice.h"
#include "streamingservice.h"
#include "inquiryservice.h"
#include "support.h"

/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class HistoricalDataService : Service<string,T>
{

public:

    // Persist data to a store
    virtual void PersistData(string persistKey, T& data) = 0;

};


/***************************************************************************************/
/**
 * create position.txt.
 */

/**
 * Bond Historical Position Connector is used to create the file of position.txt
 * We use the type Bond instead of using template T
 */
class BondHistoricalPositionConnector : public Connector<Position<Bond>>{

private:

    //ctor
    BondHistoricalPositionConnector(){}

public:

    //Generate Instance
    static BondHistoricalPositionConnector* Generate_Instance(){
        static BondHistoricalPositionConnector ins;
        return &ins;
    }

    //   member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(Position<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();
};


/**
 * Bond Historical Position Service connects the data in it and listener
 * We use the type Bond instead of using template T
 */
class BondHistoricalPositionService : public HistoricalDataService<Position<Bond>>{

private:
    //define listener
    std::vector<ServiceListener<Position<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Position<Bond>> pos_data;

    //define Bond Historical Position Connector
    BondHistoricalPositionConnector* bond_his_pos_connector;

    //ctor
    BondHistoricalPositionService(){
        bond_his_pos_connector=BondHistoricalPositionConnector::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalPositionService* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalPositionService.
        static BondHistoricalPositionService ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    Position<Bond>& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Position<Bond> &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Position<Bond>> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<Position<Bond>>* >& GetListeners() const  ;

    // make the data out
    void PersistData(string persistKey, Position<Bond>& data)  ;
};


/**
 * Bond Historical Position Service Listener will wirte the data out
 * We use the type Bond instead of using template T
 */

class BondHistoricalPositionServiceListener : public ServiceListener<Position<Bond>>{

private:

    BondHistoricalPositionService * bond_his_pos_service;

    //ctor
    BondHistoricalPositionServiceListener(){
        bond_his_pos_service=BondHistoricalPositionService::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalPositionServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalPositionService
        static BondHistoricalPositionServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(Position<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Position<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Position<Bond> &data)  ;

};




/*************************************************************************************/
/**
 * create risk.txt.
 */

/**
 * Bond Historical PV01 Connector is used to create the file of risk.txt
 * We use the type Bond instead of using template T
 */
class BondHistoricalPV01Connector : public Connector<PV01<Bond>>{

private:

    //ctor
    BondHistoricalPV01Connector(){}

public:

    //Generate Instance
    static BondHistoricalPV01Connector* Generate_Instance(){
        static BondHistoricalPV01Connector ins;
        return &ins;
    }

    //   member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(PV01<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();
};


/**
 * Bond Historical PV01 Service connects the data in it and listener
 * We use the type Bond instead of using template T
 */
class BondHistoricalPV01Service : public HistoricalDataService<PV01<Bond>>{

private:
    //define listener
    std::vector<ServiceListener<PV01<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, PV01<Bond>> pv01_data;

    //define Bond Historical PV01 Connector
    BondHistoricalPV01Connector* bond_his_pv01_connector;

    //ctor
    BondHistoricalPV01Service(){
        bond_his_pv01_connector=BondHistoricalPV01Connector::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalPV01Service* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalPV01Service.
        static BondHistoricalPV01Service ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    PV01<Bond>& GetData(std::string key);

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PV01<Bond> &data);

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PV01<Bond>> *listener);

    // Get all listeners on the Service.
    const std::vector< ServiceListener<PV01<Bond>>* >& GetListeners() const;

    // make the data out
    void PersistData(string persistKey, PV01<Bond>& data);
};

/**
 * Bond Historical PV01 Service Listener will wirte the data out
 * We use the type Bond instead of using template T
 */

class BondHistoricalPV01ServiceListener : public ServiceListener<PV01<Bond>>{

private:

    BondHistoricalPV01Service * bond_his_pv01_service;

    //ctor
    BondHistoricalPV01ServiceListener(){
        bond_his_pv01_service=BondHistoricalPV01Service::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalPV01ServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalPV01Service
        static BondHistoricalPV01ServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(PV01<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(PV01<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(PV01<Bond> &data)  ;

};



/*************************************************************************************/
/**
/ * create execution.txt.
/ */

/**
 * Bond Historical Execution Connector is used to create the file of execution.txt
 * We use the type Bond instead of using template T
 */
class BondHistoricalExecutionConnector : public Connector<ExecutionOrder<Bond>>{

private:

    //ctor
    BondHistoricalExecutionConnector(){}

public:

    //Generate Instance
    static BondHistoricalExecutionConnector* Generate_Instance(){
        static BondHistoricalExecutionConnector ins;
        return &ins;
    }

    //   member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(ExecutionOrder<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();
};


/**
 * Bond Historical PV01 Service connects the data in it and listener
 * We use the type Bond instead of using template T
 */
class BondHistoricalExecutionService : public HistoricalDataService<ExecutionOrder<Bond>>{

private:
    //define listener
    std::vector<ServiceListener<ExecutionOrder<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, ExecutionOrder<Bond>> execution_data;

    //define Bond Historical Execution Connector
    BondHistoricalExecutionConnector* bond_his_exe_connector;

    //ctor
    BondHistoricalExecutionService(){
        bond_his_exe_connector=BondHistoricalExecutionConnector::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalExecutionService* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalExecutionService.
        static BondHistoricalExecutionService ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    ExecutionOrder<Bond>& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(ExecutionOrder<Bond> &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<ExecutionOrder<Bond>>* >& GetListeners() const  ;

    // make the data out
    void PersistData(string persistKey, ExecutionOrder<Bond>& data)  ;
};


/**
 * Bond Historical PV01 Service Listener will wirte the data out
 * We use the type Bond instead of using template T
 */

class BondHistoricalExecutionServiceListener : public ServiceListener<ExecutionOrder<Bond>>{

private:

    BondHistoricalExecutionService * bond_his_execution_service;

    //ctor
    BondHistoricalExecutionServiceListener(){
        bond_his_execution_service=BondHistoricalExecutionService::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalExecutionServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalExecutionService
        static BondHistoricalExecutionServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(ExecutionOrder<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(ExecutionOrder<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(ExecutionOrder<Bond> &data)  ;

};





/*************************************************************************************/
/**
/ * create streaming.txt.
/ */

/**
 * Bond Historical Streaming Connector is used to create the file of streaming.txt
 * We use the type Bond instead of using template T
 */

class BondHistoricalStreamingConnector : public Connector<PriceStream<Bond>>{

private:

    //ctor
    BondHistoricalStreamingConnector(){}

public:

    //Generate Instance
    static BondHistoricalStreamingConnector* Generate_Instance(){
        static BondHistoricalStreamingConnector ins;
        return &ins;
    }

    //   member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(PriceStream<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();
};


/**
 * Bond Historical Streaming Service connects the data in it and listener
 * We use the type Bond instead of using template T
 */
class BondHistoricalStreamingService : public HistoricalDataService<PriceStream<Bond>>{

private:
    //define listener
    std::vector<ServiceListener<PriceStream<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, PriceStream<Bond>> streaming_data;

    //define Bond Historical Execution Connector
    BondHistoricalStreamingConnector* bond_his_stream_connector;

    //ctor
    BondHistoricalStreamingService(){
        bond_his_stream_connector=BondHistoricalStreamingConnector::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalStreamingService* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalExecutionService.
        static BondHistoricalStreamingService ins;
        return &ins;
    }

    //  pure virtual member functions in class Service.
    // Get data on our service given a key
    PriceStream<Bond>& GetData(std::string key)  ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PriceStream<Bond> &data)  ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PriceStream<Bond>> *listener)  ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<PriceStream<Bond>>* >& GetListeners() const  ;

    // make the data out
    void PersistData(string persistKey, PriceStream<Bond>& data)  ;
};


/**
 * Bond Historical Streaming Service Listener will wirte the data out
 * We use the type Bond instead of using template T
 */

class BondHistoricalStreamingServiceListener : public ServiceListener<PriceStream<Bond>>{

private:

    BondHistoricalStreamingService * bond_his_streaming_service;

    //ctor
    BondHistoricalStreamingServiceListener(){
        bond_his_streaming_service=BondHistoricalStreamingService::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalStreamingServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalStreamingService
        static BondHistoricalStreamingServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(PriceStream<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(PriceStream<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(PriceStream<Bond> &data)  ;

};





/*************************************************************************************/
/**
/ * create allinquiries.txt.
/ */

/**
 * Bond Historical Inauiries Connector is used to create the file of streaming.txt
 * We use the type Bond instead of using template T
 */

class BondHistoricalInquiryConnector : public Connector<Inquiry<Bond>>{

private:

    //ctor
    BondHistoricalInquiryConnector(){}

public:

    //Generate Instance
    static BondHistoricalInquiryConnector* Generate_Instance(){
        static BondHistoricalInquiryConnector ins;
        return &ins;
    }

    //   member function Publish in class Connector
    // Since it is a subscribe-only class, so there is no implementation in the Publish
    void Publish(Inquiry<Bond>& data)  ;

    // Subscribe
    // It is used for reading data from file via OnMessage Method
    void Subscribe();
};


/**
 * Bond Historical Inquiry Service connects the data in it and listener
 * We use the type Bond instead of using template T
 */
class BondHistoricalInquiryService : public HistoricalDataService<Inquiry<Bond>>{

private:
    //define listener
    std::vector<ServiceListener<Inquiry<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, Inquiry<Bond>> inquiry_data;

    //define Bond Historical Inquiry Connector
    BondHistoricalInquiryConnector* bond_his_inquiry_connector;

    //ctor
    BondHistoricalInquiryService(){
        bond_his_inquiry_connector=BondHistoricalInquiryConnector::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalInquiryService* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalInquiryService.
        static BondHistoricalInquiryService ins;
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

    // make the data out
    void PersistData(string persistKey, Inquiry<Bond>& data)  ;
};


/**
 * Bond Historical Inquiry Service Listener will wirte the data out
 * We use the type Bond instead of using template T
 */

class BondHistoricalInquiryServiceListener : public ServiceListener<Inquiry<Bond>>{

private:

    BondHistoricalInquiryService * bond_his_inquiry_service;

    //ctor
    BondHistoricalInquiryServiceListener(){
        bond_his_inquiry_service=BondHistoricalInquiryService::Generate_Instance();
    }

public:

    // Generate instance
    static BondHistoricalInquiryServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondHistoricalInquiryService
        static BondHistoricalInquiryServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(Inquiry<Bond> &data)  ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Inquiry<Bond> &data)  ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Inquiry<Bond> &data)  ;

};


/***************************************************************************/
//define member functions in class BondHistoricalPositionConnector
void BondHistoricalPositionConnector::Publish(Position<Bond>& data)
{
    std::string s1="TRSY1", s2="TRSY2", s3="TRSY3";

    //create a new .txt and put the data of position in it
    ofstream of;
    of.open("../output/position.txt" ,ios::app);

    //put the data in
    if(of.is_open()){
        //we should persist each position for a given book as well as the aggregate position.
        std::string ss= "ProductId: " + data.GetProduct().GetProductId() + "BOOK : TRSY1, total position: " + std::to_string(data.GetPosition(s1)) + ", BOOK : TRSY2, total position:"
        + std::to_string(data.GetPosition(s2)) + ", BOOK : TRSY3, total position: " + std::to_string(data.GetPosition(s3))
        + ", Aggregate position: " + std::to_string(data.GetAggregatePosition());

        //Persist each position for 3 kinds of books! Note: Positions for a given book added from previous state if a new trade is read!
        of<<ss<<std::endl;
    }

}



void BondHistoricalPositionConnector::Subscribe()
{
    // no implementation
}



//define member functions in class: BondHistoricalPositionService
Position<Bond>& BondHistoricalPositionService::GetData(std::string key)
{
    return pos_data.at(key);
}

void BondHistoricalPositionService::OnMessage(Position<Bond> &data)
{
    //firstly, store the newly or updated data
    std::string key=data.GetProduct().GetProductId(); //get key
    pos_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    //pass the trade data to listeners
    std::cout<<"data goes from BondHistoricalPositionService -> listener."<<'\n'<<std::endl;
    for(auto& l: listeners){
        l->ProcessAdd(data);
    }
}

void BondHistoricalPositionService::AddListener(ServiceListener<Position<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<Position<Bond>>* >& BondHistoricalPositionService::GetListeners() const
{
    return listeners;
}

// make positions out
void BondHistoricalPositionService::PersistData(string persistKey, Position<Bond>& data)
{
    bond_his_pos_connector->Publish(data);
}



//define member function in class: BondHistoricalPositionServiceListener
void BondHistoricalPositionServiceListener::ProcessAdd(Position<Bond> &data)
{
    bond_his_pos_service->OnMessage(data);

    //write it
    bond_his_pos_service->PersistData(data.GetProduct().GetProductId(),data);
}

void BondHistoricalPositionServiceListener::ProcessRemove(Position<Bond> &data)
{
    // no implementation
}

void BondHistoricalPositionServiceListener::ProcessUpdate(Position<Bond> &data)
{
    // no implementation
}




//define member functions in class: BondHistoricalPV01Connector
void BondHistoricalPV01Connector::Publish(PV01<Bond>& data)
{
    //firstly, define a group of busket
    //define FrontEnd Bucket
    std::vector<Bond> front_end{GetBond(cusip1_year_2), GetBond(cusip2_year_3)};
    BucketedSector<Bond> FrontEnd_sector(front_end, "FrontEnd");

    //define Belly busket
    std::vector<Bond> belly{GetBond(cusip3_year_5), GetBond(cusip4_year_7), GetBond(cusip5_year_10)};
    BucketedSector<Bond> Belly_sector(belly, "Belly");

    //define LongEnd busket
    std::vector<Bond> Long_end{GetBond(cusip6_year_30)};
    BucketedSector<Bond> LongEnd_sector(Long_end, "LongEnd");

    RiskService * rs=RiskService::Generate_Instance();

    //Then, write the file
    //we should persist risk for each security as well as for the following bucket sectors: FrontEnd (2Y, 3Y), Belly (5Y, 7Y, 10Y), and LongEnd (30Y).
    ofstream of("../output/risk.txt", ios_base::app);
    std::string ss = "Product: " + data.GetProduct().GetProductId() + ", PV01: " + std::to_string(data.GetPV01())
    + ", Total Risk(FrontEnd): " + std::to_string(rs->GetBucketedRisk(FrontEnd_sector))
    + ", Total Risk(Belly): "+ std::to_string(rs->GetBucketedRisk(Belly_sector))
    + ", Total Risk(LongEnd): "+std::to_string(rs->GetBucketedRisk(LongEnd_sector));

    of<<ss<<std::endl;
}

void BondHistoricalPV01Connector::Subscribe()
{
    // no implementation
}





//define member functions in class: BondHistoricalPV01Service
PV01<Bond>& BondHistoricalPV01Service::GetData(std::string key)
{
    return pv01_data.at(key);
}

void BondHistoricalPV01Service::OnMessage(PV01<Bond> &data)
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    pv01_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    //pass the trade data to listeners
    std::cout<<"data goes from BondHistoricalPV01Service -> listener."<<'\n'<<std::endl;

    for(auto& l: listeners){
        l->ProcessAdd(data);
    }
}

void BondHistoricalPV01Service::AddListener(ServiceListener<PV01<Bond>> *listener)
{
    listeners.push_back(listener);
}

// Get all listeners on the Service.
const std::vector< ServiceListener<PV01<Bond>>* >& BondHistoricalPV01Service::GetListeners() const
{
    return listeners;
}

void BondHistoricalPV01Service::PersistData(string persistKey, PV01<Bond>& data)
{
    bond_his_pv01_connector->Publish(data);
}




//define member functions in class: BondHistoricalPV01ServiceListener
void BondHistoricalPV01ServiceListener::ProcessAdd(PV01<Bond> &data)
{
    bond_his_pv01_service->OnMessage(data);

    //write it
    bond_his_pv01_service->PersistData(data.GetProduct().GetProductId(),data);
}

void BondHistoricalPV01ServiceListener::ProcessRemove(PV01<Bond> &data)
{
    // no implementation
}

void BondHistoricalPV01ServiceListener::ProcessUpdate(PV01<Bond> &data)
{
    // no implementation
}







//define member functions in class: BondHistoricalExecutionConnector
void BondHistoricalExecutionConnector::Publish(ExecutionOrder<Bond>& data)
{
    //create a new .txt and put the data
    ofstream of;
    of.open("../output/execution.txt" ,ios::app);

    //put the data in
    if(of.is_open()){
        std::string ss="Product: " + data.GetProduct().GetProductId() + ", ";
        of<<ss<<data<<std::endl;
    }
}

void BondHistoricalExecutionConnector::Subscribe()
{
    // no implementation
}



//define member functions in class: BondHistoricalExecutionService
ExecutionOrder<Bond>& BondHistoricalExecutionService::GetData(std::string key)
{
    return execution_data.at(key);
}

void BondHistoricalExecutionService::OnMessage(ExecutionOrder<Bond> &data)
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    execution_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    //pass the trade data to listeners
    std::cout<<"data goes from BondHistoricalExecutionService -> listener."<<'\n'<<std::endl;
    for(auto& l: listeners){
        l->ProcessAdd(data);
    }
}

void BondHistoricalExecutionService::AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<ExecutionOrder<Bond>>* >& BondHistoricalExecutionService::GetListeners() const
{
    return listeners;
}

void BondHistoricalExecutionService::PersistData(string persistKey, ExecutionOrder<Bond>& data)
{
    bond_his_exe_connector->Publish(data);
}


//define member functions in class BondHistoricalExecutionServiceListener
void BondHistoricalExecutionServiceListener::ProcessAdd(ExecutionOrder<Bond> &data)
{
    bond_his_execution_service->OnMessage(data);
    bond_his_execution_service->PersistData(data.GetProduct().GetProductId(),data);
}

void BondHistoricalExecutionServiceListener::ProcessRemove(ExecutionOrder<Bond> &data)  {};

void BondHistoricalExecutionServiceListener::ProcessUpdate(ExecutionOrder<Bond> &data)  {};






//define member functions in class: BondHistoricalStreamingConnector
void BondHistoricalStreamingConnector::Publish(PriceStream<Bond>& data)
{
    //create a new .txt and put the data
    ofstream of;
    of.open("../output/streaming.txt" ,ios::app);

    //summarize the input
    double bid_price=data.GetBidOrder().GetPrice();
    double offer_price=data.GetOfferOrder().GetPrice();

    double bid_vq=data.GetBidOrder().GetVisibleQuantity();
    double offer_vq=data.GetOfferOrder().GetVisibleQuantity();

    double bid_hq=data.GetBidOrder().GetHiddenQuantity();
    double offer_hq=data.GetOfferOrder().GetHiddenQuantity();

    //put the data in
    if(of.is_open()){
        std::string ss="Product: " + data.GetProduct().GetProductId() + " , Bid Price: "
        + std::to_string(bid_price) + " , Bid Visible Quantity: " + std::to_string((int)bid_vq)
        + ", Bid Hidden Quantity: " + std::to_string((int)bid_hq) + " , Offer Price: " + std::to_string(offer_price)
        +  ", Offer Visible Quantity: " + std::to_string((int)offer_vq) + ", Offer Hidden Quantity: " + std::to_string((int)offer_hq);

        of<<ss<<std::endl;
    }
}

void BondHistoricalStreamingConnector::Subscribe()
{
    // no implementation
}


//define member functions in class: BondHistoricalStreamingService
PriceStream<Bond>& BondHistoricalStreamingService::GetData(std::string key)
{
    return streaming_data.at(key);
}

void BondHistoricalStreamingService::OnMessage(PriceStream<Bond> &data)
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    streaming_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    //pass the trade data to listeners
    std::cout<<"data goes from BondHistoricalStreamingService -> listener."<<'\n'<<std::endl;
    for(auto& l: listeners){
        l->ProcessAdd(data);
    }
}

void BondHistoricalStreamingService::AddListener(ServiceListener<PriceStream<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<PriceStream<Bond>>* >& BondHistoricalStreamingService::GetListeners() const
{
    return listeners;
}

void BondHistoricalStreamingService::PersistData(string persistKey, PriceStream<Bond>& data)
{
    bond_his_stream_connector->Publish(data);
}


//define member functions in class BondHistoricalStreamingServiceListener
void BondHistoricalStreamingServiceListener::ProcessAdd(PriceStream<Bond> &data)
{
    bond_his_streaming_service->OnMessage(data);
    bond_his_streaming_service->PersistData(data.GetProduct().GetProductId(),data);
}

void BondHistoricalStreamingServiceListener::ProcessRemove(PriceStream<Bond> &data)  {};

void BondHistoricalStreamingServiceListener::ProcessUpdate(PriceStream<Bond> &data)  {};





//define member functions in class: BondHistoricalInquiryConnector
void BondHistoricalInquiryConnector::Publish(Inquiry<Bond>& data)
{
    //create a new .txt and put the data
    ofstream of;
    of.open("../output/allinquiries.txt" ,ios::app);

    //define state
    std::string state;
    if(data.GetState()==DONE){
        state="Done";
    }
    //put the data in
    if(of.is_open()){
        std::string ss = "ProductID: " + data.GetProduct().GetProductId();
        of<<ss<<", "<<data<<std::endl;
    }
}

void BondHistoricalInquiryConnector::Subscribe()
{
    // no implementation
}



//define member functions in class: BondHistoricalInquiryService
Inquiry<Bond>& BondHistoricalInquiryService::GetData(std::string key)
{
    return inquiry_data.at(key);
}

void BondHistoricalInquiryService::OnMessage(Inquiry<Bond> &data)
{
    //firstly, store the newly or updated data
    auto key=data.GetProduct().GetProductId(); //get key
    inquiry_data.insert(std::make_pair(key,data));

    //then, pass the updated data to listener
    //pass the trade data to listeners
    std::cout<<"data goes from BondHistoricalInquiryService -> listener."<<'\n'<<std::endl;
    for(auto& l: listeners){
        l->ProcessAdd(data);
    }
}

void BondHistoricalInquiryService::AddListener(ServiceListener<Inquiry<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<Inquiry<Bond>>* >& BondHistoricalInquiryService::GetListeners() const
{
    return listeners;
}

void BondHistoricalInquiryService::PersistData(string persistKey, Inquiry<Bond>& data)
{
    bond_his_inquiry_connector->Publish(data);
}



//define member functions in class BondHistoricalInquiryServiceListener
void BondHistoricalInquiryServiceListener::ProcessAdd(Inquiry<Bond> &data)
{
    bond_his_inquiry_service->OnMessage(data);
    bond_his_inquiry_service->PersistData(data.GetProduct().GetProductId(),data);
}

void BondHistoricalInquiryServiceListener::ProcessRemove(Inquiry<Bond> &data)  {};

void BondHistoricalInquiryServiceListener::ProcessUpdate(Inquiry<Bond> &data)  {};

#endif