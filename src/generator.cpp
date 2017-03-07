#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

bool is_number(const std::string & s){
    return !s.empty() && std::find_if(s.begin(),s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

int main(int argn, char ** argc){
	
	int ppid = 1;

	if(argn > 1){
		ppid = atoi(argc[1]);
	}

	path p("/proc");

    directory_iterator end_itr;
    map<string, pair < string, vector< string > > > processes;

    for (directory_iterator itr(p); itr != end_itr; ++itr) {

    	if( is_directory( itr->path() ) && is_number(itr->path().leaf().string()) ){

    		ifstream file(itr->path().string() + "/stat", ifstream::in);
    		std::vector<string> parsed(std::istream_iterator<string>(file), {});

    		if( processes.find(  parsed[0] )  == processes.end()){
    			processes.insert( make_pair( parsed[0] , make_pair( parsed[1], vector<string>() ) ) );
    		}else{
    			processes[  parsed[0] ].first = parsed[1];
    		}

    		if(parsed[3].compare("0")){
    			if(processes.find(  parsed[3] )  == processes.end()){
	    			processes.insert( make_pair( parsed[3] , make_pair( "", vector<string>() ) ) );
	    		}
	    		processes[ parsed[3] ].second.push_back( parsed[0] );
    		}
    		
    	}
    }

    for(auto it = processes.begin(); it != processes.end(); it++){
    	cout << "PROCESS " << it->second.first << " | PID (" << it->first << ")\n";
    	for(int i = 0; i < it->second.second.size(); ++i){
    		cout << it->second.second[i] << " ";
    	}
    	cout << endl;
    }

	return 0;
}