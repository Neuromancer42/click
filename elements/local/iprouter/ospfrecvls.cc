#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include "ospf.h"
#include "ospfrecvls.hh"
#include <click/args.hh>

CLICK_DECLS

OSPFRecvLS::OSPFRecvLS() : lss(0) { }

OSPFRecvLS::~OSPFRecvLS() { }

int OSPFRecvLS::configure(Vector<String> &conf, ErrorHandler *errh) {
  if (cp_va_kparse(conf, this, errh,
                   "LSINFOBASE", cpkM, cpElement, &lss,
                   "ID", cpkM, cpIPAddress, &_id,
                   cpEnd) < 0)
    return -1;

  return 0;
}


void OSPFRecvLS::push(int, Packet *p) {
  WritablePacket *q = p->uniqueify();

  click_ip *iph = (click_ip *) q->data();
  click_ospf *ospfh = (click_ospf *) (iph + 1);

  IPAddress from(ospfh->ospf_rt);
  int num = ospfh->ospf_len;

  click_chatter("%d: receiving link-state from %s",
                Timestamp::now().sec(),
                from.unparse().c_str());
  uint32_t *nbs = (uint32_t *) (ospfh + 1);

  if (lss) {
    for (int i = 0; i < num; i++) {
        if (IPAddress(nbs[i]) != _id) {
          lss->addLS(from, IPAddress(nbs[i]));
        }
    }
  }

  p->kill();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(OSPFRecvLS)
