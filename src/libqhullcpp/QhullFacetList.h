/****************************************************************************
**
** Copyright (c) 2008-2015 C.B. Barber. All rights reserved.
** $Id: //main/2011/qhull/src/libqhullcpp/QhullFacetList.h#10 $$Change: 1810 $
** $DateTime: 2015/01/17 18:28:15 $$Author: bbarber $
**
****************************************************************************/

#ifndef QHULLFACETLIST_H
#define QHULLFACETLIST_H

#include "QhullLinkedList.h"
#include "QhullFacet.h"

#include <ostream>

#ifndef QHULL_NO_STL
#include <vector>
#endif

namespace orgQhull {

#//!\name Used here
    class Qhull;
    class QhullFacet;
    class QhullQh;

#//!\name Defined here
    //! QhullFacetList -- List of Qhull facets, as a C++ class.  See QhullFacetSet.h
    class QhullFacetList;
    //! QhullFacetListIterator -- if(f.isGood()){ ... }
    typedef QhullLinkedListIterator<QhullFacet> QhullFacetListIterator;

class QhullFacetList : public QhullLinkedList<QhullFacet> {

#//!\name  Fields
private:
    bool                select_all;   //! True if include bad facets.  Default is false.

#//!\name Constructors
public:
                        QhullFacetList(const Qhull &q, facetT *b, facetT *e);
                        QhullFacetList(QhullQh *qh, facetT *b, facetT *e);
                        QhullFacetList(QhullFacet b, QhullFacet e) : QhullLinkedList<QhullFacet>(b, e), select_all(false) {}
                        //Copy constructor copies pointer but not contents.  Needed for return by value and parameter passing.
                        QhullFacetList(const QhullFacetList &other) : QhullLinkedList<QhullFacet>(*other.begin(), *other.end()), select_all(other.select_all) {}
    QhullFacetList &    operator=(const QhullFacetList &other) { QhullLinkedList<QhullFacet>::operator =(other); select_all= other.select_all; }
                        ~QhullFacetList() {}

private:                //!Disable default constructor.  See QhullLinkedList
                        QhullFacetList();
public:

#//!\name Conversion
#ifndef QHULL_NO_STL
    std::vector<QhullFacet> toStdVector() const;
    std::vector<QhullVertex> vertices_toStdVector() const;
#endif //QHULL_NO_STL
#ifdef QHULL_USES_QT
    QList<QhullFacet>   toQList() const;
    QList<QhullVertex>  vertices_toQList() const;
#endif //QHULL_USES_QT

#//!\name GetSet
                        //! Filtered by facet.isGood().  May be 0 when !isEmpty().
    countT              count() const;
    bool                contains(const QhullFacet &f) const;
    countT              count(const QhullFacet &f) const;
    bool                isSelectAll() const { return select_all; }
    QhullQh *           qh() const { return first().qh(); }
    void                selectAll() { select_all= true; }
    void                selectGood() { select_all= false; }
                        //!< operator==() does not depend on isGood()

#//!\name IO
    struct PrintFacetList{
        const QhullFacetList *facet_list;
                        PrintFacetList(const QhullFacetList &fl) : facet_list(&fl) {}
    };//PrintFacetList
    PrintFacetList      print() const  { return PrintFacetList(*this); }

    struct PrintFacets{
        const QhullFacetList *facet_list;
                        PrintFacets(const QhullFacetList &fl) : facet_list(&fl) {}
    };//PrintFacets
    PrintFacets         printFacets() const { return PrintFacets(*this); }

    struct PrintVertices{
        const QhullFacetList *facet_list;
                        PrintVertices(const QhullFacetList &fl) : facet_list(&fl) {}
    };//PrintVertices
    PrintVertices       printVertices() const { return PrintVertices(*this); }
};//class QhullFacetList

}//namespace orgQhull

#//!\name == Global namespace =========================================

std::ostream &operator<<(std::ostream &os, const orgQhull::QhullFacetList::PrintFacetList &p);
std::ostream &operator<<(std::ostream &os, const orgQhull::QhullFacetList::PrintFacets &p);
std::ostream &operator<<(std::ostream &os, const orgQhull::QhullFacetList::PrintVertices &p);
std::ostream &operator<<(std::ostream &os, const orgQhull::QhullFacetList &fs);

#endif // QHULLFACETLIST_H
