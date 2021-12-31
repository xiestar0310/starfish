PHASE 1: Setting up the project
================================================================================
Things to keep in mind for this section:
- It might be nice to get a sense for what each framework does and what we're
    using it for: for example, maybe to write some example tests, or example
    benchmarks, so we can see how everything fits together
- It might also be nice to think a little about how to structure everything
    into directories

STEPS:
- First, set up a bare bones CMake project
- Write a simple hello world program and try out the compilation pipeline
- Then, hook up glog, gbench, and gtest (the Google logging, benchmarking,
    and testing frameworks)

PHASE 2: Designing various interfaces
================================================================================
Things to keep in mind for this section:
- Since there's a ton of information about chess programming online, it's also
    important not to simply copy implementations or designs without first
    understanding them
- We want the interfaces to be as modular and reusable as possible: for example,
    it should be theoretically possible to adjust the engine to play crazyhouse,
    or some other crazy variants without needing to completely rewrite
    everything

STEPS:
- Design a viable Piece interface, including but not limited to:
  - Storing the colour and piece type
  - That's about it, there's not much to this, it's most likely an enum

- Design a viable Move interface, including but not limited to:
  - Constructor which takes the from, to squares, as well as an optional
      promotion piece and captured piece (required for unmaking)
  - Initial ideas: probably have to try and fit this compactly into a 64-bit
      integer

- Design a viable Board interface, including but not limited to:
  - Constructor which takes an optional fen string
  - Copy constructor
  - makeMove
  - unmakeMove(?)
  - generatePseudolegalMoves(mask)
    - where mask filters the kinds of moves to generate
  - Initial ideas:
    - We can debate between square-centric, piece-centric, and bitboard-related
        representations for days, probably going to settle for a mix

- Document these interfaces

PHASE 3: Implementing move generation
================================================================================
Things to keep in mind for this section:
- Don't forget to log, assert, and document! Keeping the code clean will really
    really help us later
- Finally, try and make every individual function as short as possible: separate
    individual steps into small functions whenever possible: this helps
    readability and keeps the code clean

STEPS:
- Implement the constructors and printers for the interfaces we designed
- Then move on to the simple accessors and mutators
- Then start implementing legal moves
  - Don't forget to start with pseudocode and modularize as soon as possible!
- Write some tests, add some logging, add some timing infrastructure
- In particular, we should add some perft tests and perft benchmarks

PHASE 4: Implementing static evaluation
================================================================================
STEPS:
- Design a modular way of statically evaluating a position, which is open to
    modification
- Start by implementing a simple evaluation function, but keeping in mind ideas
    for future modification, and making sure the interface can support that
    gracefully

PHASE 5: Implementing the tree search
================================================================================
STEPS:
- Start with simple minimax, then negamax, then alpha-beta: these are all
    relatively simple to write, but increase slightly in complexity
- At some point, we will wish for a few things:
  - Transposition tables
  - Better move ordering
  - Quiescense search
- In which case, we'll probably talk about how to do these things with good
    design in mind
