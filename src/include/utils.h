#ifndef _UTILS_H
#define _UTILS_H

#include <string>

using namespace std;

class model;

 class utils
{
	public:
    // parse command line arguments
    static int parse_args(int argc, char ** argv, model * pmodel);
    
    // read and parse model parameters from <model_name>.others
    static int read_and_parse(string filename, model * model); 
    
    // generate the model name for the current iteration
    // iter = -1 => final model
    static string generate_model_name(int iter);  
    // sort    
    static void sort(vector<double> & probs, vector<int> & words);
    static void quicksort(vector<pair<int, double> > & vect, int left, int right);
};
#endif

