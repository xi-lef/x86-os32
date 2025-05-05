# Mini 32 bit multi-core operating system for x86

![Shell example](/screenshots/example.png?raw=true)

An x86 32 bit multi-core operating system, developed in a university course and (vastly) expanded in my free time.
It supports

- interrupts (from the keyboard, LAPIC timer, RTC)
- multiple processes (running on multiple CPUs)
- timer based scheduling
- synchronization using semaphores
- a simple shell with support for shell history and multiple commands to set the RTC time/date or timezone or to perform string based operations (e.g., strtol)

This repository includes the 32 bit version without any isolation of user and kernel space.
Two other versions exist:

- 32 bit single-core with
  - isolation of user and kernel space
  - system calls (e.g., fork)
  - paging (with copy-on-write)
  - inter process communication (IPC) with messages
  - and more
- 64 bit multi-core with
  - isolation of user and kernel space
  - system calls (e.g., fork)
  - core local storage (using swapgs)
  - and more
