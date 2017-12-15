/**algostreamingservice.hpp
 * Defines the data types and Service for bond streaming
 * @author: Sijia Zhang
 */

#ifndef ALGO_STREAMING_SERVICE_HPP
#define ALGO_STREAMING_SERVICE_HPP

#include "streamingservice.h"
#include "products.h"
#include "pricingservice.h"


class AlgoStream{

private:

    //define Price Stream in order to send the bid/offer prices to the BondStreamingService
    PriceStream<Bond> price_stream;

public:

    //destructor
    ~AlgoStream(){};

    //ctor
    AlgoStream(){};

    //ctor
    AlgoStream(const Price<Bond>& pri);

    // Price stream order choosing is using algorithm to choose price order
    void PriceChoosing(const Price<Bond>& pri);

    // Get price stream
    PriceStream<Bond> GetPriceStream() const;
};

/**
 * AlgoStreamingService is aimed to send the bid/offer prices to the BondStreamingService
 * We use type Bond instead of using template T
 */

class AlgoStreamingService : public Service<std::string, AlgoStream>{

private:

    //define listener
    std::vector<ServiceListener<AlgoStream>*> listeners;

    //define a map to find data on the service
    std::map<std::string, AlgoStream> algo_stream_data;

    //ctor
    AlgoStreamingService(){};

public:

    // Generate instance
    static AlgoStreamingService* Generate_Instance(){
        //we define this function to generate instance for class AlgoStreamingService.
        static AlgoStreamingService ins;
        return &ins;
    }

    // pure virtual member functions in class Service.
    // Get data on our service given a key

    AlgoStream& GetData(std::string key) ;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoStream &data) ;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<AlgoStream> *listener) ;

    // Get all listeners on the Service.
    const std::vector< ServiceListener<AlgoStream>* >& GetListeners() const ;

    // Add price streams to the service
    void AddPrice(Price<Bond>& ob);
};


/**
 * AlgoStreamingServiceListener passes the bid/offer price to BondStreamingService
 * We use the type Bond instead of using template T.
 */

class AlgoStreamingServiceListener : public ServiceListener<Price<Bond>>{

private:

    //define a pointer of AlgoStreamingService
    AlgoStreamingService * algo_stream_service;

    //ctor
    AlgoStreamingServiceListener(){
        algo_stream_service=AlgoStreamingService::Generate_Instance();
    }

public:

    // Generate instance
    static AlgoStreamingServiceListener* Generate_Instance(){
        //we define this function to generate instance for class AlgoStreamingServiceListener.
        static AlgoStreamingServiceListener ins;
        return &ins;
    }

    //overide the pure virtual functions in class: ServiceListener
    // Listener callback to process an add event to the Service
    void ProcessAdd(Price<Bond> &data) ;

    // Listener callback to process a remove event to the Service
    void ProcessRemove(Price<Bond> &data) ;

    // Listener callback to process an update event to the Service
    void ProcessUpdate(Price<Bond> &data) ;

    // return  algo streaming service
    AlgoStreamingService* GetAlgoStreamingService();

};


/*****************************************************************************************/
//define member functions in class: AlgoStream
AlgoStream::AlgoStream(const Price<Bond>& pri)
{
    //we need to create the input parameters for pricestream
    //pricestreamorder: bid
    double bid=pri.GetMid()-pri.GetBidOfferSpread()/2.; //bid price
    //visible_quantity
    long bid_vq=1000000; //initialize
    //hidden_quantity
    long bid_hq=2000000; //initialize
    PriceStreamOrder pso_bid(bid,bid_vq,bid_hq,BID);

    //pricestreamorder: OFFER
    double offer=pri.GetMid()+pri.GetBidOfferSpread()/2.; //offer price
    //visible_quantity
    long offer_vq=1000000; //initialize
    //hidden_quantity
    long offer_hq=2000000; //initialize
    PriceStreamOrder pso_offer(offer,offer_vq,offer_hq,OFFER);

    //create Price Stream
    Bond product=pri.GetProduct();
    PriceStream<Bond> ps(product,pso_bid,pso_offer);

    //assign ps to price_stream
    price_stream=ps;
}

void AlgoStream::PriceChoosing(const Price<Bond>& pri)
{
    //Actually, There is no specific algorithm to choose price, since we just need to send the bid/offer prices to the BondStreamingService
    //pricestreamorder: bid
    double bid=pri.GetMid()-pri.GetBidOfferSpread()/2.; //bid price
    //visible_quantity
    long bid_vq=(1+rand()%2)*1000000; //changing quantity of both visible and hidden to get some change
    //hidden_quantity
    long bid_hq=bid_vq*2;
    PriceStreamOrder pso_bid(bid,bid_vq,bid_hq,BID);

    //pricestreamorder: OFFER
    double offer=pri.GetMid()+pri.GetBidOfferSpread()/2.; //offer price
    //visible_quantity
    long offer_vq=(1+rand()%2)*1000000;
    //hidden_quantity
    long offer_hq=offer_vq*2;
    PriceStreamOrder pso_offer(offer,offer_vq,offer_hq,OFFER);

    //create Price Stream
    Bond product=pri.GetProduct();
    PriceStream<Bond> ps(product,pso_bid,pso_offer);

    //assign ps to price_stream
    price_stream=ps;
}

PriceStream<Bond> AlgoStream::GetPriceStream() const
{
    return price_stream;
}



//define member functions in class: AlgoStreamingService
AlgoStream& AlgoStreamingService::GetData(std::string key)
{
    return algo_stream_data.at(key);
}

void AlgoStreamingService::OnMessage(AlgoStream &data)
{
    // no implementation
}

void AlgoStreamingService::AddListener(ServiceListener<AlgoStream> *listener)
{
    listeners.push_back(listener);
}

const std::vector< ServiceListener<AlgoStream>* >& AlgoStreamingService::GetListeners() const
{
    return listeners;
}

// Add price streams to the service
void AlgoStreamingService::AddPrice(Price<Bond>& ob)
{
    //firstly, making the stream price stored
    std::string productId=ob.GetProduct().GetProductId();

    //store the order
    if(algo_stream_data.find(productId)!=algo_stream_data.end()){
        //if we can find the key of algo_stream_data, then just choose the order which has smallest spread
        algo_stream_data[productId].PriceChoosing(ob);
    }
    else{
        //make a new pair
        AlgoStream new_algo(ob);
        algo_stream_data.insert(std::make_pair(productId,new_algo));
    }

    //pass the algo streaming data to listeners
    std::cout<<"data goes from AlgoStreamingService -> listener."<<std::endl;
    AlgoStream as=algo_stream_data[productId];
    for(auto& l: listeners){
        l->ProcessAdd(as);
    }
}



//define member functions in class: AlgoStreamingServiceListener
void AlgoStreamingServiceListener::ProcessAdd(Price<Bond> &data)
{
    algo_stream_service->AddPrice(data);
}

void AlgoStreamingServiceListener::ProcessRemove(Price<Bond> &data)
{
    // no implementation
}

void AlgoStreamingServiceListener::ProcessUpdate(Price<Bond> &data)
{
    // no implementation
}

AlgoStreamingService* AlgoStreamingServiceListener::GetAlgoStreamingService()
{
    return algo_stream_service;
}

#endif
