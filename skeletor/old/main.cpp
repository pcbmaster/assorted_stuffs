/*////////////////////////////////////////////////
| Fun fact! if you type rm *>o instead of rm *.o |
| all your files are deleted and a file named o  |
| is created!                                    |
| Skeletor - main.cpp                            |
| Brett Monty 2013                               |
////////////////////////////////////////////////*/

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <csv.h>

using namespace std;

static int numfields = 0;
static string fields[3]; //We will only ever have 3 fields so we can statically allocate this array

struct File {
  const char *filename;
  FILE *stream;
};

void cb1 (void *s, size_t i, void *p){
  string d;
  csv_write2(&d, i, s, i, '\0');
  fields[numfields] = d;
  numfields++;
}
 
static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct File *out=(struct File *)stream;
  if(out && !out->stream) {
    /* open file for writing */ 
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1; /* failure, can't open file to write */ 
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

void cb2(int c, void *p){
  numfields = 0;
  cout << fields[0] << endl << fields[1] << endl << fields[2] << endl;
  //we are done reading a row of the file
  if(fields[0] == "FILE"){
    CURL *curl;
    CURLcode res;
    const char* URL = fields[1].c_str();
    const char* PATH = fields[2].c_str();
    struct File dl={
      PATH,
      NULL,
    };
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, URL);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dl);
      res = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
      if(CURLE_OK != res) {
	cout << "Failed to fetch" << fields[2] << " as was instructed.\n"; 
      }
      else{
	cout << "Fetch of " << fields[2] << " was successful!\n";
      }
    }
    if(dl.stream)
      fclose(dl.stream);
    curl_global_cleanup();
  }
}

void do_update(int i){
  signal(SIGUSR1, do_update);
  cout << "Fetching update...\n";
  //Fetch update files
  CURL *curl;
  CURLcode res;
  struct File update={
    "update.csv",
    NULL
  };
  curl_global_init(CURL_GLOBAL_DEFAULT);
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/skulls/update.csv");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &update);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if(CURLE_OK != res) {
      cout << "Failed to fetch the update file.\n"; 
    }
    else{
      cout << "Fetch successful!\n\n";
    }
  }
  if(update.stream)
    fclose(update.stream);
  curl_global_cleanup();
  struct csv_parser p;
  csv_init(&p, 0);
  char c;
  ifstream fs ("update.csv");
  if (fs.is_open())
    {
      string buf((istreambuf_iterator<char>(fs)), istreambuf_iterator<char>());
      for(int i=0; i<buf.length(); i++){
	c = buf[i];
	if(csv_parse(&p, &c, 1, cb1, cb2, NULL) != 1)
	  cout << "Error while parsing! " << csv_strerror(csv_error(&p)) << "\n";
      }
      csv_fini(&p,cb1,cb2,NULL);
      fs.close();
    }
  else{
    cout << "failed to open update.csv!\n";
  }
  csv_free(&p);
}

int main(void){
  signal(SIGUSR1, do_update);
  cout << "Skeletor v0.01 started.\n";
  do_update(0);
  /*
  while(1){  //We don't want the daemon to exit, so we create an infinite loop
    sleep(15); //If we don't wait a it before looping, we will be constantly eating up CPU time!
    }*/
}
