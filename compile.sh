g++ -g -c -Wall -std=c++11 -O3 -I include -o "build/jsoncpp.o" "src/jsoncpp.cpp"
g++ -g -c -Wall -std=c++11 -O3 -I include -o "build/generator.o" "src/generator.cpp"
g++ -std=c++11 -O3 "build/generator.o" "build/jsoncpp.o" -o "bin/generator" -lboost_system -lboost_filesystem