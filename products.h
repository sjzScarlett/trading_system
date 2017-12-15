/**
 * products.hpp
 * Defines Bond products.
 *
 * @author Breman Thuraisingham, Sijia Zhang
 */
#ifndef PRODUCTS_HPP
#define PRODUCTS_HPP

#include <iostream>
#include <string>
#include <map>

#include "boost/date_time/gregorian/gregorian.hpp"

using namespace std;
using namespace boost::gregorian;

enum ProductType { IRSWAP, BOND };

/**
 * Base class for a product.
 */
class Product
{

public:

    // ctor for a prduct
    Product(string _productId, ProductType _productType);

    // Get the product identifier
    const string& GetProductId() const;

    // Ge the product type
    ProductType GetProductType() const;

private:
    string productId;
    ProductType productType;

};


enum BondIdType { CUSIP, ISIN };

/**
 * Bond product class
 */
class Bond : public Product
{

public:

    // ctor for a bond
    Bond(string _productId, BondIdType _bondIdType, string _ticker, float _coupon, date _maturityDate);
    Bond();

    // Get the ticker
    const string& GetTicker() const;

    // Get the coupon
    float GetCoupon() const;

    // Get the maturity date
    const date& GetMaturityDate() const;

    // Get the bond identifier type
    BondIdType GetBondIdType() const;

    // Print the bond
    friend ostream& operator<<(ostream &output, const Bond &bond);

private:
    string productId;
    BondIdType bondIdType;
    string ticker;
    float coupon;
    date maturityDate;

};

/**
 * Base class for bondService
 * Pure Virtual member function GetData
 * V is type for return data, K is type for input key
 */
template<typename K, typename V>
class BondService{

public:

    virtual V& GetData(K key) = 0;

};


/**
 * BondProductService is working as adding data, getting data, etc.
 * Using Bond as type instead of V and K
 */
class BondProductService: public BondService<std::string, Bond>{

private:

    //define a map to cache of bond products
    std::map<std::string, Bond> bond_map;


    // ctor
    BondProductService(){
        bond_map=map<std::string, Bond>();
    }

public:

    // Generator Instance
    static BondProductService* Generate_Instance(){
        static BondProductService ins;
        return &ins;
    }

    // Add a bond to the service
    void AddBond(Bond& bond);

    // GetData
    Bond& GetData(std::string key);

    // GetBonds gets all bonds with specific ticker
    std::vector<Bond> GetBonds(const std::string& ticker) const;
};



//define member functions in class Product
Product::Product(string _productId, ProductType _productType)
{
    productId = _productId;
    productType = _productType;
}

const string& Product::GetProductId() const
{
    return productId;
}

ProductType Product::GetProductType() const
{
    return productType;
}


//define member functions in class Bond
Bond::Bond(string _productId, BondIdType _bondIdType, string _ticker, float _coupon, date _maturityDate) : Product(_productId, BOND)
{
    bondIdType = _bondIdType;
    ticker = _ticker;
    coupon = _coupon;
    maturityDate =_maturityDate;
}

Bond::Bond() : Product("0", BOND)
{
}

const string& Bond::GetTicker() const
{
    return ticker;
}

float Bond::GetCoupon() const
{
    return coupon;
}

const date& Bond::GetMaturityDate() const
{
    return maturityDate;
}

BondIdType Bond::GetBondIdType() const
{
    return bondIdType;
}

ostream& operator<<(ostream &output, const Bond &bond)
{
    output << bond.ticker << " " << bond.coupon << " " << bond.GetMaturityDate();
    return output;
}

//define member funcions in class BondProductService
void BondProductService::AddBond(Bond &bond)
{
    bond_map.insert(std::make_pair(bond.GetProductId(), bond));
}

Bond& BondProductService::GetData(std::string key)
{
    return bond_map[key];
}

std::vector<Bond> BondProductService::GetBonds(const std::string& ticker) const
{
    //define a container to contain bond for specific requirment
    std::vector<Bond> container;

    for(auto i:bond_map){
        if(i.second.GetTicker()==ticker){
            container.push_back(i.second);
        }
    }

    return container;
}

#endif