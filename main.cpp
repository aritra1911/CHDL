#include <iostream>
#include <cmath>
#include "chips.hpp"

Wire POSITIVE_RAIL = 5 VOLTS;
Wire GROUND_RAIL = 0 VOLTS;

void inline showVoltage(float);

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
    std::cout << "Chip 1, Pin " << (size_t) DemoNAND::OUT << " (OUT) : ";
    showVoltage(myChip.probePin(DemoNAND::OUT));
    std::cout << "Chip 2, Pin " << (size_t) DemoNAND::OUT << " (OUT) : ";
    showVoltage(myOtherChip.probePin(DemoNAND::OUT));

    // Now apply some inputs to the first chip
    myChip.connect(DemoNAND::IN0, POSITIVE_RAIL);
    myChip.connect(DemoNAND::IN1, POSITIVE_RAIL);

    // get a reference to the net the output pin is connected to
    Wire w1;  // Define a wire that'll serve as the connection between myChip's output and myOtherChip's inputs
    myChip.connect(DemoNAND::OUT, w1);  // connect it to myChip's output

    // feed it into both the inputs of the other chip.
    myOtherChip.connect(DemoNAND::IN0, w1);
    myOtherChip.connect(DemoNAND::IN1, w1);

    // Make the first chip process the inputs and give the respective output
    myChip.compute();
    std::cout << "Chip 1 computed!\n";

    // Now, make the other chip compute!
    myOtherChip.compute();
    std::cout << "Chip 2 computed!\n";

    // Check the output of first chip
    std::cout << "Chip 1, Pin " << (size_t) DemoNAND::OUT << " (OUT) : ";
    showVoltage(myChip.probePin(DemoNAND::OUT));

    // And expect to get an inverted version of the first chip's output
    std::cout << "Chip 2, Pin " << (size_t) DemoNAND::OUT << " (OUT) : ";
    showVoltage(myOtherChip.probePin(DemoNAND::OUT));

    return 0;
}

void inline showVoltage(float voltage) {
    if (std::isnan(voltage))
        std::cout << "Not connected!";
    else
        std::cout << voltage << " volts";
    std::cout << std::endl;
}
