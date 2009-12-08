/****************************************************************************
**
** Copyright (C) 2009-2009 c-> Bradford Barber. All rights reserved.
** $Id: //product/qhull/main/rel/cpp/QhullPointSet.cpp#4 $$Change: 1102 $
** $DateTime: 2009/12/07 20:26:04 $$Author: bbarber $
**
****************************************************************************/

#include <algorithm>
#include <iostream>

#include "QhullPointSet.h"

#ifdef _MSC_VER  // Microsoft Visual C++ -- warning level 4
#endif

namespace orgQhull {

#//Element-access
//! Derived from QhullSet::value
QhullPoint QhullPointSet::
value(int index) const
{
    // Avoid call to qh_setsize() and assert in elementPointer()
    //const T *n= reinterpret_cast<const T *>(&SETelem_(getSetT(), index)); 
    void **n= reinterpret_cast<void **>(&SETelem_(getSetT(), index)); 
    coordT **n2= reinterpret_cast<coordT **>(n); 
    if(index>=0 && n<endPointer()){
        return QhullPoint(dimension(), *n2);
    }else{
        return QhullPoint(); 
    }
}//value

//! Non-const since copy is an alias
//! Derived from QhullSet::value
QhullPoint QhullPointSet::
value(int index, QhullPoint &defaultValue) const
{
    // Avoid call to qh_setsize() and assert in elementPointer()
    void **n= reinterpret_cast<void **>(&SETelem_(getSetT(), index)); 
    coordT **n2= reinterpret_cast<coordT **>(n); 
    if(index>=0 && n<endPointer()){
        return QhullPoint(dimension(), *n2);
    }else{
        return defaultValue;
    }
}//value

#//Read-only

bool QhullPointSet::
operator==(const QhullPointSet &o) const
{
    if(dimension()!=o.dimension() || count()!=o.count()){
        return false;
    }
    QhullPointSetIterator i(*this);
    QhullPointSetIterator j(o);
    while(i.hasNext()){
        if(i.next()!=j.next()){
            return false;
        }
    }
    return true;
}//operator==

#//Search
bool QhullPointSet::
contains(const QhullPoint &t) const
{
    QhullPointSetIterator i(*this);
    while(i.hasNext()){
        if(i.next()==t){
            return true;
        }
    }
    return false;
}//contains

int QhullPointSet::
count(const QhullPoint &t) const
{
    int n= 0;
    QhullPointSetIterator i(*this);
    while(i.hasNext()){
        if(i.next()==t){
            ++n;
        }
    }
    return n;
}//count

int QhullPointSet::
indexOf(const QhullPoint &t) const
{
    int index= 0;
    QhullPointSetIterator i(*this);
    while(i.hasNext()){
        if(i.next()==t){
            return index;
        }
        ++index;
    }
    return -1;
}//indexOf

int QhullPointSet::
lastIndexOf(const QhullPoint &t) const
{
    int index= count()-1;
    QhullPointSetIterator i(*this);
    i.toBack();
    while(i.hasPrevious()){
        if(i.previous()==t){
            break;
        }
        --index;
    }
    return index;
}//lastIndexOf


#//QhullPointSetIterator

bool QhullPointSetIterator::
findNext(const QhullPoint &p)
{
    while(i!=c->constEnd()){
        if(*i++ == p){
            return true; 
        }
    }
    return false; 
}//findNext

bool QhullPointSetIterator::
findPrevious(const QhullPoint &p)
{ 
    while(i!=c->constBegin()){
        if(*(--i) == p){
            return true;
        }
    }
    return false;  
}//findPrevious

}//namespace orgQhull

#//Global functions

using std::endl;
using std::ostream;
using orgQhull::QhullPoint;
using orgQhull::QhullPointSet;
using orgQhull::UsingLibQhull;

#//operator<<

ostream & 
operator<<(ostream &os, const QhullPointSet &ps)
{
    os<< ps.print(UsingLibQhull::NOqhRunId); 
    return os; 
}//<<QhullPointSet

ostream & 
operator<<(ostream &os, const QhullPointSet::PrintIdentifiers &pr)
{
    const QhullPointSet s= *pr.point_set;
    if (pr.message) {
        os<< pr.message;
    }
    for(QhullPointSet::const_iterator i=s.begin(); i != s.end(); ++i){
        if(i!=s.begin()){
            os<< " ";
        }
        const QhullPoint point= *i;
        int id= point.id(pr.run_id);
        os<< "p" << id;
    }
    os<< endl;
    return os;
}//PrintIdentifiers

ostream & 
operator<<(ostream &os, const QhullPointSet::PrintPointSet &pr)
{
    const QhullPointSet s= *pr.point_set;
    if (pr.message) {
        os<< pr.message;
    }
    for(QhullPointSet::const_iterator i=s.begin(); i != s.end(); ++i){
        const QhullPoint point= *i;
        os<< point.print(pr.run_id);
    }
    return os;
}//printPointSet

