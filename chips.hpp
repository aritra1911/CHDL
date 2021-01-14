#define VOLTS *1.0f

enum PinType : uint8_t {
    INPUT,
    OUTPUT,
    POWER
};

struct Wire {
    float voltage;
    bool operator ==(Wire&);
};

class Pin {
    PinType type;
    Wire* pin;  // A pointer to a net having a voltage

    public:
    Pin(PinType = INPUT);
    Pin(Wire, PinType = INPUT);
    void defineAs(PinType);
    void connectTo(Wire&);
    bool isConnected(void) const;
    Wire* getNet(void) const;
    PinType getType(void) const;
    float probe(void) const;
    void transition(float voltage);
};

class IC {
    protected:
    uint8_t pins;  // Total # of pins the chip has
    Pin* pin;  // Stores the state of each pin; either 0 or 1;
    double min_tp, max_tp;  // TODO, forget about these for now

    public:
    virtual bool isPowered(void) const = 0;
    void probePins(void) const;
    virtual void compute(void) = 0;  // Looks at INPUT pins and determine state of OUTPUT pins
};

class DemoNAND : public IC {
    /* A 5-pin Single Gate NAND Chip */
    public:
    enum Pins : uint8_t { VCC, IN0, IN1, OUT, GND };

    DemoNAND(void);
    bool isPowered(void) const;
    void compute(void);
    void connect(Pins, Wire&);
    void probePin(Pins) const;
    Wire* getNet(Pins) const;
};
