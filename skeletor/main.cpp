/*////////////////////////////////////////////////
| Fun fact! if you type rm *>o instead of rm *.o |
| all your files are deleted and a file named o  |
| is created!                                    |
| Skeletor - main.cpp                            |
| Brett Monty 2013                               |
////////////////////////////////////////////////*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <curl/curl.h>
#include <csv_parser/csv_parser.hpp>

using namespace std;
vector<string> statlog;

struct File {
  const char *filename;
  FILE *stream;
};
 
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

void do_update(int i){
  signal(SIGUSR1, do_update);
  syslog(LOG_INFO, "Fetching update...\n");
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
      syslog(LOG_INFO, "Failed to fetch the update file.\n"); 
    }
    else{
      syslog(LOG_INFO, "Fetch successful!\n");
    }
  }
  if(update.stream)
    fclose(update.stream);
  curl_global_cleanup();
  csv_parser csv;
  csv.init("update.csv");
  csv.set_enclosed_char('"', ENCLOSURE_OPTIONAL);
  csv.set_field_term_char(',');
  csv.set_line_term_char('\n');
  while(csv.has_more_rows()){
    csv_row row = csv.get_row();
    if(row[0] == "FILE"){
      //fetch file
      const char * URL = row[1].c_str();
      const char * Path = row[2].c_str();
      CURL *curl;
      CURLcode res;
      struct File dl={
	Path,
	NULL
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
	  string logm = "Failed to fetch file: " + row[1].substr(row[1].find_last_of('/')+1) + "\n";
	  syslog(LOG_ERR, logm.c_str());
	}
	else{
	  string logm = "File " + row[1].substr(row[1].find_last_of('/')+1) + " fetched succesfully.\n";
	  syslog(LOG_INFO, logm.c_str());
	}
      }
      if(dl.stream)
	fclose(dl.stream);
      curl_global_cleanup();
    }
    if (row[0] == "CMD"){
      try{
	system(row[1].c_str());
      }
      catch (int e){
	string logm = "Command " + row[1] + " failed! THIS IS BAD!\n";
	syslog(LOG_ERR, logm.c_str());
      }
    }
  }
}

int buffer_clear(void){
  statlog.clear();
  if(statlog.size() > 0)
    return -1;
  else
    return 0;
}

void buffer_add(string *d){ //Probably can be deleted, no clue what this is
  statlog.push_back(*d);
}

void compile_log(void){ //Logging?
  ofstream fs;
  fs.open("/home/brett/www/skulls/report.txt");
  fs.close();
  if((buffer_clear())!=0)
    syslog(LOG_ERR, "Failed to clear statistics buffer! THIS IS BAD!!!\n");
}

int main(void){
  openlog("skeletor",LOG_NOWAIT|LOG_PID,LOG_USER);
  syslog(LOG_INFO, "Skeletor v0.5 started.\n");
  signal(SIGUSR1, do_update);
  pid_t pid, sid;
  pid = fork();
  if (pid < 0) { syslog(LOG_ERR, "Failed to fork child process! Exiting...\n"); exit(EXIT_FAILURE); }
  if (pid > 0) { exit(EXIT_SUCCESS); }
  umask(0);
  if (sid < 0) { syslog(LOG_ERR, "Failed to get unique SID! Exiting...\n"); exit(EXIT_FAILURE); }
  if ((chdir("/tmp/")) < 0) { syslog(LOG_ERR, "Failed to change root directory! Exiting...\n"); exit(EXIT_FAILURE); }
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // buffer_add("Testing 123");
  // buffer_
  // ^ The hell was that for? ^ 
  while(1){  //We don't want the daemon to exit, so we create an infinite loop
    sleep(60); //If we don't wait a it before looping, we will be constantly eating up CPU time!
  }
}
