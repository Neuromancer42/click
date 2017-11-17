#include <click/config.h>
#include <click/confparse.hh>
#include "mylookup.hh"
#include <click/ipaddress.hh>
#include <click/straccum.hh>
#include <click/router.hh>
#include <click/error.hh>
CLICK_DECLS

MyLookup::MyLookup() : rt(0) { }

MyLookup::~MyLookup() { }

int MyLookup::configure(Vector<String> &conf, ErrorHandler *errh)
{
  if (cp_va_kparse(conf, this, errh,
                   "TABLE", cpkM, cpElement, &rt,
                   cpEnd) < 0)
    return -1;

  return 0;
}

void
MyLookup::push(int, Packet *p)
{
    IPAddress gw;
    int port = rt->lookup(p->dst_ip_anno());

    if (port >= 0) {
        output(port).push(p);
    } else
        p->kill();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MyLookup)
