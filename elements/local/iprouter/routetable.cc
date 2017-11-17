#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "routetable.hh"

CLICK_DECLS

RouteTable::RouteTable() { }

RouteTable::~RouteTable() { }

int RouteTable::configure(Vector<String> &conf, ErrorHandler *errh) {
  if (cp_va_kparse(conf, this, errh, cpEnd) < 0)
    return -1;
  return 0;
}


void RouteTable::add_route(const IPAddress &addr, int32_t port) {
  RTable::iterator r = rt.begin();
  for ( ; r != rt.end(); r++) {
    if (r->addr == addr) {
      if (r->port != port) {
        click_chatter("%d: update route to %s via port %d",
                      Timestamp::now().sec(),
                      addr.unparse().c_str(), port);
        r->port = port;
      }
      break;
    }
  }
  if (r == rt.end()) {
    click_chatter("%d: adding route to %s via port %d",
                  Timestamp::now().sec(),
                  addr.unparse().c_str(), port);
    rt.push_back(Route(addr, port));
  }
}

void RouteTable::remove_route(const IPAddress &addr, int32_t port) {
  click_chatter("%d: removing a route to %s via port %d",
                Timestamp::now().sec(),
                addr.unparse().c_str(), port);
  RTable::iterator r = rt.begin();
  for ( ; r != rt.end(); r++) {
    if (r->addr == addr && r->port == port) {
      rt.erase(r);
      break;
    }
  }
}

int32_t RouteTable::lookup(const IPAddress &addr) {
  
  for (RTable::iterator r = rt.begin(); r != rt.end(); r++) {
    if (r->addr == addr)
      return r->port;
  }

  return -1;   // port -1 means no match found
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouteTable)

#include <click/vector.cc>
#if EXPLICIT_TEMPLATE_INSTANCES
template class RTable;
#endif
