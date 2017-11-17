#ifndef CLICK_NEIGHBOURINFOBASE_HH
#define CLICK_NEIGHBOURINFOBASE_HH
#include <click/element.hh>
#include <click/vector.hh>
#include <click/timer.hh>

CLICK_DECLS

struct NeighbourData {
  IPAddress ip;
  int32_t port;
  Timestamp expirationTime;
};

typedef Vector<NeighbourData> NeighbourVector;

class NeighbourInfoBase : public Element {

private:

  NeighbourVector neighbours;
  Timer timer;

public:
  NeighbourInfoBase();
  ~NeighbourInfoBase();

  const char *class_name() const { return "NeighbourInfoBase"; }
  const char *port_count() const { return "0/0";               }
  const char *processing() const { return PUSH;                }

  int configure(Vector<String>&, ErrorHandler*);

  void addNeighbour(const IPAddress &dstIP, int32_t port);
  bool removeNeighbour(const IPAddress &dstIP, int32_t port);

  void run_timer(Timer*);

  int get_size() {
    return neighbours.end() - neighbours.begin();
  }
  NeighbourVector::iterator begin() {
    return neighbours.begin();
  }
  NeighbourVector::iterator end() {
    return neighbours.end();
  }

};

CLICK_ENDDECLS
#endif
