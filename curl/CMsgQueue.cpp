#include "CMsgQueue.h"

bool CMsgQueue::Empty() {
    return size == 0 ? true:false ;
}
bool CMsgQueue::Ready() {  // if not read bolock
    pthread_mutex_lock(&lock);
    while(size == 0 ){
        pthread_cond_wait(&cond,&lock);
    }
    pthread_mutex_unlock(&lock);
    return true;
}
int CMsgQueue::Size() {
    return size;
}
void CMsgQueue::Push(string  data) {
    pthread_mutex_lock(&lock);
    string * tmp = new string(data);
    q.push(tmp);
    size++;
    pthread_cond_broadcast(&cond);                                                                                                                                                                                                                                           
    pthread_mutex_unlock(&lock);
}
string CMsgQueue::Pop() {
    string * tmp;
    pthread_mutex_lock(&lock);
    while(size == 0 ){
        pthread_cond_wait(&cond,&lock);
    }
    tmp = q.front();
    q.pop();
    size--;
    if(q.empty()) {  // incase memory leak
        queue<string*> emptyq;
        swap(emptyq,q);
        malloc_trim(0);
    }

    pthread_mutex_unlock(&lock);
    string ret(*tmp);
    delete tmp;
    tmp = NULL;
    return ret;
}

