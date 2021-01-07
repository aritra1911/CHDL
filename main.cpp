#include <iostream>

#define VOLTS *1.0f

enum PinType : uint8_t {
    INPUT,
    OUTPUT,
    POWER
};

struct Wire {
    float voltage;

    bool operator ==(Wire& wire) {
        return voltage == wire.voltage;
    }
};

Wire POSITIVE_RAIL = { +5.0f };
Wire GROUND_RAIL   = {  0.0f };
Wire NEGATIVE_RAIL = { -5.0f };

class Pin {
    PinType type;
    Wire* pin;  // A pointer to a net having a voltage

    public:
    Pin(PinType t=INPUT) {
        this->pin = nullptr;
        this->type = t;
    }

    Pin(Wire net, PinType t=INPUT) {
        this->pin = &net;
        this->type = t;
    }

    void defineAs(PinType t) {
        this->type = t;
    }

    void connectTo(Wire& net) {
        this->pin = &net;
    }

    bool isConnected(void) {
        return this->pin;
    }

    Wire* getNet(void) {
        return &(*(this->pin));
    }

    PinType getType(void) {
        return type;
    }

    float probe(void) {
        return pin->voltage;
    }

    void transition(float voltage) {
        if (this->type == POWER) {
            std::cerr << "Cannot transition a power pin\n";
            return;
        }

        if (!this->pin && this->type == OUTPUT)
            this->pin = new Wire;

        this->pin->voltage = voltage;
    }
};

class IC {
    protected:
    uint8_t pins;  // Total # of pins the chip has
    Pin* pin;  // Stores the state of each pin; either 0 or 1;
    double min_tp, max_tp;  // TODO, forget about these for now

    public:
    bool isPowered(void);

    void probePins(void) {
        for (uint8_t i=0; i<this->pins; i++) {
            std::cout  << "Pin " << (size_t)(i + 1) << ": ";
            if (!this->pin[i].isConnected())
                std::cout << "Not Connected\n";
            else
                std::cout << this->pin[i].probe() << " Volts\n";
        }
    }

    void compute(void);  // Looks at INPUT pins and determine state of OUTPUT pins
};

class DemoNAND : public IC {
    /* A 5-pin Single Gate NAND Chip */
    public:
    enum Pins : uint8_t { VCC, IN0, IN1, OUT, GND };

    DemoNAND() {
        this->pins = 5;
        this->pin = new Pin[this->pins];

        // Define pin types
        this->pin[VCC].defineAs(POWER);
        this->pin[IN0].defineAs(INPUT);
        this->pin[IN1].defineAs(INPUT);
        this->pin[OUT].defineAs(OUTPUT);
        this->pin[GND].defineAs(POWER);
    }

    bool isPowered(void) {
        if (!this->pin[VCC].isConnected()) return false;
        if (!this->pin[GND].isConnected()) return false;

        if (this->pin[VCC].probe() - this->pin[GND].probe() > +2.5 VOLTS)
            return true;

        return false;
    }

    void compute(void) {
        uint8_t logic_input0 = (this->pin[IN0].probe() > 2.5 VOLTS) ? 1 : 0;
        uint8_t logic_input1 = (this->pin[IN1].probe() > 2.5 VOLTS) ? 1 : 0;
        uint8_t logic_output = (logic_input0 & logic_input1) ^ 0x01;

        this->pin[OUT].transition(logic_output * 5 VOLTS);
    }

    void connect(Pins pindx, Wire& net) {
        this->pin[pindx].connectTo(net);
    }

    void probePin(Pins pindx) {
        std::cout << "Pin " << (size_t)pindx << ": ";
        if (!this->pin[pindx].isConnected())
            std::cout << "Not Connected\n";
        else
            std::cout << this->pin[pindx].probe() << " Volts\n";
    }

    Wire* getNet(Pins pindx) {
        return this->pin[pindx].getNet();
    }
};

int main(void) {
    DemoNAND myChip, myOtherChip;

    // Power myChip
    myChip.connect(DemoNAND::VCC, POSITIVE_RAIL);
    myChip.connect(DemoNAND::GND, GROUND_RAIL);

    // Power myOtherChip
    myOtherChip.connect(DemoNAND::VCC, POSITIVE_RAIL);
    myOtherChip.connect(DemoNAND::GND, GROUND_RAIL);

    // Check if chips are powered
    std::cout << (myChip.isPowered() ? "Chip 1 is powered\n" : "No power to chip 1\n");
    std::cout << (myOtherChip.isPowered() ? "Chip 2 is powered\n" : "No power to chip 2\n");

    // Check outputs initially
    myChip.probePin(DemoNAND::OUT);
    myOtherChip.probePin(DemoNAND::OUT);

    // Now apply some inputs to the first chip
    myChip.connect(DemoNAND::IN0, GROUND_RAIL);
    myChip.connect(DemoNAND::IN1, POSITIVE_RAIL);

    // Make the chip process the inputs and give the respective output
    myChip.compute();

    // Check the output
    myChip.probePin(DemoNAND::OUT);

    // Now let's hook the other chip to myChip's output

    // get a reference to the net the output pin is connected to
    Wire* w = myChip.getNet(DemoNAND::OUT);

    // feed it into both the inputs of the other chip.
    myOtherChip.connect(DemoNAND::IN0, *w);
    myOtherChip.connect(DemoNAND::IN1, *w);

    // Now, make it compute!
    myOtherChip.compute();

    // And expect to get an inverted version of the first chip's output
    myOtherChip.probePin(DemoNAND::OUT);

    return 0;
}
