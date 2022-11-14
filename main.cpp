#include <vector>
#include <iostream>
#include <cstring>

using namespace std;

// Parse command line arguments
void parse_args(int argc, char const* argv[], vector<char> &args) {
    // begin by saving the first argument to a character array
    char const* arg = argv[1];
    // loop through the characters in the array and add them to the vector
    for (int i = 0; i < strlen(arg); i++) {
        args.push_back(arg[i]);
    }
    //args.pop_back(); // remove the null terminator
}

// Print the vector
void print_vector(vector<char> &char_vector) {
    for (int i = 0; i < char_vector.size() + 1; i++) {
        cout << char_vector[i];
    }
    cout << endl;
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
        // methods
        bool compare_vector (vector<char> &user_pass) {
            if (this->char_vector.size() != user_pass.size()) {
                return false;
            }
            // for (int i = 0; i < this->char_vector.size(); i++) {
            //     if (this->char_vector[i] != user_pass[i]) {
            //         return false;
            //     }
            // }
            if (this->char_vector == user_pass) {
                return true;
            } else {
                return false;
            }
        }
        // generates all possible passwords of length n
        void generate_passwords(vector<char> &user_pass, vector<char> char_pool) {
            int n = 0;
            char_vector.push_back(char_pool[0]);
            while (n < 1000) {
                bool found = generate_passwords_rec(user_pass, char_pool, n);
                if (found) {
                    break;
                } else {
                    n++;
                    char_vector.push_back(char_pool[0]);
                    print_vector(char_vector);
                }
            }
            cout << "Password found! It is: ";
            print_vector(char_vector);
        }

        // recursive function that generates all possible passwords of length n
        bool generate_passwords_rec(vector<char> &user_pass, vector<char> char_pool, int n) {
            if (n == -1) {
                return compare_vector(user_pass);
            } else {
                for (int i = 0; i < char_pool.size(); i++) {
                    char_vector[n] = char_pool[i];
                    if (compare_vector(user_pass)) {
                        return true;
                    } else {
                        bool success = generate_passwords_rec(user_pass, char_pool, n - 1);
                        if (success) {
                            return true;
                        }
                    }
                }
                return false;
            }
        }

    private:
        vector<char> char_vector;
};


//main function
int main(const int argc, const char* argv[])
{
    vector<char> character_pool = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '_', '+', '=', '{', '}', '[', ']', '|', '\\', ':', ';', '"', '\'', '<', '>', ',', '.', '?', '/', '~', '`'};
    vector<char> password_input;
    parse_args(argc, argv, password_input);
    print_vector(password_input);
    VectorManipulator vm = VectorManipulator();
    vm.generate_passwords(password_input, character_pool);
    return 0;
}