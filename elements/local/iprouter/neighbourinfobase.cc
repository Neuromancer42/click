#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "neighbourinfobase.hh"
#include "ospf.h"

CLICK_DECLS

NeighbourInfoBase::NeighbourInfoBase(): timer(this) { }

NeighbourInfoBase::~NeighbourInfoBase() { }

int NeighbourInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
  if (cp_va_kparse(conf, this, errh, cpEnd) < 0) return -1;

  timer.initialize(this);
  timer.schedule_now();

  return 0;
}

// Note: Click provides no Set structure, using a vector instead;
//       since neighbours are few, it's acceptable
void NeighbourInfoBase::addNeighbour(const IPAddress &ip, int32_t port) {
  bool found = false;
  for (NeighbourVector::iterator i = neighbours.begin(); i != neighbours.end(); i++) {
    if (i->ip == ip) {
      found = true;
      i->port = port;
      i->expirationTime = Timestamp::now() + 
                          Timestamp::make_msec(OSPF_INTERVAL_ROUTER_DOWN);
      break;
    }
  }
  if (found == false) {
    click_chatter("%d: find neighbour %s from port %d",
                  Timestamp::now().sec(),
                  ip.unparse().c_str(), port);
    NeighbourData nb;
    nb.ip = ip;
    nb.port = port;
    nb.expirationTime = Timestamp::now() + 
                        Timestamp::make_msec(OSPF_INTERVAL_ROUTER_DOWN);
    neighbours.push_back(nb);
  }
}

bool NeighbourInfoBase::removeNeighbour(const IPAddress &ip, int32_t port) {
  for (NeighbourVector::iterator i = neighbours.begin(); i != neighbours.end(); ) {
    if (i->ip == ip && i->port == port) {
      i = neighbours.erase(i);
      click_chatter("%d: removing neighbour %s from port %d",
                    Timestamp::now().sec(),
                    ip.unparse().c_str(), port);
      return true;
    } else {
      i++;
    }
  }
  return false;
}

void NeighbourInfoBase::run_timer(Timer*) {
  for (NeighbourVector::iterator i = neighbours.begin(); i != neighbours.end(); ) {
    if (i->expirationTime < Timestamp::now()) {
      click_chatter("%d: neighbour %s from port %d lost connection",
                    Timestamp::now().sec(),
                    i->ip.unparse().c_str(), i->port);
      i = neighbours.erase(i);
    } else {
      i++;
    }
  }
  timer.schedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighbourInfoBase)

#include <click/vector.cc>
#if EXPLICIT_TEMPLATE_INSTANCES
template class NeighbourVector;
#endif
