#include <stdio.h>
#include <string>
#include <map>
#include "strtokenizer.h"
#include "utils.h"
#include "model.h"

using namespace std;



//parse the passed arguments from the command lines 
 int utils::parse_args(int argc, char ** argv, model * pmodel)
{
    int model_status = MODEL_STATUS_UNKNOWN;
    string dir = ""     ;
    string model_name="";
    string dfile = ""   ;
    double alpha = -1.0 ;
    double beta = -1.0  ;
    int K = 0           ;
    int niters = 0      ;
    int savestep = 0    ;
    int twords = 0      ;
	bool teval =false	;//get the bool type value to check whether the test  dataset need  to calculate the perplexity.
	bool treval=false   ;//get the bool type value to check whether the train dataset need  to calculate the perplexity.
	//1...argc 传递进来的参数进行解析
    int i = 0; 
    while (i < argc) {
	string arg = argv[i];
	
	if(arg == "-est")
	{
	    model_status = MODEL_STATUS_EST;
	    
	}
	 else if (arg == "-estc") 
	{
	    model_status = MODEL_STATUS_ESTC;
	    
	}
	 else if (arg == "-inf") 
	{
	    model_status = MODEL_STATUS_INF;
	}
	 else if (arg == "-dir") 
	{
	    dir = argv[++i];	    
	}
	 else if (arg == "-dfile") 
	{
	    dfile = argv[++i];	    
	    
	} else if (arg == "-model") {
	    model_name = argv[++i];	    	    
	    
	} else if (arg == "-alpha") {
	    alpha = atof(argv[++i]);	    
	    
	} else if (arg == "-beta") {
	    beta = atof(argv[++i]);	    
	    
	} else if (arg == "-ntopics"){
	    K = atoi(argv[++i]);	    
	    
	} else if (arg == "-niters") {
	    niters = atoi(argv[++i]);	    
	    
	} else if (arg == "-savestep"){
	    savestep = atoi(argv[++i]);
	    
	} else if (arg == "-twords") {
	    twords = atoi(argv[++i]);
	} 
	else if (arg == "-teval")    {
		teval = true;
	}
	else if (arg == "-treval")  {
		treval =true;
	}
	  else 
	{
	    // any more?
	}	
	  i++;
    }
    
	//2.1 加载EST需要的配置参数
     if(model_status == MODEL_STATUS_EST) 
	{
		 if(dfile=="") 
		{
			printf("Please specify the input data file for model estimation!\n");
			return 1;
		}
	
		pmodel->model_status = model_status;
	
	     if(K>0)
	    {
			pmodel->K = K;
		}
	
		if (alpha >= 0.0)
		{
			pmodel->alpha = alpha;
		} 
		else 
		{
			// default value for alpha
			pmodel->alpha = 50.0 / pmodel->K;
		}
	
		if (beta >= 0.0) 
		{
			pmodel->beta = beta;
		}
	
		if (niters > 0) 
		{
			pmodel->niters = niters;
		}
	
		 if(savestep>0)
		{
			pmodel->savestep = savestep;
		}
	
		if(twords>0)
		{
			pmodel->twords = twords;
		}
		
		pmodel->treval = treval;

		pmodel->dfile  = dfile ;
	
		string::size_type idx = dfile.find_last_of("/");
		if(idx == string::npos)
		{
			pmodel->dir = "./";
		}
		 else
		{
			pmodel->dir = dfile.substr(0,idx+1);//[0,idx+1)
			pmodel->dfile = dfile.substr(idx + 1, dfile.size() - pmodel->dir.size());
			printf("dir = %s\n", pmodel->dir.c_str());
			printf("dfile = %s\n", pmodel->dfile.c_str());
		}
	} //end model_status == MODEL_STATUS_EST
    

	 if( model_status == MODEL_STATUS_ESTC ) 
	{
		if(dir == "") 
		{
			printf("Please specify model directory!\n");
			return 1;
		}
	
		if (model_name == "") 
		{
			printf("Please specify model name upon that you want to continue estimating!\n");
			return 1;
		}	

		pmodel->model_status = model_status;

		//如果结尾没有"/"符号添加斜杠符号
		 if(dir[dir.size() - 1] != '/') 
		{
			dir += "/";
		}
		pmodel->dir = dir			   ;
		pmodel->model_name = model_name;

		 if (niters > 0) 
		{
			pmodel->niters = niters;
		}
	
		if (savestep > 0) 
		{
			pmodel->savestep = savestep;
		}
	
		if (twords > 0) 
		{
			pmodel->twords = twords;
		}
	
		pmodel->treval = treval;		
	
		//read <model>.others file to assign values for ntopics,alpha,beta,etc.
		if (read_and_parse(pmodel->dir + pmodel->model_name + pmodel->others_suffix,pmodel))
		{
			return 1;
		}
    } //END MODEL_STATUS_ESTC
    

	//Inference Model for new unseen data...
	if (model_status == MODEL_STATUS_INF) 
	{
		if (dir == "") 
		{
			printf("Please specify model directory please!\n");
			return 1;
		}
	
		if (model_name == "") 
		{
		    printf("Please specify model name for inference!\n");
			return 1;
		}	

		if (dfile == "") 
		{
			printf("Please specify the new data file for inference!\n");
		    return 1;
		}
		

		pmodel->model_status = model_status;

		if (dir[dir.size()-1] != '/' ) 
		{
		    dir += "/";
		}
		pmodel->dir = dir;
	
		pmodel->model_name = model_name;

		pmodel->dfile = dfile;
		
		pmodel->treval = treval;
		pmodel->teval  = teval ;
		 if (niters > 0) 
		{
			pmodel->niters = niters;
		}
		 else
		{
			// default number of Gibbs sampling iterations for doing inference
			pmodel->niters = 20;
		}
	
		 if (twords > 0) 
		{
			pmodel->twords = twords;
		}
	
		 // read <model>.others file to assign values for ntopics, alpha, beta, etc.
		 if(read_and_parse(pmodel->dir + pmodel->model_name + pmodel->others_suffix, pmodel)) 
		{
			return 1;
		}
    }//end MODEL_STATUS_INF
    
 	 if(model_status == MODEL_STATUS_UNKNOWN) 
	{
		printf("Please specify the task you would like to perform (-est/-estc/-inf)!\n");
		return 1;
    }
    return 0;
}


 //Other types except for estimate from scratch need to load the the ".others" file to getcorresponding trained model information. 
 int utils::read_and_parse(string filename, model * pmodel)
{
    // open file <model>.others to read:
    // alpha=?
    // beta=?
    // ntopics=?
    // ndocs=?
    // nwords=?
    // citer=? // current iteration (when the model was saved)
    
    FILE * fin = fopen(filename.c_str(), "r");
     if (!fin) 
	{
		printf("Cannot open file: %s\n", filename.c_str());
		return 1;
    }
    
    char buff[BUFF_SIZE_SHORT];
    string line;
    
     while( fgets(buff, BUFF_SIZE_SHORT-1 , fin ) )
	{
		line = buff;
		strtokenizer strtok(line, "= \t\r\n");
		int count = strtok.count_tokens(); 
		//invalid,ignore this line
		if (count != 2)  
			continue;  
		string optstr = strtok.token(0);//alpha
		string optval = strtok.token(1);//0.250000 
		if (optstr == "alpha")				 //not useful for evaluate the lda.
		{
			pmodel->alpha = atof(optval.c_str());
	    
		} 
		else if (optstr == "beta")			 //not useful for evaluate the lda.
		{	    
			pmodel->beta = atof(optval.c_str());
	
		} 
		else if (optstr == "ntopics") 
		{
			pmodel->K = atoi(optval.c_str());
		} 
		else if (optstr == "ndocs")			  //not useful for evaluate the lda.
		{	   
			pmodel->M = atoi(optval.c_str());
	 
		}
		else if (optstr == "nwords") 
		{
			pmodel->V = atoi(optval.c_str());
		} 
		else if (optstr == "liter")			 //not useful for evaluate the lda.
		{
			pmodel->liter = atoi(optval.c_str());
		} 
		else 
		{
			// any more?
		}
    }
    fclose(fin); 
    return 0;
}

string utils::generate_model_name(int iter)
{
    string model_name = "model-";

    char buff[BUFF_SIZE_SHORT];
    
    if (0 <= iter && iter < 10) {
	sprintf(buff, "0000%d", iter);
    } else if (10 <= iter && iter < 100) {
	sprintf(buff, "000%d", iter);
    } else if (100 <= iter && iter < 1000) {
	sprintf(buff, "00%d", iter);
    } else if (1000 <= iter && iter < 10000) {
	sprintf(buff, "0%d", iter);
    } else {
	sprintf(buff, "%d", iter);
    }
    
    if (iter >= 0) {
	model_name += buff;
    } else {
	model_name += "final";
    }
    
    return model_name;
}

 void utils::sort(vector<double> & probs, vector<int> & words) 
{
    for (int i = 0; i < probs.size() - 1; i++) 
	{
		for (int j = i + 1; j < probs.size(); j++) 
		{
			if (probs[i] < probs[j]) 
			{
				double tempprob = probs[i];
				int tempword = words[i];
				probs[i] = probs[j];
				words[i] = words[j];
				probs[j] = tempprob;
				words[j] = tempword;
	    }
	}
    }
}

//quicksort is used for get topk words from each topic.
void utils::quicksort(vector<pair<int, double> > & vect, int left, int right) {
    int l_hold, r_hold;
    pair<int, double> pivot;
    
    l_hold = left;
    r_hold = right;    
    int pivotidx = left;
    pivot = vect[pivotidx];

    while (left < right) {
	while (vect[right].second <= pivot.second && left < right) {
	    right--;
	}
	if (left != right) {
	    vect[left] = vect[right];
	    left++;
	}
	while (vect[left].second >= pivot.second && left < right) {
	    left++;
	}
	if (left != right) {
	    vect[right] = vect[left];
	    right--;
	}
    }

    vect[left] = pivot;
    pivotidx = left;
    left = l_hold;
    right = r_hold;
    
    if (left < pivotidx) {
	quicksort(vect, left, pivotidx - 1);
    }
    if (right > pivotidx) {
	quicksort(vect, pivotidx + 1, right);
    }    
}

