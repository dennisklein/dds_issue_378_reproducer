### Reproducer for https://github.com/FairRootGroup/DDS/issues/378

Make sure you modify the hardcoded paths in `reproducer.cpp` and `reproducer.xml` to the context on your machine!

```
❯ git clone https://github.com/dennisklein/dds_issue_378_reproducer
❯ cd dds_issue_378_reproducer
❯ cmake -S. -Bbuild
-- The CXX compiler identification is GNU 11.1.1
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/lib64/ccache/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Using DDS 3.5.13.7.g85aee1f
-- Using Boost 1.75.0
-- Configuring done
-- Generating done
-- Build files have been written to: /home/dklein/projects/dds_issue_378_reproducer/build
❯ cmake --build build
[ 25%] Building CXX object CMakeFiles/di378task.dir/di378task.cpp.o
[ 50%] Linking CXX executable di378task
[ 50%] Built target di378task
[ 75%] Building CXX object CMakeFiles/reproducer.dir/reproducer.cpp.o
[100%] Linking CXX executable reproducer
[100%] Built target reproducer
❯ build/reproducer
51d2898e-9fe2-430e-a3a4-0e0fac08ea42
8fd70a03-26e7-4634-b859-174907989d30
af8a1ee6-1df9-475c-8e05-586584bdde42
cebcb87b-4cb8-4bce-a1d1-1192a7e95e9d
95454ae1-9515-4162-93ab-9b0409597fe2
cc8cd252-08a9-4452-95d6-dc06d041b56b
exiting ...
cc8cd252-08a9-4452-95d6-dc06d041b56b received msgs: 10000
95454ae1-9515-4162-93ab-9b0409597fe2 received msgs: 10000
cebcb87b-4cb8-4bce-a1d1-1192a7e95e9d received msgs: 10000
af8a1ee6-1df9-475c-8e05-586584bdde42 received msgs: 10000
8fd70a03-26e7-4634-b859-174907989d30 received msgs: 10000
51d2898e-9fe2-430e-a3a4-0e0fac08ea42 received msgs: 10000
```
