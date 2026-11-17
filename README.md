*This project has been created as part of the 42 curriculum by aslobodi.*

# Codexion

A multi-threaded C program. Coders sit in a circle, and each one needs the
two dongles next to them to compile. There are as many dongles as coders,
so they have to share.

Each coder repeats: **take 2 dongles → compile → debug → refactor.**
If a coder waits too long between two compiles, they **burn out** and the
simulation stops. After a dongle is released, it needs a short **cooldown**
before anyone can take it again.

## Build and run

```bash
make
./codexion <coders> <burnout> <compile> <debug> <refactor> <compiles_required> <cooldown> <fifo|edf>
```

Example:

```bash
./codexion 5 3000 200 200 200 10 400 edf
```

All times are in milliseconds. The simulation ends when every coder has
compiled `compiles_required` times, or when someone burns out.

## Scheduling

Each dongle has a small waiting line (queue) of coders.

- **fifo**: first to ask, first to get the dongle.
- **edf**: the coder closest to burning out goes first.

## How the problems are solved

| Problem | Solution |
| :--- | :--- |
| Deadlock | Every coder takes the dongle with the **lower id first**, so a circular wait is impossible. |
| Starvation | Each dongle has a priority queue (fifo or edf). Ties are broken by request time. |
| Cooldown | A coder at the front of the queue sleeps the remaining cooldown without holding the mutex. |
| Burnout | A monitor thread checks all coders every 1 ms. |
| Messy output | One print mutex, and nothing is printed after the simulation ends. |
| Clean stop | The monitor sets `is_finished` and wakes every waiting thread, so all threads can be joined. |

## Files

| File | Role |
| :--- | :--- |
| `main.c` | Starts threads, coder routine |
| `parser.c` | Checks the arguments |
| `init.c` | Allocates and cleans the data |
| `dongle.c` | Take / drop a dongle, cooldown |
| `pqueue.c` | Priority queue of a dongle |
| `monitor.c` | Burnout and end detection |
| `utils.c` | Time, safe printing |

## Resources

- Dijkstra, *Hierarchical ordering of sequential processes* (1965)
- [Code Vault - C Threads playlist](https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2)
- [JomaClass - Priority Queue & Heap](https://youtu.be/yntfI_jqNms)

AI was used for: reading logs and timestamps, discussing design choices,
and writing this README.
