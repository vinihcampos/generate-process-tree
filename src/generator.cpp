#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <utility>
#include <fstream>
#include <boost/filesystem.hpp>
#include "json/json.h"

using namespace std;
using namespace boost::filesystem;

map<string, pair < string, vector< string > > > processes;

bool is_number(const std::string & s){
    return !s.empty() && std::find_if(s.begin(),s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

Json::Value populate_json(string pid){
	Json::Value name;
	name["name"] = Json::Value(pid + " " + processes[pid].first);

	Json::Value children;
	if(!processes[pid].second.empty()){
		for(int i = 0; i < processes[pid].second.size(); ++i){
			children.append( populate_json( processes[pid].second[i] ) );
		}
	}
	Json::Value structure;
	structure["text"] = name;
	
	if(!children.isNull()) structure["children"] = children; 

	return structure; 
}

int main(int argn, char ** argc){
	
	int pid = 1;

	if(argn > 1){
		pid = atoi(argc[1]);
	}

	cout << "PID: " << pid << endl;

	path p("/proc");

    directory_iterator end_itr;

    queue<string> directory_has_subprocesses;

    for (directory_iterator itr(p); itr != end_itr; ++itr) {

    	if( is_directory( itr->path() ) && is_number(itr->path().leaf().string()) ){
    		ifstream file(itr->path().string() + "/stat", ifstream::in);
    		std::vector<string> parsed(std::istream_iterator<string>(file), {});

            cout <<"Init: " << itr->path().string() << "/stat" << endl;

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

            if(exists(itr->path().string() + "/task")){
                directory_has_subprocesses.push(itr->path().string() + "/task");
            }    		
    	}
    }

    while(!directory_has_subprocesses.empty()){
        string front = directory_has_subprocesses.front();
        directory_has_subprocesses.pop();

        cout << front << endl;

        ifstream file(front + "/stat", ifstream::in);
        std::vector<string> parsed(std::istream_iterator<string>(file), {});

        cout << "File: " << front << "/stat\n"; 
        if(!parsed.empty()){
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

            if(exists(front + "/task")){            
                cout << "Queue: " << front << "/task\n";
                directory_has_subprocesses.push(front + "/task");
            }
        }        
    }

    Json::Value tree;
    Json::Value chartValue;
    Json::Value connectors;
    Json::Value style;

    connectors["type"] = Json::Value("step");
    //style["stroke-width"] = Json::Value("2");
    //style["stroke"] = Json::Value("#ccc");
    //connectors["style"] = style;

    chartValue["container"] = Json::Value("#tree-simple");
    chartValue["rootOrientation"] = Json::Value("WEST");
    chartValue["nodeAlign"] = Json::Value("BOTTOM");
    chartValue["connectors"] = connectors;

    tree["chart"] = chartValue;
    
    tree["nodeStructure"] = populate_json(to_string(pid));

    //-------- Writing JSON ------------//
    Json::StyledWriter writer;

    std::ofstream ofl("page/json/tree.json");
    ofl << "var simple_chart_config = ";
    ofl << writer.write(tree);
    ofl << ";";
    ofl.close();

	return 0;
}