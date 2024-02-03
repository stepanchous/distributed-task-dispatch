#include "broker-impl/broker_impl.h"

int main() {
    auto broker = Broker::New();
    broker.RunBroker();
}
