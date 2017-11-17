/*
 * ospfsendls.{cc,hh} -- broadcast lins states with IP packets
 * Author: Yifan CHEN, Haoze WU
 */

#include <click/config.h>
#include "ospfsendls.hh"
#include <click/args.hh>
#include <click/glue.hh>
#include <clicknet/ip.h>
#include <click/integers.hh>
#include "ospf.h"

CLICK_DECLS

OSPFSendLS::OSPFSendLS() : _timer(this), neighbours(0) { }

OSPFSendLS::~OSPFSendLS() { }

int OSPFSendLS::configure(Vector<String> &conf, ErrorHandler *errh)
{
  if (cp_va_kparse(conf, this, errh,
      "ID", cpkM, cpIPAddress, &_id,
      "NBINFOBASE", cpkM, cpElement, &neighbours,
      cpEnd) < 0)
    return -1;

  return 0;
}

int OSPFSendLS::initialize(ErrorHandler *)
{
  _timer.initialize(this);
  _timer.schedule_after_msec(OSPF_INTERVAL_BROADCAST);
  return 0;
}

void OSPFSendLS::run_timer(Timer *)
{
  uint16_t num = neighbours->end() - neighbours->begin();

  /* if no neighbours found, send no packets */
  if (num > 0) {
    click_chatter("%d: sending link-state messages from %s",
                  Timestamp::now().sec(),
                  _id.unparse().c_str());
    WritablePacket *p = Packet::make(sizeof(click_ip) + sizeof(click_ospf) +
                                     num * sizeof(uint32_t));
    memset(p->data(), '\0', p->length());

    /* set IP headers */
    click_ip *ipp = reinterpret_cast<click_ip *>(p->data());
    ipp->ip_len = htons(p->length() - sizeof(*ipp));
    ipp->ip_p = IPPROTO_OSPF;
    ipp->ip_src = _id.in_addr();
    ipp->ip_dst = IPAddress::make_broadcast().in_addr(); // set destination as broadcast

    /* set OSPF header */
    click_ospf *ospfp = reinterpret_cast<click_ospf *>(ipp + 1);
    ospfp->ospf_ver = 0;
    ospfp->ospf_type = 1;
    ospfp->ospf_len = num;
    ospfp->ospf_rt = _id.addr();

    uint32_t *nb = (uint32_t *) (ospfp + 1);
    for (NeighbourVector::iterator i = neighbours->begin(); i != neighbours->end(); i++ ) {
      *nb = i->ip.addr();
      nb++;
    }
 
    /* continue to set IP headers */
    ipp->ip_len = htons(p->length());
    ipp->ip_hl = sizeof(click_ip) >> 2;
    ipp->ip_v = 4;
    ipp->ip_ttl = 255;                  // send to as far as possible
    ipp->ip_sum = click_in_cksum((unsigned char*) ipp, sizeof(*ipp));

    p->set_ip_header(ipp, sizeof(click_ip));

    output(0).push(p);
  }

  _timer.schedule_after_msec(OSPF_INTERVAL_BROADCAST);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(OSPFSendLS)
