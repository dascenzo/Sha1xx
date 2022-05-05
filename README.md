# Sha1++
Command line program that computes the SHA-1 hash of a file, using C++17 fold expressions and `if constexpr`, along with templates, to statically generate the iterations of the SHA-1 algorithm.

## Building
To build, you need to be in a POSIX environment and have a C++17 compiler installed. From within the top-level directory of the project, run:

    $ make [BUILD=<debug|release>]

The resulting build, including `sha1++` binary, will be placed in the directory `build-<debug|release>/`. The default build type is `debug`.

## Example
    $ echo "hello world" > hello.txt
    $ make BUILD=release
    $ build-release/sha1++ hello.txt
    22596363b3de40b06f981fb85d82312e8c0ed511
    
## Testing
There is a test script `test/test-runner.sh`. It performs system testing by running the program with various input files and checking if the output is what's expected.

    $ ./test-runner.sh -h
    usage: ./test-runner.sh [-h] [-p|--show-passing] <debug|release>
    
## License

See [LICENSE](LICENSE.txt).
