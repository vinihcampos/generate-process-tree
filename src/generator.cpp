#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <utility>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp> // system file
#include <thread>	// std::this_thread::sleep_for
#include <chrono>   // std::chrono::seconds
#include "json/json.h"

using namespace std;
using namespace boost::filesystem;

map<string, pair < string, set< string > > > processes;
map<string, pair < string, int > > process_users;

bool is_number(const std::string & s){
    return !s.empty() && std::find_if(s.begin(),s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

Json::Value populate_json(string pid){
	Json::Value name;
	name["name"] = Json::Value(pid + " " + processes[pid].first);

	Json::Value children;
	if(!processes[pid].second.empty()){
		for(auto it = processes[pid].second.begin(); it != processes[pid].second.end(); ++it){
			children.append( populate_json( *it ) );
		}
	}
	Json::Value structure;
	structure["text"] = name;

	Json::Value stroke, style;
	int r = rand()%255;
	int g = rand()%255;
	int b = rand()%255;

	std::stringstream ss;	
	string r_,g_,b_;
	ss << std::hex << r;
	r_ = ss.str(); ss.clear(); ss.str(string());
	ss << std::hex << g;
	g_ = ss.str(); ss.clear(); ss.str(string());
	ss << std::hex << b;
	b_ = ss.str(); ss.clear(); ss.str(string());

	r_.size() == 1 ? r_ = "0" + r_ : r_;
	g_.size() == 1 ? g_ = "0" + g_ : g_;
	b_.size() == 1 ? b_ = "0" + b_ : b_; 

	stroke["stroke"] = Json::Value("#" + r_ + g_ + b_);
	style["style"] = stroke;
	structure["connectors"] = style;	
	if(!children.isNull()) structure["children"] = children; 

	return structure; 
}

void identify_users(int & n_processes){
	ifstream file("/etc/passwd", ifstream::in);
	string input;

	string name, id;

	while(getline(file, input)){
		std::istringstream ss(input);
		std::string token;
		int counter = 1;
		while(getline(ss, token, ':')) {
		    if(counter == 1) name = token;
		    if(counter == 3) id = token;
		    counter++;

		    if(counter > 3) break;
		}

		process_users[id] = make_pair(name, 0);
	}

	file.close();
	if(exists("/proc/self/status")){
		n_processes++;
		file.open("/proc/self/status",ifstream::in);
		string input;
		int counter = 0;
		while(getline(file, input)){
			++counter;
			if(counter == 8){
				stringstream ss(input);
				ss >> input;
				ss >> input;
				process_users[input].second++;
			}				
		}
		file.close();
	}

	if(exists("/proc/thread-self/status")){
		n_processes++;
		file.open("/proc/thread-self/status",ifstream::in);
		string input;
		int counter = 0;
		while(getline(file, input)){
			++counter;
			if(counter == 8){
				stringstream ss(input);
				ss >> input;
				ss >> input;
				process_users[input].second++;
			}				
		}
		file.close();
	}	
}

int check_processes(int & pid, int & sleep_time){
	path p("/proc");

    directory_iterator end_itr;
    
    int n_processes = 0;
    // Identifying users of system
    identify_users(n_processes);

    for (directory_iterator itr(p); itr != end_itr; ++itr) {

    	if( is_directory( itr->path() ) && is_number(itr->path().leaf().string()) ){
    		n_processes++;

    		// Counting process by user
    		ifstream pu(itr->path().string() + "/status", ifstream::in);

    		string input;
			int counter = 0;
			while(getline(pu, input)){
				++counter;
				if(counter == 8){
					stringstream ss(input);
					ss >> input;
					ss >> input;
					process_users[input].second++;
				}				
			}

			// Getting information about PID, PPID and NAME PROCESS
    		ifstream file(itr->path().string() + "/stat", ifstream::in);
    		std::vector<string> parsed(std::istream_iterator<string>(file), {});

    		string PID = "", NAME = "", PPID = "";
    		counter = 1;
    		for(int i = 0; i < parsed.size(); ++i){
    			if((counter == 1 || counter == 3) && is_number(parsed[i])){
    				counter == 1 ? PID = parsed[i] : PPID = parsed[i];
    				counter++;
    			}else if(counter == 2 && !is_number(parsed[i])){
    				while(parsed[i][parsed[i].size()-1] != ')'){
    					NAME += parsed[i++] + " "; 
    				}
    				NAME += parsed[i];
    				counter++;
    			}
    			if(counter > 3) break;
    		} 

    		if( processes.find(  PID )  == processes.end()){
    			processes.insert( make_pair( PID , make_pair( NAME, set<string>() ) ) );
    		}else{
    			processes[  PID ].first = NAME;
    		}

    		if(PPID.compare("0")){
    			if(processes.find(  PPID )  == processes.end()){
	    			processes.insert( make_pair( PPID , make_pair( "", set<string>() ) ) );
	    		}
	    		processes[ PPID ].second.insert( PID );
    		}	
    	}
    }    

    // Json Informations
    Json::Value tree;
    Json::Value chartValue;
    Json::Value connectors;
    Json::Value style;
    Json::Value node;
    Json::Value animation;

    connectors["type"] = Json::Value("step");
    style["stroke-width"] = Json::Value("2");
    style["arrow-end"] = Json::Value("oval-wide-long");
    //style["stroke"] = Json::Value("#ccc");
    connectors["style"] = style;

    node["collapsable"] = Json::Value("true");

    //animation["nodeAnimation"] = Json::Value("easeOutBounce");
    //animation["nodeSpeed"] = Json::Value(700);
    //animation["connectorsAnimation"] = Json::Value("easeOutBounce");
    //animation["connectorsSpeed"] = Json::Value(700);

    chartValue["container"] = Json::Value("#tree-simple");
    chartValue["rootOrientation"] = Json::Value("WEST");
    chartValue["nodeAlign"] = Json::Value("BOTTOM");
    chartValue["connectors"] = connectors;
    chartValue["node"] = node;
    //chartValue["animation"] = animation;
    //chartValue["animateOnInit"] = Json::Value(true);

    tree["chart"] = chartValue;
    
    tree["nodeStructure"] = populate_json(to_string(pid));
    tree["totalProcesses"] = Json::Value(n_processes);

    //Populate processes in json
    Json::Value user;
    for(auto it = process_users.begin(); it != process_users.end(); ++it){
    	user["name"] = it->second.first;
    	user["quantity"] = it->second.second;
    	tree["processes"].append(user);
    }

    //-------- Writing JSON ------------//
    Json::StyledWriter writer;

    std::ofstream ofl("page/json/tree.json");
    ofl << "var reload_time = " << sleep_time << ";" << endl;
    ofl << "var simple_chart_config = ";
    ofl << writer.write(tree);
    ofl << ";";
    ofl.close();

    return n_processes;
}

int main(int argn, char ** argc){
	
	srand (time(NULL));
	int pid = 1;
	int sleep_time = 30;

	if(argn > 1){
		pid = atoi(argc[1]);
	}
	if(argn > 2){
		sleep_time = atoi(argc[2]) * 1000;
	}

	int counter = 0;

	while(1){
		
		cout << "# Number of processes: " << check_processes(pid, sleep_time) << endl;
		cout << "# Number of executions: " << ++counter << endl;
		for(auto it = process_users.begin(); it != process_users.end(); ++it){
			cout << "# PID->" << it->first << " | UNAME->" << it->second.first << " | nº proc-> " << it->second.second << endl;
		}
		
		cout << "############################################\n";
		processes.clear();
		process_users.clear();
		std::this_thread::sleep_for (std::chrono::milliseconds(sleep_time));
	}
	

	return 0;
}