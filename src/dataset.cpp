#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "strtokenizer.h"
#include "dataset.h"

using namespace std;

//write  training dataset words map  to the wordmapfile
 int dataset::write_wordmap(string wordmapfile, mapword2id * pword2id) 
{
    FILE * fout = fopen(wordmapfile.c_str(), "w");
     if (!fout)
	{
		printf("Cannot open file %s to write!\n", wordmapfile.c_str());
		return 1;
    }
    
    mapword2id::iterator it;
    fprintf(fout, "%d\n", pword2id->size());
     for(it = pword2id->begin(); it != pword2id->end(); it++) 
	{
		fprintf(fout, "%s %d\n", (it->first).c_str(), it->second);
    }
    fclose(fout);
    return 0;
}

int dataset::read_wordmap(string wordmapfile, mapword2id * pword2id) 
{
    pword2id->clear();
    
    FILE * fin = fopen(wordmapfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", wordmapfile.c_str());
	return 1;
    }    
    
    char buff[BUFF_SIZE_SHORT];
    string line;
    
    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int nwords = atoi(buff);
    
    for (int i = 0; i < nwords; i++) 
	{
		fgets(buff, BUFF_SIZE_SHORT - 1, fin);
		line = buff;
	
		strtokenizer strtok(line, " \t\r\n");
		if (strtok.count_tokens() != 2) 
		{
			continue;
		}
		pword2id->insert(pair<string,int>(strtok.token(0), atoi(strtok.token(1).c_str())));
    }
    fclose(fin);
    return 0;
} 




int dataset::read_wordmap(string wordmapfile , mapid2word * pid2word)
{
    pid2word->clear();    
    FILE * fin = fopen(wordmapfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", wordmapfile.c_str());
	return 1;
    }    
    
    char buff[BUFF_SIZE_SHORT];
    string line;
    
    fgets(buff, BUFF_SIZE_SHORT - 1, fin);
    int nwords = atoi(buff);
    
    for (int i = 0; i < nwords; i++) {
	fgets(buff, BUFF_SIZE_SHORT - 1, fin);
	line = buff;
	
	strtokenizer strtok(line, " \t\r\n");
	if (strtok.count_tokens() != 2) {
	    continue;
	}
	
	pid2word->insert(pair<int, string>(atoi(strtok.token(1).c_str()), strtok.token(0)));
    }
    
    fclose(fin);
    
    return 0;
}


//read training dataset...
int dataset::read_trndata(string dfile, string wordmapfile)
{
    mapword2id word2id;    
    FILE * fin = fopen(dfile.c_str(), "r");
    if (!fin) {
	printf("Cannot open file %s to read!\n", dfile.c_str());
	return 1;
    }   
    
    mapword2id::iterator it;    
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get the number of documents
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
    M = atoi(buff);
    if (M <= 0) {
	printf("No document available!\n");
	return 1;
    }
    
    // allocate memory for corpus
    if (docs) {
	deallocate();
    } else {
	docs = new document*[M];
    }
    
    // set number of words to zero
    V = 0;
    
    for (int i = 0; i < M; i++) {
	fgets(buff, BUFF_SIZE_LONG - 1, fin);
	line = buff;
	strtokenizer strtok(line, " \t\r\n");
	int length = strtok.count_tokens();

	if (length <= 0) {
	    printf("Invalid (empty) document!\n");
	    deallocate();
	    M = V = 0;
	    return 1;
	}
	
	// allocate new document
	document * pdoc = new document(length);
	
	for (int j = 0; j < length; j++) {
	    it = word2id.find(strtok.token(j));
	    if (it == word2id.end()) {
		// word not found, i.e., new word
		pdoc->words[j] = word2id.size();
		word2id.insert(pair<string, int>(strtok.token(j), word2id.size()));
	    } else {
		pdoc->words[j] = it->second;
	    }
	}
	
	// add new doc to the corpus
	add_doc(pdoc, i);
    }
    
    fclose(fin);
    
    // write word map to file
    if (write_wordmap(wordmapfile, &word2id)) {
	return 1;
    }
    
    // update number of words
    V = word2id.size();
    
    return 0;
}

//when estc or inference model need to use this function.
 int dataset::read_newdata( string dfile , string wordmapfile )
{
    mapword2id word2id;
    map<int, int> id2_id;
    
	//1.load wordmapfile trained in etc model to memory. and push the data to the structure of map<string,int> word2id.
	//this map is used to check whether the word in  unseen data is also have exsited in before training data .
    read_wordmap(wordmapfile, &word2id);
    if(word2id.size() <= 0)
	{
		printf("No word map available!\n");
		return 1;
    }

	//2.read new  unseen documents...
    FILE * fin = fopen(dfile.c_str(), "r");
     if(!fin) 
	{
		printf("Cannot open file %s to read!\n", dfile.c_str());
		return 1;
    }

    mapword2id::iterator it;
    map<int, int>::iterator _it;
    char buff[BUFF_SIZE_LONG];
    string line;
    
    // get number of new documents
    fgets(buff, BUFF_SIZE_LONG - 1, fin);
    M = atoi(buff);
    if (M <= 0) 
	{
		printf("No document available!\n");
		return 1;
    }
    
    // allocate memory for corpus
    if (docs) 
	{
		deallocate();
    } 
	else 
	{
		docs = new document*[M];
    }
	_docs = new document*[M];
    
    // set number of words to zero
    V = 0; 
    for (int i = 0; i < M; i++) 
	{
		fgets(buff, BUFF_SIZE_LONG - 1, fin);
		line = buff;
		strtokenizer strtok(line, " \t\r\n");
		int length = strtok.count_tokens()  ;
	
		vector<int> doc ; 
		vector<int> _doc;
		for (int j = 0; j < length; j++)
		{
			it = word2id.find(strtok.token(j));	
			if (it == word2id.end())			
			{
				// word not found, i.e., word unseen in training data
				// do anything? (future decision)
			} 
			 else 
			{
				int _id;
				_it = id2_id.find(it->second);	
				 if(_it == id2_id.end())		
				{
					_id = id2_id.size();
					id2_id.insert(pair<int, int>(it->second, _id));
					_id2id.insert(pair<int, int>(_id,it->second)) ;//
				}
				 else 
				{
					_id = _it->second;
				}
				doc.push_back(it->second);	
				_doc.push_back(_id);		
			} //else end...

		}//for end...
	
		// allocate memory for new doc
		document * pdoc = new document(doc);
		document * _pdoc = new document(_doc);
	
		// add new doc
		add_doc(pdoc,i);
		_add_doc(_pdoc,i);
    }	//for i<M end
    
    fclose(fin);
    
    // update number of new words
    V = id2_id.size();//the number of term in unseen data.
    
    return 0;
}

 


 
