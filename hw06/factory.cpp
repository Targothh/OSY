#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
using namespace std;

class worker_t;
vector<worker_t> worker_arr;
mutex product_mutex;
condition_variable product_var;
enum place {
    NUZKY, VRTACKA, OHYBACKA, SVARECKA, LAKOVNA, SROUBOVAK, FREZA,
    _PLACE_COUNT
};

string step_order[3][6] = {
        {"nuzky", "vrtacka", "ohybacka", "svarecka", "vrtacka", "lakovna"},
        {"vrtacka", "nuzky", "freza", "vrtacka", "lakovna", "sroubovak"},
        {"freza", "vrtacka", "sroubovak", "vrtacka", "freza", "lakovna"}
    };

map<string, int> work_time = {
    {"nuzky", 100},{"vrtacka", 200},{"ohybacka", 150},{"svarecka", 300},{"lakovna", 400},{"sroubovak", 250},{"freza", 500}
};
string product_type_to_str[_PLACE_COUNT] = {
    "nuzky", "vrtacka", "ohybacka", "svarecka", "lakovna", "sroubovak", "freza"
};

map<string, int> place_map = {
    {"nuzky", NUZKY}, {"vrtacka", VRTACKA}, {"ohybacka", OHYBACKA},
    {"svarecka", SVARECKA}, {"lakovna", LAKOVNA}, {"sroubovak", SROUBOVAK}, {"freza", FREZA}
};

map<int, int> ready_places;

map<string, int> product_map = {
        {"A", 0}, {"B", 1}, {"C", 2}
    };
map<int, string> product_to_str = {
        {0, "A"}, {1, "B"}, {2, "C"}
    };
/// -------------------------------------------------------------------------------------------------------------------
int product [3][6];
int product_in_work[3][6];
bool end_of_file = false;
class worker_t{
public:
    const string name;
    int prod_id;
    int prod_step;
    int place_spec;
    bool should_work;
    bool end_work;
    thread worker_thread;

    worker_t(const string &name_input, int place_id) : name(name_input), place_spec(place_id), should_work(true), end_work(false) {}

    void start_thread(){
       worker_thread = thread(work_job, this);
    }

    void join_thread() {
        worker_thread.join();
    }

    static bool find_job(void *arg){
        auto* worker = (worker_t*)arg;
        if (worker->end_work || (can_end_check(worker->place_spec) && end_of_file)){
            worker->end_work = true;
            return true;
        }
        for (int i = 5; i >= 0; i--){
            for (int j = 0; j < 3; j++){
                if (product[j][i] != 0 && place_map[step_order[j][i]] == worker->place_spec && ready_places[worker->place_spec] != 0){
                    if (product[j][i] - product_in_work[j][i] != 0){
                        worker->prod_id = j;
                        worker->prod_step = i;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    static void *work_job(void *arg) {
    auto* worker = (worker_t*)arg;
    while (worker->should_work && !worker->end_work) {
        {
            unique_lock<mutex> lock(product_mutex);
            product_var.wait(lock, [&] { return find_job(worker); });
            if (worker->end_work) {
                break;
            }
            stringstream stream;
            stream << worker->name << " " << product_type_to_str[worker->place_spec] 
                   << " " << worker->prod_step + 1 << " " << product_to_str[worker->prod_id] << endl;
            cout << stream.str();
            fflush(stdout);
            ready_places[worker->place_spec]--;
            product_in_work[worker->prod_id][worker->prod_step]++;
        }
        //cout << endl << work_time[step_order[worker->prod_id][worker->prod_step]] << " " << worker->prod_step << " " << worker->prod_id << " " << worker->name << endl;
        usleep(work_time[step_order[worker->prod_id][worker->prod_step]] * 1000);

        {
            unique_lock<mutex> lock(product_mutex);

            if (worker->prod_step == 5) {
                product[worker->prod_id][worker->prod_step]--;
                cout << "done " << product_to_str[worker->prod_id] << endl;
                fflush(stdout);
            } else {
                product[worker->prod_id][worker->prod_step]--;
                product[worker->prod_id][worker->prod_step + 1]++;
            }
            ready_places[worker->place_spec]++;
            product_in_work[worker->prod_id][worker->prod_step]--;
        }

        product_var.notify_all();
        {
        lock_guard<mutex> lock(product_mutex);
        if (end_of_file) {
            if (can_end_check(worker->place_spec)) {
                break;
            }
        }
        }
    }
    stringstream stream;
    stream << worker->name << " goes home" << endl;
    cout << stream.str();
    fflush(stdout);

    return nullptr;
}


    static bool can_end_check(int place_spec){
        if(ready_places[place_spec] == 0) {
            return true;
        }
        for (int i = 5; i >= 0; i--){
            for (int j = 0; j < 3; j++){
                if (place_map[step_order[j][i]] == place_spec){
                    if (product_in_line(j, i)){
                        if (worker_found(j, i)){
                            return false;
                        }
                    }

                }
            }
        }
        return true;
    }
    static bool product_in_line(int j, int i){
        for (int line = i; line >= 0; line --){
            if (product[j][line] >= 1){
                return true;
            }       
        }
        return false;
    }
    static bool worker_found(int j, int i ){ // tady je chyba
        while(i >= 0){
        bool found = false;
            for (auto &worker : worker_arr) {
                if(worker.place_spec == place_map[step_order[j][i]]){
                    i--;
                    found = true;
                    break;
                }
            }
        if (!found){ 
            return false;
        }
        }
        return true;
    }
};

int find_string_in_array(const string* array, int length, string what) {
    for (int i = 0; i < length; ++i) {
        if (array[i] == what) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv)
{
    worker_arr.reserve(50);
    string line;
    while (getline(cin, line)) {
        string cmd, arg1, arg2, arg3;
        istringstream iss(line);
        iss >> cmd >> arg1 >> arg2 >> arg3;

        if (cmd.empty()) {
            continue; /* Empty line */
        } else if (cmd == "start" && !arg1.empty() && !arg2.empty() && arg3.empty()) {
            if (place_map.find(arg2) != place_map.end()){
                {
                lock_guard<mutex> lock(product_mutex);
                worker_arr.emplace_back(arg1, place_map[arg2]);
                worker_arr.back().start_thread();
                }
        
            } else {
                cerr << "invalid place" << endl;
                continue;

            }

        } else if (cmd == "make" && !arg1.empty() && arg2.empty()) {
            if (product_map.find(arg1) != product_map.end()){
                {
                lock_guard<mutex> lock(product_mutex);
                product[product_map[arg1]][0]++;
                product_var.notify_all();
                }
            } else {
                cerr << "invalid product type" << endl;
            }
        } else if (cmd == "end" && !arg1.empty() && arg2.empty()) {
            for (auto &worker : worker_arr){
                if (worker.name == arg1){
                    {
                    lock_guard<mutex> lock(product_mutex);
                    worker.end_work = true;
                    }
                    product_var.notify_all();
                }
            }
        } else if (cmd == "add" && !arg1.empty() && arg2.empty()) { //ADD
            if (place_map.find(arg1) != place_map.end()){
                {
                lock_guard<mutex> lock(product_mutex);
                ready_places[place_map[arg1]]++ ;
                product_var.notify_all();
                }
            } else {
                cerr << "invalid place" << endl;
            }

        } else if (cmd == "remove" && !arg1.empty() && arg2.empty()) { //REMOVE
            if (place_map.find(arg1) != place_map.end()){
                {
                lock_guard<mutex> lock(product_mutex);
                ready_places[place_map[arg1]]-- ;
                }
            } else {
                cerr << "invalid place" << endl;
            }
        } else if (cmd == "EOF"){
            break;
        } else {
            cerr << "invalid command" << endl;
        }
    }
    {
    lock_guard<mutex> lock(product_mutex);
    end_of_file = true;
    }
    product_var.notify_all();
    for (auto &worker : worker_arr) {
        worker.join_thread();
    }
    
    return 0;
}