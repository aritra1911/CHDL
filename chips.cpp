#include <iostream>
#include <cmath>
#include "chips.hpp"

Wire::Wire(void) {
    this->voltage = NAN;
}

Wire::Wire(float voltage) {
    this->voltage = voltage;
}

bool Wire::operator ==(Wire& wire) {
    return this->voltage == wire.voltage;
}

Pin::Pin(PinType t) {
    this->pin = nullptr;
    this->type = t;
}

Pin::Pin(Wire net, PinType t) {
    this->pin = &net;
    this->type = t;
}

void Pin::defineAs(PinType t) {
    this->type = t;
}

void Pin::connectTo(Wire& net) {
    this->pin = &net;
}

bool Pin::isConnected(void) const {
    return this->pin;
}

Wire* Pin::getNet(void) const {
    return this->pin;
}

PinType Pin::getType(void) const {
    return type;
}

float Pin::probe(void) const {
    return pin ? pin->voltage : NAN;
}

void Pin::transition(float voltage) {
    if (this->type == POWER) {
        std::cerr << "Cannot transition a power pin\n";
        return;
    }

    if (!this->pin && this->type == OUTPUT)
        this->pin = new Wire;

    this->pin->voltage = voltage;
}

void IC::probePins(void) const {
    for (uint8_t i=0; i<this->pins; i++) {
        std::cout  << "Pin " << (size_t)(i + 1) << ": ";
        if (!this->pin[i].isConnected())
            std::cout << "Not Connected\n";
        else
            std::cout << this->pin[i].probe() << " Volts\n";
    }
}

DemoNAND::DemoNAND(void) {
    this->pins = 5;
    this->pin = new Pin[this->pins];

    // Define pin types
    this->pin[VCC].defineAs(POWER);
    this->pin[IN0].defineAs(INPUT);
    this->pin[IN1].defineAs(INPUT);
    this->pin[OUT].defineAs(OUTPUT);
    this->pin[GND].defineAs(POWER);
}

bool DemoNAND::isPowered(void) const {
    if (!this->pin[VCC].isConnected()) return false;
    if (!this->pin[GND].isConnected()) return false;

    if (this->pin[VCC].probe() - this->pin[GND].probe() > +2.5 VOLTS)
        return true;

    return false;
}

void DemoNAND::compute(void) {
    uint8_t logic_input0 = (this->pin[IN0].probe() > 2.5 VOLTS) ? 1 : 0;
    uint8_t logic_input1 = (this->pin[IN1].probe() > 2.5 VOLTS) ? 1 : 0;
    uint8_t logic_output = (logic_input0 & logic_input1) ^ 0x01;

    this->pin[OUT].transition(logic_output * 5 VOLTS);
}

void DemoNAND::connect(Pins pindx, Wire& net) {
    this->pin[pindx].connectTo(net);
}

float DemoNAND::probePin(Pins pindx) const {
    return this->pin[pindx].probe();
}

Wire* DemoNAND::getNet(Pins pindx) const {
    return this->pin[pindx].getNet();
}
