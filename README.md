# A comparison between Linux approaches for soft real-time applications

- Link to the Research: https://bv.fapesp.br/en/bolsas/200524/a-comparison-between-linux-approaches-for-soft-real-time-applications/

## Abstract

The growth of the use of the Linux operating system in embedded systems projects brings to the spotlight essential questions about the capabilities of this operating system in real-time systems, in particular, soft real-time systems, which represent a substantial spectrum of embedded projects. In this context, the quantitative and qualitative analysis of Linux embedded systems is the focus of this undergraduate research project, which includes the evaluation of the latency time, jitter,
and worst-case response time. Therefore, using a signal generator, an oscilloscope, and a single-board computer, it is intended to implement a computational routine that interfaces with the General Purpose Input/Output (GPIO) of the embedded system, replying to the stimuli generated by the signal generator. It is intended to implement these routines in User Space and Kernel Space on a Linux-based distribution and, also, in Linux real-time patches. These different approaches have
the intention to compare the real-time performance of distinct Linux-based systems.

## Experiments

1. User Space application with libgpiod

2. Kernel Space application with gpiod

3. User Space application with PREEMPT_RT

4. User Space application with Xenomai

## Blogposts

- [Installing Xenomai on the Beaglebone Black](https://mairacanal.github.io/install-xenomai-beaglebone-black/)