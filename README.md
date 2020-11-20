# Pair Wars

For this project I created a “Pair Wars” card game to see how p-threads work with task parallelism using the c++ programing language. I structured this program using the top down approach since I initialized the p-threads first that would control the locks as well as the synchronization of multiple threads. I laid out a thread for the dealer as well as three threads for the players of the pair war. After a player wins, the thread sends out a message saying that the player won and is exiting the game. 

In order to run the program, you must first build it inside of a linux command line using the command “gcc pairwars.cpp -lpthread”. Once the program has been compiled successfully, you will be able to run the program using the command “./a.out” followed by a seed value of your choice. Here is an example of a possible build and run:

gcc pairwars.cpp -lpthread [ENTER]
./a.out 123 [ENTER]

Contained in testcases.txt, is five sample runs. For the test cases, we used the values of:
1
5
33
150
233
