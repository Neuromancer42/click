#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "ospfupdate.hh"
#include <click/vector.hh>
#include "ospf.h"

CLICK_DECLS

OSPFUpdate::OSPFUpdate() : _timer(this), neighbours(0), lss(0) { }

OSPFUpdate::~OSPFUpdate() { }

int OSPFUpdate::configure(Vector<String> &conf, ErrorHandler *errh) {
  if (cp_va_kparse(conf, this, errh,
                   "ID", cpkM, cpIPAddress, &_id,
                   "NBINFOBASE", cpkM, cpElement, &neighbours,
                   "LSINFOBASE", cpkM, cpElement, &lss,
                   "IPTABLE", cpkM, cpElement, &iptable,
                   cpEnd) < 0)
    return -1;

  _timer.initialize(this);
  _timer.schedule_now();

  return 0;
}

void OSPFUpdate::run_timer(Timer *) {
  click_chatter("%d: calculating router %s 's routing table",
                Timestamp::now().sec(),
                _id.unparse().c_str());

  SimpleIPRouteTable temp;

  for (NeighbourVector::iterator i = neighbours->begin(); i != neighbours->end(); i++) {
    SimpleIPRoute r;
    r.addr = i->ip;
    r.metric = 1;
    r.port = i->port;
    r.v = false;
    temp.push_back(r);
  }

/*
  // a non-optimized Dijkstra Algo, O(V^2 E)
  while (true) {
    SimpleIPRouteTable::iterator t = temp.begin();
    for ( ; t != temp.end() && t->v ; t++);
    if (t == temp.end()) break;

    for (SimpleIPRouteTable::iterator r = t; r != temp.end(); r++)
      if (r->v == false && r->metric < t->metric)
        t = r;

    for (LSVector::iterator ls = lss->begin(); ls != lss->end(); ls++) {
      if (ls->srcIP == t->addr) {
        SimpleIPRouteTable::iterator r = temp.begin();
        for ( ; r != temp.end(); r++) {
          if (r->addr == ls->dstIP) {
            if (t->metric + 1 < r->metric) {
              r->metric = t->metric + 1;
              r->port = t->port;
            }
            break;
          }
        }
        if (r == temp.end()) {
          SimpleIPRoute tr;
          tr.addr = ls->dstIP;
          tr.metric = t->metric + 1;
          tr.port = t->port;
          tr.v = false;
          temp.push_back(tr);
        }
      }

      if (ls->dstIP == t->addr) {
        SimpleIPRouteTable::iterator r = temp.begin();
        for ( ; r != temp.end(); r++) {
          if (r->addr == ls->srcIP) {
            if (t->metric + 1 < r->metric) {
              r->metric = t->metric + 1;
              r->port = t->port;
            }
            break;
          }
        }
        if (r == temp.end()) {
          SimpleIPRoute tr;
          tr.addr = ls->srcIP;
          tr.metric = t->metric + 1;
          tr.port = t->port;
          tr.v = false;
          temp.push_back(tr);
        }
      }
    }

    t->v = true;
  }
*/


  // Since all lines are set to metric 1, a simpler floodfill could be used, O(V^2 E)
  for (SimpleIPRouteTable::iterator i = temp.begin(); i != temp.end(); i++) {
    for (LSVector::iterator ls = lss->begin(); ls != lss->end(); ls++) {
      if (ls->srcIP == i->addr) {
        SimpleIPRouteTable::iterator r = temp.begin();
        for ( ; r != temp.end() && r->addr != ls->dstIP; r++);
        if (r == temp.end()) {
          SimpleIPRoute tr;
          tr.addr = ls->dstIP;
          tr.metric = i->metric + 1;
          tr.port = i->port;
          tr.v = false;
          temp.push_back(tr);
        }
      }
      if (ls->dstIP == i->addr) {
        SimpleIPRouteTable::iterator r = temp.begin();
        for ( ; r != temp.end() && r->addr != ls->srcIP; r++);
        if (r == temp.end()) {
          SimpleIPRoute tr;
          tr.addr = ls->srcIP;
          tr.metric = i->metric + 1;
          tr.port = i->port;
          tr.v = false;
          temp.push_back(tr);
        }
      }
    }
  }

 
  // incremental update
  for (SimpleIPRouteTable::iterator i = rt.begin(); i != rt.end(); i++) {
    bool found = false;
    for (SimpleIPRouteTable::iterator j = temp.begin(); j != temp.end(); j++) {
      if (i->addr == j->addr && i->port == j->port) {
        found = true;
        break;
      }
    }
    if (found == false) {
      // delete unreachable points
      iptable->remove_route(i->addr, i->port);
    }
  }

  for (SimpleIPRouteTable::iterator i = temp.begin(); i != temp.end(); i++) {
    bool found = false;
    for (SimpleIPRouteTable::iterator j = rt.begin(); j != rt.end(); j++) {
      if (i->addr == j->addr && i->port == j->port) {
        found = true;
        break;
      }
    }
    if (found == false) {
      // add newly found routes
      iptable->add_route(i->addr, i->port);
    }
  }
  rt.clear();
  rt = temp;

  // may be better to directly clean up the iptable and rebuild for a **small** network

  _timer.schedule_after_msec(OSPF_INTERVAL_UPDATE);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(OSPFUpdate)
