*This project has been created as part of the 42 curriculum by abdait-s*

# Codexion

## Description

Codexion is a concurrency simulation written in C. Several coders sit around a
circular co-working hub and share USB dongles placed between them. To compile
their quantum code, a coder must hold **two** dongles at the same time (one in
each hand). The life of a coder is an endless loop:

```
take 2 dongles -> compiling -> put them back -> debugging -> refactoring -> ...
```

If a coder does not manage to *start* a new compile before
`time_to_burnout` milliseconds have passed since the beginning of their last
compile (or since the start of the simulation), they **burn out** and the
simulation stops. The simulation also stops successfully when every coder has
compiled at least `number_of_compiles_required` times.

Two extra twists compared to a classic dining philosophers problem:

- **Dongle cooldown**: after being released, a dongle cannot be taken again
  until `dongle_cooldown` milliseconds have elapsed.
- **Scheduling policy**: when several coders wait for the same dongle, it is
  granted either in arrival order (`fifo`) or to the coder with the earliest
  burnout deadline (`edf`, where deadline = last compile start +
  time_to_burnout).

## Instructions

Compile:

```sh
make
```

Run (all 8 arguments are mandatory):

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
    time_to_refactor number_of_compiles_required dongle_cooldown [fifo|edf]
```

Example (4 coders, burnout after 800 ms, compile takes 200 ms, debug 200 ms,
refactor 100 ms, everyone must compile twice, no cooldown, FIFO scheduler):

```sh
./codexion 4 800 200 200 100 2 0 fifo
```

Other rules available in the Makefile: `clean`, `fclean`, `re`.
Invalid inputs (negative numbers, non integers, unknown scheduler) are
rejected with a usage message.

## Blocking cases handled

- **Deadlock prevention**: every coder always asks for its lower numbered
  dongle first, so a coder can only hold one dongle while waiting for a
  strictly bigger one. A circular chain of waiting coders is therefore
  impossible, which breaks one of Coffman's conditions (circular wait).
  Hold and wait alone cannot freeze the program because grants are always
  eventually given by the queue.
- **Starvation prevention**: each dongle has a waiting priority queue. With
  `fifo` requests are served strictly in arrival order; with `edf` the
  closest deadline wins, ties being broken deterministically (arrival time,
  then coder number), so under feasible parameters no coder starves and
  nobody burns out because of scheduling.
- **Cooldown handling**: a released dongle stores `available_at = now +
  cooldown`. Waiters sleep on a timed condition variable until that date,
  then compete again through the normal queue. No busy waiting is needed.
- **Precise burnout detection**: a dedicated monitor thread polls every
  millisecond and compares the current time with each coder's deadline
  (`last_compile_start + time_to_burnout`). The `burned out` log appears
  within ~1-2 ms of the real event, well inside the 10 ms tolerance.
- **Log serialization**: every message goes through one function protected
  by a print mutex, so two threads can never interleave characters on the
  same line.

## Thread synchronization mechanisms

- `pthread_mutex_t`: one mutex per dongle protects its whole state
  (busy flag, owner, cooldown date, waiting queue). One data mutex protects
  the shared flags (`stop`, `fed`) and per coder values (deadline, meal
  count). One print mutex serializes the logs.
- `pthread_cond_t`: one per dongle. A coder that cannot be served yet sleeps
  on it with `pthread_cond_wait` (waiting for a release) or
  `pthread_cond_timedwait` (waiting for a cooldown to expire). Releasing a
  dongle broadcasts to wake up every waiter, and the monitor broadcasts on
  all dongles when the simulation stops so nobody stays blocked.
- **Race condition prevention example**: the "am I allowed to take this
  dongle?" test (I am first in the queue AND the dongle is free AND the
  cooldown expired) and the actual grant happen atomically under the dongle
  mutex, so two coders can never both believe they own the same dongle.
  Similarly the deadline update done when a compile starts and the monitor's
  read of that deadline are both guarded by the data mutex.
- **Coders <-> monitor communication**: they only share the `stop` / `fed`
  flags protected by the data mutex. The monitor sets them and wakes the
  waiters; coder threads check them at every step of their loop and exit
  cleanly.


## Resources

- man pages: `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`,
  `pthread_cond_timedwait(3)`, `gettimeofday(2)`
- Wikipedia: [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem),
  [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling),
  [Coffman conditions](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)
- "Min-heap / priority queue" classic course notes used for the hand
  written heap in `heap.c`.

### Use of AI

AI was used as a study helper functions : brainstorming the deadlock-free acquisition
order, checking my understanding of `pthread_cond_timedwait`
semantics, and reviewing this README as well. all the code was read,
tested and fully understood by me before being pushed to the intra .
