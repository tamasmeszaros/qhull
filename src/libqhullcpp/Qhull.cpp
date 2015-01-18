/****************************************************************************
**
** Copyright (c) 2008-2015 C.B. Barber. All rights reserved.
** $Id: //main/2011/qhull/src/libqhullcpp/Qhull.cpp#16 $$Change: 1810 $
** $DateTime: 2015/01/17 18:28:15 $$Author: bbarber $
**
****************************************************************************/

#//! Qhull -- invoke qhull from C++
#//! Compile libqhull and Qhull together due to use of setjmp/longjmp()

#include "QhullError.h"
#include "RboxPoints.h"
#include "QhullQh.h"
#include "QhullFacet.h"
#include "QhullFacetList.h"
#include "Qhull.h"
extern "C" {
    #include "libqhullr/qhull_ra.h"
}

#include <iostream>

using std::cerr;
using std::string;
using std::vector;
using std::ostream;

#ifdef _MSC_VER  // Microsoft Visual C++ -- warning level 4
#pragma warning( disable : 4611)  // interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning( disable : 4996)  // function was declared deprecated(strcpy, localtime, etc.)
#endif

namespace orgQhull {

#//Global variables

const char s_unsupported_options[]=" Fd TI ";
const char s_not_output_options[]= " Fd TI A C d E H P Qb QbB Qbb Qc Qf Qg Qi Qm QJ Qr QR Qs Qt Qv Qx Qz Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 Q10 Q11 R Tc TC TM TP TR Tv TV TW U v V W ";

#//Constructor, destructor, etc.
Qhull::
Qhull()
: qh_qh(0)
, origin_point()
, run_called(false)
, feasiblePoint()
{
    allocateQhullQh();
}//Qhull

//! Invokes Qhull on rboxPoints 
//! Same as runQhull()
//! For rbox commands, see http://www.qhull.org/html/rbox.htm or html/rbox.htm
//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
Qhull::
Qhull(const RboxPoints &rboxPoints, const char *qhullCommand2)
: qh_qh(0)
, origin_point()
, run_called(false)
, feasiblePoint()
{
    allocateQhullQh();
    runQhull(rboxPoints, qhullCommand2);
}//Qhull rbox

//! Invokes Qhull on a set of input points
//! Same as runQhull()
//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
Qhull::
Qhull(const char *inputComment, int pointDimension, int pointCount, const realT *pointCoordinates, const char *qhullCommand2)
: qh_qh(0)
, origin_point()
, run_called(false)
, feasiblePoint()
{
    allocateQhullQh();
    runQhull(inputComment, pointDimension, pointCount, pointCoordinates, qhullCommand2);
}//Qhull points

void Qhull::
allocateQhullQh()
{
    qh_qh= new QhullQh;
    if(qh_qh!=static_cast<qhT *>(qh_qh)){
        throw QhullError(10074, "Qhull error: QhullQh at a different address than base type QhT (%d bytes).  Please report compiler to qhull.org", (int)(static_cast<qhT *>(qh_qh)-qh_qh));
    }
}//allocateQhullQh

Qhull::
~Qhull() throw()
{
    // Except for cerr, does not throw errors
    if(qh_qh->hasQhullMessage()){
        cerr<< "\nQhull output at end\n"; //FIXUP QH11005: where should error and log messages go on ~Qhull?
        cerr<< qh_qh->qhullMessage();
        qh_qh->clearQhullMessage();
    }
    delete qh_qh;
    qh_qh= 0;
}//~Qhull

#//!\name GetSet

void Qhull::
checkIfQhullInitialized()
{
    if(!initialized()){ // qh_initqhull_buffers() not called
        throw QhullError(10023, "Qhull error: checkIfQhullInitialized failed.  Call runQhull() first.");
    }
}//checkIfQhullInitialized

#//GetValue

double Qhull::
area(){
    checkIfQhullInitialized();
    if(!qh_qh->hasAreaVolume){
        QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
            qh_getarea(qh_qh, qh_qh->facet_list);
        }
        qh_qh->maybeThrowQhullMessage(QH_TRY_status);
    }
    return qh_qh->totarea;
}//area

double Qhull::
volume(){
    checkIfQhullInitialized();
    if(!qh_qh->hasAreaVolume){
        QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
            qh_getarea(qh_qh, qh_qh->facet_list);
        }
        qh_qh->maybeThrowQhullMessage(QH_TRY_status);
    }
    return qh_qh->totvol;
}//volume

#//!\name Foreach

//! Define QhullVertex::neighborFacets().
//! Automatically called if merging facets or computing the Voronoi diagram.
//! Noop if called multiple times.
void Qhull::
defineVertexNeighborFacets(){
    checkIfQhullInitialized();
    if(!qh_qh->hasAreaVolume){
        QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
            qh_vertexneighbors(qh_qh);
        }
        qh_qh->maybeThrowQhullMessage(QH_TRY_status);
    }
}//defineVertexNeighborFacets

QhullFacetList Qhull::
facetList() const{
    return QhullFacetList(beginFacet(), endFacet());
}//facetList

QhullPoints Qhull::
points() const
{
    return QhullPoints(qh_qh, qh_qh->hull_dim, qh_qh->num_points*qh_qh->hull_dim, qh_qh->first_point);
}//points

QhullPointSet Qhull::
otherPoints() const
{
    return QhullPointSet(qh_qh, qh_qh->other_points);
}//otherPoints

//! Return vertices of the convex hull.
QhullVertexList Qhull::
vertexList() const{
    return QhullVertexList(beginVertex(), endVertex());
}//vertexList

#//!\name Methods

void Qhull::
outputQhull()
{
    checkIfQhullInitialized();
    QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
        qh_produce_output2(qh_qh);
    }
    qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//outputQhull

void Qhull::
outputQhull(const char *outputflags)
{
    checkIfQhullInitialized();
    string cmd(" "); // qh_checkflags skips first word
    cmd += outputflags;
    char *command= const_cast<char*>(cmd.c_str());
    QH_TRY_(qh_qh){ // no object creation -- destructors skipped on longjmp()
        qh_clear_outputflags(qh_qh);
        char *s = qh_qh->qhull_command + strlen(qh_qh->qhull_command) + 1; //space
        strncat(qh_qh->qhull_command, command, sizeof(qh_qh->qhull_command)-strlen(qh_qh->qhull_command)-1);
        qh_checkflags(qh_qh, command, const_cast<char *>(s_not_output_options));
        qh_initflags(qh_qh, s);
        qh_initqhull_outputflags(qh_qh);
        if(qh_qh->KEEPminArea < REALmax/2
           || (0 != qh_qh->KEEParea + qh_qh->KEEPmerge + qh_qh->GOODvertex
                    + qh_qh->GOODthreshold + qh_qh->GOODpoint + qh_qh->SPLITthresholds)){
            facetT *facet;
            qh_qh->ONLYgood= False;
            FORALLfacet_(qh_qh->facet_list) {
                facet->good= True;
            }
            qh_prepare_output(qh_qh);
        }
        qh_produce_output2(qh_qh);
        if(qh_qh->VERIFYoutput && !qh_qh->STOPpoint && !qh_qh->STOPcone){
            qh_check_points(qh_qh);
        }
    }
    qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//outputQhull

//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
void Qhull::
runQhull(const RboxPoints &rboxPoints, const char *qhullCommand2)
{
    runQhull(rboxPoints.comment().c_str(), rboxPoints.dimension(), rboxPoints.count(), &*rboxPoints.coordinates(), qhullCommand2);
}//runQhull, RboxPoints

//! pointCoordinates is a array of points, input sites ('d' or 'v'), or halfspaces with offset last ('H')
//! Derived from qh_new_qhull [user.c]
//! For rbox commands, see http://www.qhull.org/html/rbox.htm or html/rbox.htm
//! For qhull commands, see http://www.qhull.org/html/qhull.htm or html/qhull.htm
void Qhull::
runQhull(const char *inputComment, int pointDimension, int pointCount, const realT *pointCoordinates, const char *qhullCommand2)
{
    if(run_called){
        throw QhullError(10027, "Qhull error: runQhull called twice.  Only one call allowed.");
    }
    run_called= true;
    string s("qhull ");
    s += qhullCommand2;
    char *command= const_cast<char*>(s.c_str());
    /* FIXUP
    int QH_TRY_status; 
    if(qh_qh->NOerrexit){ 
        qh_qh->NOerrexit= False; 
        QH_TRY_status= setjmp(qh_qh->errexit); 
    }else{ 
    QH_TRY_status= QH_TRY_ERROR; 
    } 
    if(!QH_TRY_status){ 
*/
    QH_TRY_(qh_qh){ // no object creation -- destructors are skipped on longjmp()
        qh_checkflags(qh_qh, command, const_cast<char *>(s_unsupported_options));
        qh_initflags(qh_qh, command);
        *qh_qh->rbox_command= '\0';
        strncat( qh_qh->rbox_command, inputComment, sizeof(qh_qh->rbox_command)-1);
        if(qh_qh->DELAUNAY){
            qh_qh->PROJECTdelaunay= True;   // qh_init_B() calls qh_projectinput()
        }
        pointT *newPoints= const_cast<pointT*>(pointCoordinates);
        int newDimension= pointDimension;
        int newIsMalloc= False;
        if(qh_qh->HALFspace){
            --newDimension;
            initializeFeasiblePoint(newDimension);
            newPoints= qh_sethalfspace_all(qh_qh, pointDimension, pointCount, newPoints, qh_qh->feasible_point);
            newIsMalloc= True;
        }
        qh_init_B(qh_qh, newPoints, pointCount, newDimension, newIsMalloc);
        qh_qhull(qh_qh);
        qh_check_output(qh_qh);
        qh_prepare_output(qh_qh);
        if(qh_qh->VERIFYoutput && !qh_qh->STOPpoint && !qh_qh->STOPcone){
            qh_check_points(qh_qh);
        }
    }
    for(int k= qh_qh->hull_dim; k--; ){  // Do not move into QH_TRY block.  It may throw an error
        origin_point << 0.0;
    }
    qh_qh->maybeThrowQhullMessage(QH_TRY_status);
}//runQhull

#//Helpers -- be careful of allocating C++ objects due to setjmp/longjmp() error handling by qh_... routines

void Qhull::
initializeFeasiblePoint(int hulldim)
{
    if(qh_qh->feasible_string){
        qh_setfeasible(qh_qh, hulldim);
    }else{
        if(feasiblePoint.isEmpty()){
            qh_fprintf(qh_qh, qh_qh->ferr, 6209, "qhull error: missing feasible point for halfspace intersection.  Use option 'Hn,n' or set qh_qh.feasiblePoint\n");
            qh_errexit(qh_qh, qh_ERRmem, NULL, NULL);
        }
        if(feasiblePoint.size()!=(size_t)hulldim){
            qh_fprintf(qh_qh, qh_qh->ferr, 6210, "qhull error: dimension of feasiblePoint should be %d.  It is %u", hulldim, feasiblePoint.size());
            qh_errexit(qh_qh, qh_ERRmem, NULL, NULL);
        }
        if (!(qh_qh->feasible_point= (coordT*)qh_malloc(hulldim * sizeof(coordT)))) {
            qh_fprintf(qh_qh, qh_qh->ferr, 6202, "qhull error: insufficient memory for feasible point\n");
            qh_errexit(qh_qh, qh_ERRmem, NULL, NULL);
        }
        coordT *t= qh_qh->feasible_point;
        // No qh_... routines after here -- longjmp() ignores destructor
        for(Coordinates::ConstIterator p=feasiblePoint.begin(); p<feasiblePoint.end(); p++){
            *t++= *p;
        }
    }
}//initializeFeasiblePoint

}//namespace orgQhull

