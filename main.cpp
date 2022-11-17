#include <vector>
#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <boost/crc.hpp>

using namespace std;
using sec = chrono::duration<double>;

class VectorManipulator;
void worker_thread(int current_len, int covered_section);
const vector<char> char_pool = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '_', '+', '=', '{', '}', '[', ']', '|', '\\', ':', ';', '"', '\'', '<', '>', ',', '.', '?', '/', '~', '`'};
vector<char> password_input;
bool found = false;
mutex mtx;
long user_pass_crc;

// Parse command line arguments
void parse_args(int argc, char const* argv[], vector<char> &args) {
    // begin by saving the first argument to a character array
    char const* arg = argv[1];
    // loop through the characters in the array and add them to the vector
    for (int i = 0; i < strlen(arg); i++) {
        args.push_back(arg[i]);
    }
}

// Print the vector
void print_vector(vector<char> &char_vector) {
    for (int i = 0; i < char_vector.size() + 1; i++) {
        cout << char_vector[i];
    }
    cout << endl;
}

long calculate_crc(vector<char> &char_vector) {
    boost::crc_32_type result;
    result.process_bytes(char_vector.data(), char_vector.size());
    return result.checksum();
}


// class for manipulating the vector
class VectorManipulator {
    public:
        // constructor
        VectorManipulator(vector<char> &char_vector) {
            this->char_vector = char_vector;
        }
        VectorManipulator() {
            this->char_vector = vector<char>();
        }
        VectorManipulator(int current_len, int begin_char, int end_char) {
            this->current_len = current_len;
            this->begin_char = begin_char;
            this->end_char = end_char;
            this->char_vector = vector<char>(current_len + 1);
        }
        // methods
        bool compare_vector () {
            long candidate_crc = calculate_crc(this->char_vector);
            if (candidate_crc != user_pass_crc) {
                return false;
            }
            // for (int i = 0; i < this->char_vector.size(); i++) {
            //     if (this->char_vector[i] != candidate_pass[i]) {
            //         return false;
            //     }
            // }
            if (this->char_vector == password_input) {
                return true;
            } else {
                return false;
            }
        }
        // generates all possible passwords of length n
        void generate_passwords(int n) {
            // for (int i = 0; i <= n; i++) {  // since char_vector is initially empty, we need to add an element when i == n as well
            //     this->char_vector.push_back(char_pool[0]);
            // }
            bool success = generate_passwords_rec(char_pool, current_len);
            if (success) {
                mtx.lock();
                found = true;
                mtx.unlock();
                cout << "Password found!  It is: ";
                print_vector(char_vector);
            }
        }

        // recursive function that generates all possible passwords of length n
        bool generate_passwords_rec(vector<char> char_pool, int n) {
            if (n == 0) {
                for (int i = begin_char; i <= end_char; i++) {
                    if (found) {
                        break;
                    } else {
                        char_vector[0] = char_pool[i];
                        if (compare_vector()) {
                            return true;
                        }
                    }
                }
                return false;
            } else {
                for (int i = 0; i < char_pool.size(); i++) {
                    if (found) {
                        break;
                    } else {
                        char_vector[n] = char_pool[i];
                        if (compare_vector()) {
                            return true;
                        } else {
                            bool success = generate_passwords_rec(char_pool, n - 1);
                            if (success) {
                                return true;
                            }
                        }
                    }
                    
                }
                return false;
            }
        }

    private:
        vector<char> char_vector;
        int current_len;
        int begin_char;
        int end_char;
};

// function that spawns four threads to find the password
void cracking_controller() {
    int n = 0;
    while (true) {
        // spawn four threads
        // each thread will try a different password
        // if the password is found, the thread will return true
        // if the password is not found, the thread will return false
        // if all four threads return false, the password is not in the pool
        // if any thread returns true, the password is in the pool
        // if the password is in the pool, the thread will print the password
        // if the password is not in the pool, the thread will print "Password not found"
        // if the password is found, the program will exit
        // if the password is not found, the program will continue
        // if the password is not found, the program will increment n and try again
        // unsigned int* thread1;
        // int pthread_create(thread1, NULL, void *(*start_routine) (void *), void *arg);
        thread t1(worker_thread, n, 1);
        thread t2(worker_thread, n, 2);
        thread t3(worker_thread, n, 3);
        thread t4(worker_thread, n, 4);
        thread t5(worker_thread, n, 5);
        thread t6(worker_thread, n, 6);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        t6.join();
        if (found) {
            break;
        } else {
            n++;
        }
    }
    
}

// thread helper function
void worker_thread (int current_len, int covered_section) {
    int pool_size = char_pool.size();
    auto dv = div(pool_size, 6);
    int slice_size = (int) dv.quot;
    int start = (covered_section - 1) * slice_size;
    int end;
    if (covered_section == 6) {
        end = pool_size;
    } else {
        end = start + slice_size - 1;
    }
    VectorManipulator vm = VectorManipulator(current_len, start, end);
    vm.generate_passwords(current_len);
    
};


//main function
int main(const int argc, const char* argv[])
{
    parse_args(argc, argv, password_input);
    user_pass_crc = calculate_crc(password_input);
    const auto start_time = chrono::system_clock::now();
    cracking_controller();
    const sec duration = chrono::system_clock::now() - start_time;
    cout << "Time taken to crack password: " << duration.count() << " seconds" << endl;
    return 0;
}