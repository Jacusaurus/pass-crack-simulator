#include <vector>
#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <boost/crc.hpp>


using namespace std;
using sec = chrono::duration<double>;

const vector<char> char_pool = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '_', '+', '=', '{', '}', '[', ']', '|', '\\', ':', ';', '"', '\'', '<', '>', ',', '.', '?', '/', '~', '`'};
vector<char> password_input;
bool found = false;
mutex mtx;
long user_pass_crc;

// prototypes
void parse_args(int argc, char const* argv[], vector<char> &args);
void print_vector(vector<char> &char_vector);
long calculate_crc(vector<char> &char_vector);
class VectorManipulator;
void worker_thread(int current_len, int covered_section);
void cracking_controller(void);
void worker_thread (int current_len, int covered_section);


// Parse command line arguments
void parse_args(int argc, char const* argv[], vector<char> &args) {
    // begin by saving the first argument to a character array
    char const* arg = argv[1];
    // loop through the characters in the array and add them to the vector
    for (int i = 0; i < (int) strlen(arg); i++) {   // we cast to int here and other locations to avoid sing compare warnings from -Wall
        args.push_back(arg[i]);
    }
}

// prints the vector, character by character
void print_vector(vector<char> &char_vector) {
    for (int i = 0; i < (int) char_vector.size() + 1; i++) {
        cout << char_vector[i];
    }
    cout << endl;
}

// calculates the crc of a character vector
long calculate_crc(vector<char> &char_vector) {
    boost::crc_32_type result;
    result.process_bytes(char_vector.data(), char_vector.size());
    return result.checksum();
}


// class for manipulating the vector
class VectorManipulator {
    public:
        // constructors
        VectorManipulator() {
            this->char_vector = vector<char>();
        }
        VectorManipulator(int current_len, int begin_char, int end_char) {
            this->current_len = current_len;
            this->begin_char = begin_char;
            this->end_char = end_char;
            this->char_vector = vector<char>(current_len + 1);
        }
        // destructor
        ~VectorManipulator() {
            char_vector.clear();    // clearing the vector is automatically handled, but it can't hurt to do so manually
        }
        // begins generation of vector to find password
        void begin_generation(int n) {
            bool success = generate_passwords_rec(char_pool, current_len);
            if (success) { // lock the mutex to prevent corruption due to write after write
                mtx.lock(); // since this is a simple program, we're not worried about read after write (which would only return true if altered anyway), which is why we don't lock the mutex in the compare_vector function.
                found = true;
                mtx.unlock();
                cout << "Password found!  It is: ";
                print_vector(char_vector);
            }
        }

    private:
        vector<char> char_vector;
        int current_len;
        int begin_char;
        int end_char;

        // compares the object's vector to the user's input
        bool compare_vector () {
            // calculates CRC of the vector to simulate a hash (which is much slower), which would be performed on the password in a real application
            long candidate_crc = calculate_crc(this->char_vector);
            if (candidate_crc != user_pass_crc) {   // if the CRCs don't match, neither do the passwords
                return false;
            }
            if (this->char_vector == password_input) {  // unfortunately, since we're using a CRC, which *can* have collisions, we need to compare the vectors to make sure they're the same
                return true;
            } else {
                return false;
            }
        }

        // recursive function that generates all possible passwords of length n.  This is a brute force approach.
        bool generate_passwords_rec(vector<char> char_pool, int n) {    // unfortunately, I didn't have time to implement a more efficient algorithm and had to resort to using recursion
            if (n == 0) {   // base case
                // each thread has a different range of characters to check for the first character
                for (int i = begin_char; i <= end_char; i++) {
                    if (found) {    // if the password has already been found, stop the thread
                        break;
                    } else {
                        char_vector[0] = char_pool[i];
                        if (compare_vector()) {
                            return true;
                        }
                    }
                }
                return false;   // if the password hasn't been found, return false so thread can terminate
            } else {    // recursive case
                // for all other characters, each thread has the same range of characters to check at each position in the vector
                for (int i = 0; i < (int) char_pool.size(); i++) {
                    if (found) {
                        break;
                    } else {
                        char_vector[n] = char_pool[i];  // we use the index operator here because we know we're in the bounds of the vector and the check using .at() would just add additional overhead
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
};

// function that spawns six threads to find the password
void cracking_controller() {
    int n = 0;
    // spawn six threads, each with a different range of characters to check for the first character.  Each loop increases the length of the password by one.
    while (true) {
        //Why six threads?  Because it seemed like it offered a decent performance boost compared to five and the gains from seven weren't as significant.
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
    int slice_size = (int) dv.quot; // this seemed to be the most proper way to obtain a quotient
    int start = (covered_section - 1) * slice_size; // since the sections are 1-indexed, we subtract 1 from the covered section to get the correct start index
    int end;
    if (covered_section == 6) {
        end = pool_size;    // in case the pool size isn't divisible by 6, we want to make sure the last thread covers the entire remaining range
    } else {
        end = start + slice_size - 1;   // we subtract 1 from the end index to make sure we don't include the start index of the next section
    }
    VectorManipulator vm = VectorManipulator(current_len, start, end);  // create a new VectorManipulator object for each thread
    vm.begin_generation(current_len);   // begin the generation of passwords
};


//main function
int main(const int argc, const char* argv[])
{
    parse_args(argc, argv, password_input); // parse password input from command line
    user_pass_crc = calculate_crc(password_input);
    const auto start_time = chrono::system_clock::now();
    cracking_controller();
    const sec duration = chrono::system_clock::now() - start_time;  // calculates wall clock time to find password
    cout << "Time taken to crack password: " << duration.count() << " seconds" << endl;
    return 0;
}