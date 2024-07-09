# What is in this repository

This repository contains a few bits and pieces, as leftover from my short experiments with single slope ADC architecture.

# Single slope ADC
Typical low- to mid-end ADC DMM architecture is [dual-slope ADC](https://en.wikipedia.org/wiki/Integrating_ADC), integrated in form of ICs like [ICL7106](https://www.analog.com/en/products/icl7106.html) that started their way in Intersil as a ~~stolen~~ unanthorized copy of [Fluke 429100](https://github.com/fivesixzero/keithley-169-display-replacement/blob/main/README.md) ASIC; and now it's copies of unknown legal status are in every two dollar multimeter.
Its status is a result of good power line frequency supression, low complexity enabling integration into a monolithic IC, low sensitivity to passive component properties at acceptable linearity and resolution.
There are other integrating ADC topologies, and relevant to this repository is single slope ADC. Historically it's been a evolutionary predecessor of dual slope (and more involved integration schemes) and there were many reasons to leave and almost forget this architecture - namely, sensitivity to passive component variation and drift, as well as higher noise compared to dual slope. The sensitivity could be workarounded by using periodic calibration -  that is, before measuring the unknown voltage, measuring zero, reference voltage and calculating the unknown voltage by a simple arithmetics. This leaves us the speed/noise ratio even worse than it was to begin with, and the whole ADC implemetation greatly benefits from - and keeping the complexity at reasonable level is possible only with - using an MCU. Unlike the dual slope variant that works elegantly with a bunch of counters and simple timing logic.
This approach was chosen by Jim Williams in his [AN-260](https://github.com/jaromir-sukuba/single-slope-adc/blob/main/resources/snoa597b.pdf) appnote. It's a very good read, including the motivation to dig out this cadaver out of ADC graveyard; some of the hardware tricks to make it work and results he obtained.

## My implementation

Unlike Jim, I had the benefit of 40 years of semiconductor improvements in hand, newer components to choose from, so I went with somehow simpler implementation, but kept the basic principle the same overall.
![PCB](https://github.com/jaromir-sukuba/single-slope-adc/blob/main/media/board.jpg)

Instead of using JFET switches and related level switching, I went with DG411 switches. Input amplifier, integrator and slope amplifier contains OPA145 opamps. Somehow overkill for this application, but I tried to go with good opamp first and optimize later, if needed.
Comparator is good old LM311, in very basic configuration.
Brainbox of the whole setup is PIC18F04Q41 and its 86 lines of C code setup to measure all three values and emit it out through serial port immediately. In its current configuration it produces 31 samples per second.

## Results


# Resume

Don't do that.
