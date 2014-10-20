#include "csv.h"

using namespace std;

csv::csv(){
}

//let's parse a csv string because FUCK JSON and XML
vector < vector<string> > csv::parse(string values){
  int rows = 0;
  string rowContent[100];
  int cols = 0;
  string columnContent[100];
  int found = -1;
  /* We need to figure out how many rows we have in out CSV string and seprate the CSV string into substrings*/
  while(found != string::npos){
    found = values.find("\n",0);
    rowContent[rows] = values.substr(0,found-1);
    values = values.substr(found+1);
    if(found != string::npos)
      rows++;
  }
  found = -1;
  /* Now we need to figure out how many columns we have and seperate the rows into substrings*/
  for(int i=0; i<rows; i++){
    found = rowContent[i].find(",",0);
    columnContent[cols] = rowContent[i].substr(0, found-1);
    rowContent[i] = rowContent[i].substr(found+1);
    if(found != string::npos)
      cols++;
  }
  vector < vector<string> > ret(rows, vector<string>(cols));
  for(int r=0; r<rows; r++){
    for(int c=0; c<cols; c++){
      ret[r][c]=columnContent[c];
    }
  }
  return ret;
}
//Fuck me that was hard
