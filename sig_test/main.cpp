#include<iostream>
#include<signal.h>
#include<stdlib.h>

using namespace std;

void doSigAction(int i){
  signal(SIGUSR1,doSigAction);
  cout << "I recived a signal!\n";
  exit(0);
}

int main(){
  signal(SIGUSR1, doSigAction);
  while(1);
}
