#include <queue>
#include <string>
#include <pthread.h>
#include <malloc.h>

using namespace std;

class CMsgQueue{
    private :
        queue<string *> q;
        int cap;
        pthread_mutex_t lock;
        pthread_cond_t cond;
        int size;
    public:
        CMsgQueue(){
            pthread_mutex_init(&lock,NULL);
            pthread_cond_init(&cond,NULL);
        }   
        ~CMsgQueue(){
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);

        }   
        void Push(string data);
        string Pop();
        bool Empty();
        bool Ready();
        int Size();

};
