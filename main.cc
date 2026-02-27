#include "flow_field.hpp"


int main(){

    // Create a Matrix
    GenerateFlowField main_template(0,0,8,8);

    main_template.Generate();


    main_template.Print_Cost_Field();
    std::cout << std::endl;
    main_template.Print_Integration_Field();
        std::cout << std::endl;
    main_template.Print_Flow_Field();

    return 0;
}