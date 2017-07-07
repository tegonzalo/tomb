/********************************/
/* TOMB: Tool of Model Building */
/********************************/
/*
* \file
* linkedlists.h
*
* \author
* T. Gonzalo (t.e.gonzalo@fys.uio.no)
*
* \date
* 20/04/2012
*/

#ifndef __LINKEDLISTS_H
#define __LINKEDLISTS_H

#include <iostream>
#include <sstream>
#include "libjson.h"
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

/***************************/
/* Class List declarations */
/***************************/

namespace Tomb 
{
  template <class TYPE> class List : public std::vector<TYPE>
  {

    public:
      List();
      List(const TYPE &);
      List(const TYPE *, const TYPE *);
      List(const List<TYPE> &);
      List(const std::vector<TYPE> &);
      List(const JSONNode &);
      List(const JSONNode &, const void *);
      List(List<TYPE> &&);

      ~List();
      List<TYPE> &operator=(const List<TYPE> &);
      List<TYPE> &operator=(List<TYPE> &&);
      
      int nterms() const;
      TYPE &FirstTerm() const;
      TYPE &GetObject(int) const;
      TYPE &operator[](int) const;
      int AddTerm(const TYPE &);
      int AddTermOrdered(const TYPE &, const std::string = "DESC");
      void InsertTerm(int, const TYPE &);
      void DeleteTerm(int);
      int DeleteObject(const TYPE &, int = 0);
      void SwapTerms(int, int);
      int EliminateRepeated();
      bool Empty() const;
      void Clear();
      void AppendList(const List &);
      void Order(std::string = "DESC");
      void Reverse();
      int Index(const TYPE &) const;
      List<List<TYPE> > Subsets(int = 0);
      std::string Print() const;
      
      bool operator==(const List<TYPE> &) const;
      bool operator!=(const List<TYPE> &) const;
      bool operator<(const List<TYPE> &) const;
      bool operator>(const List<TYPE> &) const;
      
      JSONNode json(std::string = "") const;
      void ParseJSON(const JSONNode &);
      void ParseJSON(const JSONNode &, const void *);
  };

  template <class TYPE> std::ostream &operator<<(std::ostream &, const List<TYPE> &);
}

/******************************/
/* Class Product declarations */
/******************************/

namespace Tomb
{
  template <class TYPE> class Product : public List<TYPE>
  {
    public:
      Product();
      Product(const TYPE &);
      Product(const Product<TYPE> &);
      Product(Product<TYPE> &&);
      ~Product();
      Product<TYPE> &operator=(const Product<TYPE> &);
      Product<TYPE> &operator=(Product<TYPE> &&);
      
      std::string Print() const;
      //std::string Table() const;
  };

  template <class TYPE> std::ostream &operator<<(std::ostream &, const Product<TYPE> &);
  template <class TYPE> List< Product<TYPE> > DistributeProduct(List<TYPE>, TYPE);
  template <class TYPE> List< Product<TYPE> > DistributeProduct(List< List<TYPE> >);
}

/**************************/
/* Class Sum declarations */
/**************************/

namespace Tomb
{
  template <class TYPE> class Sum : public List<TYPE>
  {
    public:
      Sum();
      Sum(const TYPE &);
      Sum(const Sum<TYPE> &);
      Sum(Sum<TYPE> &&);
      ~Sum();
      Sum &operator=(const Sum<TYPE> &);
      Sum &operator=(Sum<TYPE> &&);
      
      std::string Print() const;
      //std::string Table() const;

  };

  template <class TYPE> std::ostream &operator<<(std::ostream &, const Sum<TYPE> &);
}

/**************************/
/* Class List definitions */
/**************************/

namespace Tomb
{
  /* Empty constructor */
  template <class TYPE> List<TYPE>::List() : std::vector<TYPE>()
  {
    
  }

  /* Constructor with object */
  template <class TYPE> List<TYPE>::List(const TYPE &Object) : std::vector<TYPE>(1,Object)
  {
    
  }
  
  /* Constructor with an iterator of objects */
  template <class TYPE> List<TYPE>::List(const TYPE *first, const TYPE *last) : std::vector<TYPE>(first, last)
  {
    
  }

  /* Copy constructor 0 */
  template <class TYPE> List<TYPE>::List(const List<TYPE> &otherList) : std::vector<TYPE>(otherList)
  {

  }
  
  /* Copy constructor 1 */
  template <class TYPE> List<TYPE>::List(const std::vector<TYPE> &otherList) : std::vector<TYPE>(otherList)
  {

  }
  
  /* Copy constructor 2.0 */
  template <class TYPE> List<TYPE>::List(const JSONNode &json)
  {
    if(std::is_same<TYPE, std::string>::value)
      for(auto i=json.begin(); i!=json.end(); i++)
        AddTerm(TYPE(i->as_string()));
    else
      ParseJSON(json);
  }


  /* Copy constructor 2 */
  template <class TYPE> List<TYPE>::List(const JSONNode &json, const void *pointer)
  {
    if(std::is_same<TYPE, std::string>::value)
      for(auto i=json.begin(); i!=json.end(); i++)
        AddTerm(TYPE(i->as_string()));
    else
    {
      if(pointer == NULL)
        ParseJSON(json);
      else
    	ParseJSON(json, pointer);
    }
  }

  /* Move constructor */
  template <class TYPE> List<TYPE>::List(List<TYPE> &&otherList) : std::vector<TYPE>(std::move(otherList))
  {
  }
  
  /* Class destructor */
  template <class TYPE> List<TYPE>::~List()
  {
    this->Clear();
  }

  /* Overloaded = operator */
  template <class TYPE> List<TYPE> &List<TYPE>::operator=(const List<TYPE> &otherList) 
  {
    try
    {
      if(this == &otherList) return *this;
      std::vector<TYPE>::operator=(otherList);
      return *this;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Move = operator */
  template <class TYPE> List<TYPE> &List<TYPE>::operator=(List<TYPE> &&otherList) 
  {
    try
    {
      if(this == &otherList) return *this;
      std::vector<TYPE>::operator=(std::move(otherList));
      return *this;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Returns the number of elements */
  template <class TYPE> int List<TYPE>::nterms() const
  {
    try
    {
      return this->size();
    }
    catch (...) 
    {
      throw;
    }
  }

  /* Return the first element of the list */
  template <class TYPE> TYPE &List<TYPE>::FirstTerm() const
  {
    try
    {
      return (TYPE &)*this->begin();
    }
    catch (...)
    {
      throw;
    }
  }

  /* Returns the ith object of the List */
  template <class TYPE> TYPE &List<TYPE>::GetObject(int i) const
  {
    try
    {
      if(i >= this->nterms()) 
        throw "List::GetObject::Element doesn\'t exist";
      if(i < 0) 
        return this->GetObject(this->size()+i);
      return (TYPE &) std::vector<TYPE>::operator[](i);
    }
    catch (...)
    {
      throw;
    }
  }
  
  /* Returns the ith object of the List */
  template <class TYPE> TYPE &List<TYPE>::operator[](int i) const
  {
    try
    {	if(i >= this->nterms()) 
        throw "List::operator[]::Element doesn\'t exist";
      if(i < 0)
        return (*this)[nterms()+i];
      else
        return (TYPE &) std::vector<TYPE>::operator[](i);
    } catch (...)
    {
      throw;
    }
  }

  /* Adds a new term to the List */
  template <class TYPE> int List<TYPE>::AddTerm(const TYPE &Object)
  {
    try
    {
      this->push_back(Object);
      return nterms()-1;
    }
    catch (...)
    {
      throw;
    }
  }

  template <class TYPE> int List<TYPE>::AddTermOrdered(const TYPE &Object, const std::string order)
  {
    try
    {
      typename std::vector<TYPE>::iterator where;
      if(!order.compare("DESC"))
        where = this->insert(std::upper_bound(this->rbegin(), this->rend(), Object).base(), Object);
      else if(!order.compare("ASC")) 
        where = this->insert(std::upper_bound(this->begin(), this->end(), Object), Object);
      else
        throw "List::Order::Ordering must be ASC or DESC";

      return distance(this->begin(), where);
    }
    catch (...)
    {
      throw;
    }
  }

  /* Inserts a new term in a specific position in the List */
  template <class TYPE> void List<TYPE>::InsertTerm(int i, const TYPE &Object)
  {
    try
    {
      if(i > this->nterms()) 
        throw "Element doesn\'t exist";
      if(i < 0) 
        this->InsertTerm(this->size()+i, Object);
      else if (i == nterms())
        this->AddTerm(Object);
      else
        this->insert(this->begin()+i, Object);
    }
    catch (...)
    {
      throw;
    }
  }

  /* Deletes a term of the List by position */
  template <class TYPE> void List<TYPE>::DeleteTerm(int i)
  {
    try
    {
      if(i >= this->nterms()) 
        throw "List::DeleteTerm::Element doesn\'t exist";
      if(i < 0) 
        this->DeleteTerm(this->size()+i);
      else
        this->erase(this->begin()+i);
    }
    catch (...)
    {
      throw;
    }
  }

  /* Deletes a term of the List by term */
  template <class TYPE> int List<TYPE>::DeleteObject(const TYPE &Object, int times)
  {
    try
    {
      int nerased = 0;
      
      typename List<TYPE>::iterator it;
      while((it = std::find(this->begin(), this->end(), Object)) != this->end()) {
        this->erase(it);
        nerased ++;
        if(times and nerased >= times) return nerased;
      }
      
      return nerased;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Swaps the objects of two terms of a list by index */
  template <class TYPE> void List<TYPE>::SwapTerms(int i, int j)
  {
    try
    {
      if(i >= this->nterms() or j >= this->nterms())
        throw "List::SwapTerms::Element doesn\'t exist";
      if(i < 0) 
        this->SwapTerms(this->size()-i, j);
      else if(j < 0)
        this->SwapTerms(i, this->size()-j);
      else 
        std::iter_swap(this->begin() + i, this->begin() + j);
    } 
    catch (...)
    {
      throw;
    }
  }

  /* Eliminates repeated terms */
  template <class TYPE> int List<TYPE>::EliminateRepeated()
  {
    try
    {
      List<TYPE> auxList;
      int nerased = 0;
      while(this->nterms())
      {
        TYPE Object = this->GetObject(0);
        auxList.AddTerm(Object);
        nerased += this->DeleteObject(Object);
        nerased--;
      }
      *this = auxList;
      
      return nerased;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Check if list is empty */
  template <class TYPE> bool List<TYPE>::Empty() const
  {
    try
    {
      return this->empty();
    }
    catch (...)
    {
      throw;
    }
  }

  /* Empty the list */
  template <class TYPE> void List<TYPE>::Clear()
  {
    try
    {
      this->clear();
    }
    catch (...)
    {
      throw;
    }
  }

  /* Append a List to the end of this List */
  template <class TYPE> void List<TYPE>::AppendList(const List &otherList)
  {
    try
    {
      this->insert(this->end(), otherList.begin(), otherList.end());
    }
    catch (...)
    {
      throw;
    }
  }

  /* Order the list in a specified order */
  template <class TYPE> void List<TYPE>::Order(std::string order)
  {
    try
    {
      if(!order.compare("DESC"))
      {
        std::sort(this->begin(), this->end());
        std::reverse(this->begin(), this->end());
      }
      else if(!order.compare("ASC")) {
        std::sort(this->begin(), this->end());
      }else
        throw "List::Order::Ordering must be ASC or DESC";
    } 
    catch (...)
    {
      throw;
    }
  }

  /* Reverses the order of the list */
  template <class TYPE> void List<TYPE>::Reverse()
  {
    try
    {
      std::reverse(this->begin(), this->end());
    }
    catch (...)
    {
      throw;
    }
  }

  /* Returns the index of an object if it is in the list and -1 if it is not*/
  template <class TYPE> int List<TYPE>::Index(const TYPE &Object) const
  {
    try
    {
      int pos = std::find(this->begin(), this->end(), Object) - this->begin();
      if(pos < this->size())
        return pos;
      else
        return -1;
    }
    catch (...)
    {
      throw;
    }
  }
  
  /* Creates all possible subsets of the list */
  template <class TYPE> List<List<TYPE> > List<TYPE>::Subsets(int k)
  {
    try
    {
      List<List<TYPE> > subsets;
      
      int n = k;
      if(!n)
        n = nterms();
      
      std::vector<bool> bit_mask(nterms());
      bool next_bit_mask = false;
      do
      {

        if(std::count(bit_mask.begin(), bit_mask.end(), true) <= n)
        {
          subsets.AddTerm(List<TYPE>());
          for(int i=0; i!=bit_mask.size(); i++)
            if(bit_mask[i])
              subsets[subsets.nterms()-1].AddTerm((*this)[i]);
        }
        
        // next_bitmask
        std::size_t i = 0 ;
        for( ; ( i < bit_mask.size() ) && (bit_mask[i] or std::count(bit_mask.begin(),bit_mask.end(),true) >=n); ++i )
        {
          bit_mask[i] = false;
        }

        if( i < bit_mask.size())
        {
          if(std::count(bit_mask.begin(), bit_mask.end(), true) < n)
              bit_mask[i] = true;
          next_bit_mask = 	true;
          
        }
        else 
          next_bit_mask = false ;
      }
      while(next_bit_mask);
      
      
      return subsets;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Prints the list */
  template <class TYPE> std::string List<TYPE>::Print() const
  {
    try
    {
      std::stringstream stream;
      stream << (List<TYPE> &)*this;
      return stream.str();
    } 
    catch (...)
    {
      throw;
    }
  }
  
  /* Overloaded == operator */
  template <class TYPE> bool List<TYPE>::operator==(const List<TYPE> &list) const
  {
    return (std::vector<TYPE>(*this) == std::vector<TYPE>(list));
  }
  
  /* Overloaded != operator */
  template <class TYPE> bool List<TYPE>::operator!=(const List<TYPE> &list) const
  {
    return !(*this == list);
  }
  
  /* Overloaded < operator */
  template <class TYPE> bool List<TYPE>::operator<(const List<TYPE> &list) const
  {
    return (nterms() < list.nterms());
  }
  
  /* Overloaded > operator */
  template <class TYPE> bool List<TYPE>::operator>(const List<TYPE> &list) const
  {
    return (nterms() > list.nterms());
  }

  /* Prints to json format */
  template <class TYPE> JSONNode List<TYPE>::json(std::string name) const
  {
    
    try
    {
      JSONNode JSONList(JSON_ARRAY);
      if(name != "") {
        JSONList.set_name(name);
      }
      
      for(typename List<TYPE>::const_iterator it = this->begin(); it != this->end(); it++) {
        JSONList.push_back(it->json("id"));
      }
    
      return JSONList;
    
    }
    catch (...)
    {
      throw;
    }
  }

  /* Parses a json object into the attributes of the class */
  template <class TYPE> void List<TYPE>::ParseJSON(const JSONNode & n)
  { 
    try
    {
      if(this->nterms())
        this->Clear();
      
      JSONNode::const_iterator it = n.begin();

      while(it != n.end())
      {
        if(it->as_string() != "" or std::is_same<TYPE, std::string>::value)
        {
          TYPE Object(it->as_string());
          this->AddTerm(Object);
        } 
        else
        {
          TYPE Object(*it);
          this->AddTerm(Object);
        }
        ++it;
        
      }
      
      
    } catch (...) {
      throw;
    }
  }


  /* Parses a json object into the attributes of the class */
  template <class TYPE> void List<TYPE>::ParseJSON(const JSONNode & n, const void *pointer)
  { 
    try
    {
      if(this->nterms())
        this->Clear();
      
      JSONNode::const_iterator it = n.begin();

      while(it != n.end())
      {
        if(it->as_string() != "" or std::is_same<TYPE, std::string>::value)
        {
          TYPE Object(it->as_string());
          this->AddTerm(Object);
        } 
        else
        {
          if(pointer == NULL)
          {
            TYPE Object(*it);
            this->AddTerm(Object);
          }
          else
          {
            TYPE Object(*it, pointer);
            this->AddTerm(Object);
          }
        }
        ++it;
        
      }
      
      
    } catch (...) {
      throw;
    }
  }

  /* Overloaded << operator with lists */
  template <class TYPE> std::ostream &operator<<(std::ostream &stream, const List<TYPE> &aList)
  {
    try
    {
      stream << "{";
      for (typename List<TYPE>::const_iterator it = aList.begin(); it != aList.end(); it++)
      {
        stream << *it;
        if(std::next(it) != aList.end()) 
          stream << ", ";
      }
      stream << "}";
      return stream;
    } 
    catch (...)
    {
      throw;
    }
  }
}

/*****************************/
/* Class Product definitions */
/*****************************/

namespace Tomb
{
  /* Constructor of a product */
  template <class TYPE> Product<TYPE>::Product() : List<TYPE>()
  {

  }

  /* Constructor of a product with a first term */
  template <class TYPE> Product<TYPE>::Product(const TYPE &Object) : List<TYPE>(Object)
  {

  }

  /* Copy constructor */
  template <class TYPE> Product<TYPE>::Product(const Product<TYPE> &Prod) : List<TYPE>(Prod)
  {

  }

  /* Move constructor */
  template <class TYPE> Product<TYPE>::Product(Product<TYPE> &&Prod) : List<TYPE>(std::move(Prod))
  {

  }

  /* Destructor */
  template <class TYPE> Product<TYPE>::~Product()
  {
    
  }

  /* Overloaded = operator */
  template <class TYPE> Product<TYPE> &Product<TYPE>::operator=(const Product<TYPE> &Prod)
  {
    try {
      if(this == &Prod) return *this;
      List<TYPE>::operator=(Prod);
      return *this;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Move = operator */
  template <class TYPE> Product<TYPE> &Product<TYPE>::operator=(Product<TYPE> &&Prod)
  {
    try {
      if(this == &Prod) return *this;
      List<TYPE>::operator=(std::move(Prod));
      return *this;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Prints the product */
  template <class TYPE> std::string Product<TYPE>::Print() const
  {
    try
    {
      std::stringstream stream;
      stream << (Product<TYPE> &)*this;
      return stream.str();
    }
    catch (...)
    {
      throw;
    }
  }

  /* Overloaded << operator with products */
  template <class TYPE> std::ostream &operator<<(std::ostream &stream, const Product<TYPE> &prod)
  {
    try
    {
      stream << "";
      for (typename Product<TYPE>::const_iterator it = prod.begin(); it != prod.end(); it++)
      {
        stream << (TYPE &)*it;
        if(std::next(it) != prod.end())
          stream << "x";
      }
      return stream;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Distribute the product throughout a list */
  template <class TYPE> List< Product<TYPE> > DistributeProduct(List<TYPE> list, TYPE obj)
  {
    try
    {
      List< Product<TYPE> > ResultsList;
      for(typename List<TYPE>::iterator it = list.begin(); it != list.end(); it++)
      {
        Product<TYPE> AuxProduct;
        AuxProduct.AddTerm(*it);
        AuxProduct.AddTerm(obj);
        ResultsList.AddTerm(AuxProduct);
      }
      return ResultsList;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Distribute the product throughout a list of lists */
  template <class TYPE> List< Product<TYPE> > DistributeProduct(List< List<TYPE> > list)
  {
    try
    {
      List< Product<TYPE> > ResultsList;
      int totalterms = 1;
      for(typename List< List<TYPE> >::iterator it = list.begin(); it != list.end(); it++)
        totalterms *= it->nterms();
      for(int i=0; i<totalterms; i++)
      {
        Product<TYPE> AuxProduct;
        int terms = 1;
        for(typename List< List<TYPE> >::iterator it = list.begin(); it != list.end(); it++)
        {
          int thisterms = it->nterms();
          terms *= thisterms;
          int index = (i*terms/totalterms)%thisterms;
          AuxProduct.AddTerm(it->GetObject(index));
        }
        ResultsList.AddTerm(AuxProduct);
      }
      return ResultsList;
      
    }
    catch (...)
    {
      throw;
    }
  }
}

/*************************/
/* Class Sum definitions */
/*************************/

namespace Tomb
{

  /* Constructor of a sum */
  template <class TYPE> Sum<TYPE>::Sum() : List<TYPE>()
  {

  }

  /* Constructor of a sum with a first term */
  template <class TYPE> Sum<TYPE>::Sum(const TYPE &Object) : List<TYPE>(Object)
  {

  }

  /* Copy constructor */
  template <class TYPE> Sum<TYPE>::Sum(const Sum<TYPE> &sum) : List<TYPE>(sum)
  {

  }

  /* Move constructor */
  template <class TYPE> Sum<TYPE>::Sum(Sum<TYPE> &&sum) : List<TYPE>(std::move(sum))
  {
    
  }

  /* Destructor */
  template <class TYPE> Sum<TYPE>::~Sum()
  {

  }

  /* Overloaded = operator */
  template <class TYPE> Sum<TYPE> &Sum<TYPE>::operator=(const Sum<TYPE> &sum)
  {
    try
    {
      if(this == &sum) return *this;
      List<TYPE>::operator=(sum);
      return *this;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Move = operator */
  template <class TYPE> Sum<TYPE> &Sum<TYPE>::operator=(Sum<TYPE> &&sum)
  {
    try
    {
      if(this==&sum) return *this;
      List<TYPE>::operator=(std::move(sum));
      return *this;
    }
    catch (...)
    {
      throw;
    }
  }

  /* Prints the sum */
  template <class TYPE> std::string Sum<TYPE>::Print() const
  {
    try
    {
      std::stringstream stream;
      stream << (Sum<TYPE> &)*this;
      return stream.str();
    } 
    catch (...)
    {
      throw;
    }
  }

  /* Overloaded << operator with sums */
  template <class TYPE> std::ostream &operator<<(std::ostream &stream, const Sum<TYPE> &sum)
  {
    try
    {
      stream << "";
      for (typename Sum<TYPE>::const_iterator it = sum.begin(); it != sum.end(); it++)
      {
        stream << (TYPE &)*it;
        if(std::next(it) != sum.end())
          stream << "+";
      }
      return stream;
    }
    catch (...)
    {
      throw;
    }
  }
}
#endif /*  __LINKEDLISTS_H */
