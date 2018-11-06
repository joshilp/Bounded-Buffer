# Bounded-Buffer README

1.  Bounded buffer implementation in C
2.  Uses Semaphore library - more information can be found at http://pubs.opengroup.org/onlinepubs/7908799/xsh/semaphore.h.html

## Overview

implement producer-consumer algorithm to manage access to a bounded buffer to store candy.  One group of threads will model candy factories which generate candy one at a time and insert the candy into the bounded buffer. Another group of threads will model kids which eat candy one a time from the bounded buffer.

## How to run

`./candykids <#factories> <#kids> <#seconds>`

For example, `./candykids 3 1 10`

* Factories: Number of candy-factory threads to spawn.
* Kids: Number of kid threads to spawn.
* Seconds: Number of seconds to allow the factory threads to run for.
