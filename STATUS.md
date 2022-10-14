Update to the below: Insert now works, everything has been attempted and completed except for remove


I got lookup, count and depth to work. I could not get insert to work. Since insert did not work, most of the tests will fail because they rely on insert. However I set up test cases in project4.cc to make sure that it worked properly. For lookup, I tested looking for things in and not in the tree. The expected value turned up for each of the things I tested. Similarly to depth where I set up they key being in the tree, not in the tree and in the root. Each of these passed. Then for count, I counted that there are 31 keys in the tree and then tested to see if it returned 31 keys which it did after fixing the code.

the test code is at the end of project4.cc

count test:
B
Looking up  CORRECT



depth tests:
V PENGUIN
Looking up PENGUIN Level: 1

V UNICORN
Looking up UNICORN Level: 2

V NEWT
Looking up NEWT Root

V ASDF
Looking up ASDF Not found



lookup tests:
L UNICORN
Looking up UNICORN - Found: URSULA

L NEWT
Looking up NEWT - Found: NATE

L PENGUIN
Looking up PENGUIN - Found: PAUL

L ASDF
Looking up ASDF - Not found
