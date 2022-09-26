#include <iostream>
#include <vector>
#include <fstream> 
#include <stdio.h>
#include <string.h>
#include <sstream>

using namespace std ;

int gmethod = 0 ;

struct cFrame {
  char ch ; // page reference string 
  int count ; // counter 
};

struct frame{
	char pageNum ; // page reference string 
	bool pageFault ;
	vector<cFrame> frameContent ; // page frame
};

class pageReplacement{
	private:
		int gframeNum = 0 ; // frame 數量 
		int PageReplaces = 0 ; // Page Replaces次數 
		int PageFault = 0 ; //  Page Fault 次數 
		vector<cFrame> currentFrame ; // 目前frame的內容 
		vector<frame> vec ; // 所有data 
	public:
		void ReadFile(string &filename) ;
		void FIFO() ;
		bool PageInCurrentFrame(char ch) ;
		void LRU() ;
		bool PageInCurrentFrame_LRU(char ch, int &index) ; 
		void LFU_LRU() ;
		int findLFU_LRU() ;
		void MFU_FIFO() ;
		int findMFU_FIFO() ;
		void MFU_LRU() ;
		void ALL(string filename) ;
		void initialize() ;
		void writeFile(string filename, string method, int i) ;
		void writeALLFile(string filename, string method) ;
		
};

void pageReplacement::ReadFile(string &filename){
	fstream file ;
	frame f ;
    char ss[5] = ".txt" ;
    char ff[20] = "\0" ;
    strcpy(ff,filename.c_str()) ;
    strcat(ff,ss) ;
	file.open (ff,ios::in) ;
	while(!file) { // file not found
		cout << "file not found!" << endl ;
		cout << "Input a file number: " ;
		cin >> ff ;
		filename = ff ;
        strcat(ff,ss) ;
		file.open(ff,ios::in) ;
	}
	file >> gmethod ;
	file >> gframeNum ;
	
	while( !file.eof() ){
		file >> f.pageNum ;
		if( f.pageNum >= '0' && f.pageNum <= '9'){
			f.pageFault = false ;
			f.frameContent.clear() ;
			vec.push_back(f) ;
		}
		f.pageNum = '\0' ;
	}

}

// =========== FIFO ================

bool pageReplacement::PageInCurrentFrame(char ch) { // FIFO
	
    for( int i = 0; i < currentFrame.size(); i++ ) {
    	if ( ch == currentFrame.at(i).ch ) {
            return true ;
    	}
    }
    return false ;
} 

void pageReplacement::FIFO(){
	int i = 0 ;
	int index = 0 ;
	cFrame f ;
	while( i < vec.size() ){
		f.ch = vec.at(i).pageNum ;
		if( PageInCurrentFrame(vec.at(i).pageNum) == true ){ // 已在frame裡 
			;
		}
		else{ // page fault
			PageFault++ ;
			vec.at(i).pageFault = true ;
			if( currentFrame.size() < gframeNum ){
				currentFrame.push_back(f) ;
			}
			else{ // replace
				PageReplaces++ ;
				currentFrame.erase(currentFrame.begin()) ;
				currentFrame.push_back(f) ;
			}
		}
		vec.at(i).frameContent.assign(currentFrame.begin(), currentFrame.end()) ;
		i++ ;
	}
}

// =========== LRU ================

bool pageReplacement::PageInCurrentFrame_LRU(char ch, int &index) { // LRU
	
    for( int i = 0; i < currentFrame.size(); i++ ) {
    	if ( ch == currentFrame.at(i).ch ) {
    		index = i ;
            return true ;
    	}
    }
    return false ;
} 

void pageReplacement::LRU(){
	int i = 0 ;
	int index = 0 ;
	cFrame f ;
	while( i < vec.size() ){
		f.ch = vec.at(i).pageNum ;
		if( PageInCurrentFrame_LRU(vec.at(i).pageNum,index) == true ){ // 已在frame裡
			currentFrame.erase(currentFrame.begin()+index) ;
			currentFrame.push_back(f) ;
		}
		else{ // page fault
			PageFault++ ;
			vec.at(i).pageFault = true ;
			if( currentFrame.size() < gframeNum ){
				currentFrame.push_back(f) ;
			}
			else{ // replace
				PageReplaces++ ;
				currentFrame.erase(currentFrame.begin()) ;
				currentFrame.push_back(f) ;
			}
		}
		vec.at(i).frameContent.assign(currentFrame.begin(), currentFrame.end()) ;
		i++ ;
	}
}

// =========== LFU+LRU ================

int pageReplacement::findLFU_LRU(){

	int minRef = currentFrame.at(0).count ;
	int index = 0 ;
    for( int i = 0; i < currentFrame.size(); i++ ) {
      if ( minRef > currentFrame.at(i).count ) {
        minRef = currentFrame.at(i).count ;
        index = i ;
      }
    }
	  
	return index ;
 
}

void pageReplacement::LFU_LRU(){
	int i = 0 ;
	int index = 0 ;
	cFrame f ;

	while( i < vec.size() ){
		f.ch = vec.at(i).pageNum ;
		f.count = 1 ;
		if( PageInCurrentFrame_LRU(vec.at(i).pageNum,index) == true ){ // 已在frame裡
			f.count = currentFrame.at(index).count + 1 ;
			currentFrame.erase(currentFrame.begin()+index) ;
			currentFrame.push_back(f) ;
		}
		else{ // page fault

			PageFault++ ;
			vec.at(i).pageFault = true ;
			if( currentFrame.size() < gframeNum ){
				currentFrame.push_back(f) ;
			}
			else{ // replace
				
				PageReplaces++ ;
				index = findLFU_LRU() ;
				currentFrame.erase(currentFrame.begin() + index ) ;
				currentFrame.push_back(f) ;				
				
			}
		}
		vec.at(i).frameContent.assign(currentFrame.begin(), currentFrame.end()) ;
		i++ ;
	}
}

// =========== MFU + FIFO ================

int pageReplacement::findMFU_FIFO(){

	int maxRef = currentFrame.at(0).count ;
	int index = 0 ;
    for( int i = 0; i < currentFrame.size(); i++ ) {
      if ( maxRef < currentFrame.at(i).count ) {
        maxRef = currentFrame.at(i).count ;
        index = i ;
      }
    }
	  
	return index ;
 
}

void pageReplacement::MFU_FIFO(){
	int i = 0 ;
	int index = 0 ;
	cFrame f ;

	while( i < vec.size() ){
		
		f.ch = vec.at(i).pageNum ;
		f.count = 1 ;
		if( PageInCurrentFrame_LRU(vec.at(i).pageNum,index) == true ){ // 已在frame裡
			currentFrame.at(index).count = currentFrame.at(index).count + 1 ;
		}
		else{ // page fault

			PageFault++ ;
			vec.at(i).pageFault = true ;
			if( currentFrame.size() < gframeNum ){
				currentFrame.push_back(f) ;
			}
			else{ // replace
				
				PageReplaces++ ;
				index = findMFU_FIFO() ;
				currentFrame.erase(currentFrame.begin() + index ) ;
				currentFrame.push_back(f) ;				
				
			}
		}
		vec.at(i).frameContent.assign(currentFrame.begin(), currentFrame.end()) ;
		i++ ;
	}
	
}

// =========== MFU + LRU ================

void pageReplacement::MFU_LRU(){
	int i = 0 ;
	int index = 0 ;
	cFrame f ;

	while( i < vec.size() ){
		
		f.ch = vec.at(i).pageNum ;
		f.count = 1 ;
		if( PageInCurrentFrame_LRU(vec.at(i).pageNum,index) == true ){ // 已在frame裡
			f.count = currentFrame.at(index).count + 1 ;
			currentFrame.erase(currentFrame.begin()+index) ;
			currentFrame.push_back(f) ;
		}
		else{ // page fault

			PageFault++ ;
			vec.at(i).pageFault = true ;
			if( currentFrame.size() < gframeNum ){
				currentFrame.push_back(f) ;
			}
			else{ // replace
				
				PageReplaces++ ;
				index = findMFU_FIFO() ;
				currentFrame.erase(currentFrame.begin() + index ) ;
				currentFrame.push_back(f) ;				
				
			}
		}
		vec.at(i).frameContent.assign(currentFrame.begin(), currentFrame.end()) ;
		i++ ;
	}
}

// =========================================

void pageReplacement::ALL(string filename){ // ios::ate
    vector <frame> tt ; // 暫存原始資料
    tt.assign(vec.begin(), vec.end()) ;
	FIFO() ;
	writeFile(filename,"FIFO",1) ;
	vec.assign(tt.begin(), tt.end()) ;
	LRU() ;
	writeALLFile(filename,"LRU") ;
	vec.assign(tt.begin(), tt.end()) ;
	LFU_LRU() ;
	writeALLFile(filename,"Least Frequently Used LRU Page Replacement") ;
	vec.assign(tt.begin(), tt.end()) ;		
	MFU_FIFO() ;
	writeALLFile(filename,"Most Frequently Used Page Replacement ") ;
	vec.assign(tt.begin(), tt.end()) ;
	MFU_LRU() ;
	writeALLFile(filename,"Most Frequently Used LRU Page Replacement ") ;
	
}

void pageReplacement::writeFile(string filename, string method, int i) {
    fstream file ;
    string str = "out_" + filename + ".txt" ;
    file.open(str.c_str(),ios::out) ;
    file << "--------------" << method << "-----------------------" << endl ;
	for( int i = 0; i < vec.size(); i++ ){
		file << vec.at(i).pageNum << "\t" ;
		for( int j = vec.at(i).frameContent.size()-1; j >= 0; j-- ){
			file << vec.at(i).frameContent.at(j).ch ;
		}
		if( vec.at(i).pageFault == true )
			file << "\tF" << endl ;
		else
		    file << endl ;
	}
	file << "Page Fault = " << PageFault << "  " << "Page Replaces = " << PageReplaces << "  " 
	     << "Page Frames = " << gframeNum << endl ;
	if( i == 0 ){
		file << endl ;
	}
    file.close() ;
    initialize() ;
} 

void pageReplacement::initialize(){
	PageReplaces = 0 ;
	PageFault = 0 ;
	currentFrame.clear() ;
	vec.clear() ;	
}
void pageReplacement::writeALLFile(string filename, string method) {
    fstream file ;
    string str = "out_" + filename + ".txt" ;
    file.open(str.c_str(),ios::out|ios::app) ;
    file << endl << "--------------" << method << "-----------------------" << endl ;
	for( int i = 0; i < vec.size(); i++ ){
		file << vec.at(i).pageNum << "\t" ;
		for( int j = vec.at(i).frameContent.size()-1; j >= 0; j-- ){
			file << vec.at(i).frameContent.at(j).ch ;
		}
		if( vec.at(i).pageFault == true )
			file << "\tF" << endl ;
		else
		    file << endl ;
	}
	file << "Page Fault = " << PageFault << "  " << "Page Replaces = " << PageReplaces << "  " 
	     << "Page Frames = " << gframeNum << endl ;

    file.close() ;
    initialize() ;
} 

int main(void) {
	string filename = "\0" ;
	pageReplacement p ;
	do {
	    cout << endl << "Input a file name [0: Quit]: " ;
	    cin >> filename ;
	    if( filename == "0" ){
	    	break ;
		}
		else{
            p.ReadFile(filename) ;
            if( gmethod == 1 ){  	
  				p.FIFO() ;
  				p.writeFile(filename,"FIFO",0) ;
			} 
			else if( gmethod == 2 ){
				p.LRU() ;
				p.writeFile(filename,"LRU",0) ;
			}
			else if( gmethod == 3 ){
				p.LFU_LRU() ;
				p.writeFile(filename,"Least Frequently Used LRU Page Replacement",0) ;
			}
			else if( gmethod == 4 ){
				p.MFU_FIFO() ;
				p.writeFile(filename,"Most Frequently Used Page Replacement ",0) ;
			}
			else if( gmethod == 5 ){
				p.MFU_LRU() ;
				p.writeFile(filename,"Most Frequently Used LRU Page Replacement ",0) ;
			}
			else if( gmethod == 6 ){
				p.ALL(filename) ;
			}
			else{
				break ;
			}

		}

	} while (filename != "0"); 

	return 0;
} // end main
