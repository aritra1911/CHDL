#include <iostream>
#include "chips.hpp"

Wire POSITIVE_RAIL = { +5.0f };
Wire GROUND_RAIL   = {  0.0f };

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
    std::cout << "Chip 1: ";
    myChip.probePin(DemoNAND::OUT);
    std::cout << "Chip 2: ";
    myOtherChip.probePin(DemoNAND::OUT);

    // Now apply some inputs to the first chip
    myChip.connect(DemoNAND::IN0, POSITIVE_RAIL);
    myChip.connect(DemoNAND::IN1, POSITIVE_RAIL);

    // Make the chip process the inputs and give the respective output
    myChip.compute();

    // Check the output
    std::cout << "Chip 1: ";
    myChip.probePin(DemoNAND::OUT);

    // Now let's hook the other chip to myChip's output

    // get a reference to the net the output pin is connected to
    Wire* w = myChip.getNet(DemoNAND::OUT);

    // feed it into both the inputs of the other chip.
    myOtherChip.connect(DemoNAND::IN0, *w);
    myOtherChip.connect(DemoNAND::IN1, *w);

    // Now, make it compute!
    std::cout << "Chip 2: ";
    myOtherChip.compute();

    // And expect to get an inverted version of the first chip's output
    myOtherChip.probePin(DemoNAND::OUT);

    return 0;
}
