#include <iostream>
#include <string>

using namespace std;

typedef struct image_content{
    string data_inst ;
    image_content *prev = nullptr;
    image_content *next = nullptr;
}image_content;

