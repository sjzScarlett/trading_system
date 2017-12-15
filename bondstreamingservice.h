/**bondstreamingservice.hpp
 * Defines the data types and Service for bond streaming
 * @author: Sijia Zhang
 */

#ifndef BOND_STREAMING_SERVICE_HPP
#define BOND_STREAMING_SERVICE_HPP

#include "streamingservice.h"
#include "products.h"
#include "algostreamingservice.h"


/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingService : public Service<string,PriceStream <T> >
{

public:

    // Publish two-way prices
    virtual void PublishPrice(const PriceStream<T>& priceStream) = 0;

};

/**
 * Bond Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * We use type Bond instead of using template T.
 */

class BondStreamingService : public StreamingService<Bond>{

private:

    //define listener
    std::vector<ServiceListener<PriceStream<Bond>>*> listeners;

    //define a map to find data on the service
    std::map<std::string, PriceStream<Bond>> stream_data;

    //ctor
    BondStreamingService(){};

public:

    // Generate instance
    static BondStreamingService* Generate_Instance(){
        //we define this function to generate instance for class BondStreamingService.
        static BondStreamingService ins;
        return &ins;
    }

    // pure virtual member functions in class Service.
    // Get data on our service given a key
    PriceStream<Bond>& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PriceStream<Bond> &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PriceStream<Bond>> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<PriceStream<Bond>>* >& GetListeners() const ;

    // Add Algo Stream to the service
    void AddAlgoStream(AlgoStream& ob);

    void PublishPrice(const PriceStream<Bond>& priceStream)  ;
};


/**
 * BondStreamingServiceListener passes the data from streaming service
 * We use the type Bond instead of using template T.
 */

class BondStreamingServiceListener : public ServiceListener<AlgoStream>{

private:

    //define a pointer of AlgoStreamingService
    BondStreamingService * bond_stream_service;

    //ctor
    BondStreamingServiceListener(){
        bond_stream_service=BondStreamingService::Generate_Instance();
    }

public:

    // Generate instance
    static BondStreamingServiceListener* Generate_Instance(){
        //we define this function to generate instance for class BondStreamingServiceListener.
        static BondStreamingServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(AlgoStream &data) ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(AlgoStream&data) ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(AlgoStream&data) ;

    // return position service
    BondStreamingService* GetBondStreamingService();

};



//define member functions in class: BondStreamingService
PriceStream<Bond>& BondStreamingService::GetData(std::string key)
{
    return stream_data.at(key);
}

void BondStreamingService::OnMessage(PriceStream<Bond> &data)
{
    // no implementation
}

void BondStreamingService::AddListener(ServiceListener<PriceStream<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<PriceStream<Bond>>* >& BondStreamingService::GetListeners() const
{
    return listeners;
}

void BondStreamingService::AddAlgoStream(AlgoStream& ob)
{
    //firstly, making the AlgoStream stored
    std::string productID=ob.GetPriceStream().GetProduct().GetProductId();

    //store the AlgoStream
    stream_data[productID]=ob.GetPriceStream();

    //pass the streaming data to listeners
    std::cout<<"data goes from BondStreamingService -> listener."<<std::endl;
    auto val=ob.GetPriceStream();
    for(auto& l: listeners){
        l->ProcessAdd(val);
    }
}

void BondStreamingService::PublishPrice(const PriceStream<Bond>& priceStream)
{
    //get bid and offer price
    double price_bid=priceStream.GetBidOrder().GetPrice();
    double price_offer=priceStream.GetOfferOrder().GetPrice();

    //Then, making the priceStream stored
    std::string productID=priceStream.GetProduct().GetProductId();

    //store the priceStream
    PriceStream<Bond> new_stream(priceStream);
    stream_data.insert(std::make_pair(productID, new_stream));

    //pass the streaming data to listeners
    std::cout<<"data goes from BondStreamingService -> listener."<<std::endl;
    PriceStream<Bond> ps=stream_data[productID];
    for(auto& l: listeners){
        l->ProcessAdd(ps);
    }

}



//define member functions in class BondStreamingServiceListener
void BondStreamingServiceListener::ProcessAdd(AlgoStream &data)
{
    bond_stream_service->AddAlgoStream(data);

//    //Also add pricestream to listener
//    PriceStream<Bond> ps=data.GetPriceStream();
//    bond_stream_service->PublishPrice(ps);
}

void BondStreamingServiceListener::ProcessRemove(AlgoStream&data)
{
    // no implementation
}

void BondStreamingServiceListener::ProcessUpdate(AlgoStream&data)
{
    // no implementation
}

// return bond streaming service
BondStreamingService* BondStreamingServiceListener::GetBondStreamingService()
{
    return bond_stream_service;
}
#endif