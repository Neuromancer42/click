#ifndef CLICK_LSINFOBASE_HH
#define CLICK_LSINFOBASE_HH
#include <click/element.hh>
#include <click/vector.hh>
#include <click/timer.hh>

CLICK_DECLS

struct LSData {
  IPAddress srcIP;
  IPAddress dstIP;
  Timestamp expirationTime;
};

typedef Vector<LSData> LSVector;

class LSInfoBase : public Element {

private:

  LSVector lss;
  Timer timer;

public:

  LSInfoBase();
  ~LSInfoBase();

  const char *class_name() const { return "LSInfoBase"; }
  const char *port_count() const { return "0/0";        }
  const char *processing() const { return PUSH;         }

  int configure(Vector<String>&, ErrorHandler*);

  void addLS(const IPAddress &srcIP, const IPAddress &dstIP);
  bool removeLS(const IPAddress &srcIP, const IPAddress &dstIP);

  void run_timer(Timer*);

  LSVector::iterator begin() { return lss.begin(); }
  LSVector::iterator end()   { return lss.end();   }

};

CLICK_ENDDECLS
#endif
