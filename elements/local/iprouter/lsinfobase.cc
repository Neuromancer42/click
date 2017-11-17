#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "lsinfobase.hh"
#include "ospf.h"

CLICK_DECLS

LSInfoBase::LSInfoBase() : timer(this) { }
LSInfoBase::~LSInfoBase() { }

int LSInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
  if (cp_va_kparse(conf, this, errh, cpEnd) < 0) return -1;

  timer.initialize(this);
  timer.schedule_now();

  return 0;
}

void LSInfoBase::addLS(const IPAddress &srcIP, const IPAddress &dstIP) {
  bool found = false;
  for (LSVector::iterator i = lss.begin(); i != lss.end(); i++) {
    if ((i->srcIP == srcIP && i->dstIP == dstIP) ||
        (i->srcIP == dstIP && i->dstIP == srcIP)) {
      found = true;
      i->expirationTime = Timestamp::now() +
                           Timestamp::make_msec(OSPF_INTERVAL_LINK_DOWN);
      break;
    }
  }
  if (found == false) {
    click_chatter("%d: adding a link between %s and %s",
                  Timestamp::now().sec(),
                  srcIP.unparse().c_str(),
                  dstIP.unparse().c_str());
    LSData ls;
    ls.srcIP = srcIP;
    ls.dstIP = dstIP;
    ls.expirationTime = Timestamp::now() +
                        Timestamp::make_msec(OSPF_INTERVAL_LINK_DOWN);
    lss.push_back(ls);
  }
}

bool LSInfoBase::removeLS(const IPAddress &srcIP, const IPAddress &dstIP) {
  for (LSVector::iterator i = lss.begin(); i != lss.end();) {
    if ((i->srcIP == srcIP && i->dstIP == dstIP) ||
        (i->srcIP == dstIP && i->dstIP == srcIP)) {
      i = lss.erase(i);
      click_chatter("%d: removing link between %s and %s",
                    Timestamp::now().sec(),
                    srcIP.unparse().c_str(),
                    dstIP.unparse().c_str());
      return true;
    } else {
      i++;
    }
  }
  return false;
}

void LSInfoBase::run_timer(Timer*) {
  for (LSVector::iterator i = lss.begin(); i != lss.end(); ) {
    if (i->expirationTime < Timestamp::now()) {
      click_chatter("%d: link between %s and %s out of date",
                    Timestamp::now().sec(),
                    i->srcIP.unparse().c_str(),
                    i->dstIP.unparse().c_str());
      i = lss.erase(i);
    } else {
      i++;
    }
  }
  timer.schedule_after_msec(1000);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(LSInfoBase)

#include <click/vector.cc>
#if EXPLICIT_TEMPLATE_INSTANCES
template class LSVector;
#endif
