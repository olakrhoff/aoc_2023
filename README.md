# Advent of Code 2023
This is my attempt at the 2023 Advent of Code.

The ``main.cpp`` file in the outer folder is a template for how I like to start coding these problems.

The ``ìnit.sh`` is a script for creating a folder with a given name (the argument when calling ``./init.sh filename``), the folder will contain the ``main.cpp`` file and a ``data.txt`` and ``test_data.txt`` files, which is where I add the data for the problem. Don't worry if you accidentally type a folder that already exists, then it will just abort.


This year I added a challenge for myself, ALL the solutions I have run in under a second (at least on my system). Some of them needs a little ```-O3``` to run fast enough, but in those cases I usually have deliberately created the loops in a way that will utilise vector registers, hence a GREAT speedup.
