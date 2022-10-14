
It's simple to run selected tests from the command line.
For example, run just the empty-tree tests:
```
./BTree_test --gtest_filter=empty.*
```
or run everything except three level tests with
```
./BTree_test --gtest_filter=-threelevel.*
```

For more info, run
```./BTree_test --help```
or read this [Googletest documentation](https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md#running-test-programs-advanced-options)
