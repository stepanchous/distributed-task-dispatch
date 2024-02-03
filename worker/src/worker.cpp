#include <cppzmq/zmq.hpp>
#include <iostream>
using namespace std::string_literals;

int main(int, char* argv[]) {
    zmq::context_t context(1);

    zmq::socket_t subscriber(context, zmq::socket_type::sub);
    subscriber.set(zmq::sockopt::subscribe, "");
    subscriber.connect("tcp://localhost:5560");

    zmq::socket_t dealer(context, zmq::socket_type::dealer);
    dealer.set(zmq::sockopt::routing_id, argv[1]);
    dealer.connect("tcp://localhost:5561");

    dealer.send(zmq::message_t("Hi from "s + argv[1]), zmq::send_flags::none);

    std::vector<zmq::pollitem_t> poll_items = {{subscriber, 0, ZMQ_POLLIN, 0}};

    while (1) {
        zmq::poll(&poll_items[0], 1);

        if (poll_items[0].revents & ZMQ_POLLIN) {
            std::cout << "receiving..." << std::endl;

            zmq::message_t topic;
            zmq::message_t request;

            (void)subscriber.recv(topic, zmq::recv_flags::dontwait);
            (void)subscriber.recv(request);

            std::cout << "Worker got request from broker: " << request.str()
                      << std::endl;

            sleep(10);

            dealer.send(zmq::str_buffer("Work done"), zmq::send_flags::none);
        }

        // sleep(5);
        // if (notify_required) {
        // dealer.send(zmq::message_t(std::string("Hello from ") + argv[1]),
        //             zmq::send_flags::none);
        //     notify_required = false;
        // } else {
        //     continue;
        // }
    }
}
