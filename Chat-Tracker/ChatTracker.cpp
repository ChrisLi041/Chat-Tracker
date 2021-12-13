#include "ChatTracker.h"
#include <string>
#include <list>
#include <vector>
#include <functional>
#include<iostream>
using namespace std;


class ChatTrackerImpl
{
public:
    ChatTrackerImpl(int maxBuckets);
    void join(string user, string chat);
    int terminate(string chat);
    int contribute(string user);
    int leave(string user, string chat);
    int leave(string user);

private:
    struct Info
    {
        Info(string u, string c) : user(u), chat(c), count(0) {}
        string user;
        string chat;
        int count;

    };
    
    vector <list<Info>> m_info; //store the info about all the users;
    vector <list<Info>> m_chat; //store the contribution of all chats;
    
    int getHash(string toHash)
    {
        hash<string> hashed;
        return hashed(toHash) % 20000;
    }
    
};

ChatTrackerImpl::ChatTrackerImpl(int maxBuckets)
{
    // Since this implementation does not use a hash table, we don't do
    // anything with the parameter and leave it unnamed so the compiler
    // doesn't issue a warning about an unused parameter.
    m_info.resize(maxBuckets);
    m_chat.resize(maxBuckets);
}


void ChatTrackerImpl::join(string user, string chat)
{
    //get the user string hashed;
    int hashUser = getHash(user);
    //loop from the end;
    list<Info>::iterator p = m_info[hashUser].end();
    //loop till the beginning;
    while (p != m_info[hashUser].begin())
    {
        p--;
        if (p->user == user && p->chat == chat)
        {
            //if user has joined this chat, make it the user's current chat;
            Info info = *p;
            m_info[hashUser].erase(p);
            m_info[hashUser].push_back(info);
            //now it is the user's current chat'
            return;
        }
    }
    // if the user is not in that chat;
    m_info[hashUser].push_back(Info(user,chat));
    //now the user is in that chat'
}

int ChatTrackerImpl::terminate(string chat)
{
    //count the total contribution;
    int sumContribution = 0;
    //loop through the m_info bucket;
    vector<list<Info>>::iterator p = m_info.end();
    while(p != m_info.begin())
    {
        p--;
        //loop through each list in that bucket;
        list<Info>::iterator r = p->end();
        while (r != p->begin())
        {
            r--;
            //find user with that chat;
            if (r->chat == chat)
            {
                //add the user's contribution to the total;
                sumContribution = sumContribution + r->count;
                //erase the user;
                r = p->erase(r);
            }
        }
    }
    
    //get the chat string hased;
    int hashChat = getHash(chat);
    
    //loop through the chat bucket;
    list<Info>::iterator q = m_chat[hashChat].end();
    while (q != m_chat[hashChat].begin())
    {
        q--;
        if (q->chat == chat)
        {
            //add the chat's current contribution to the total;
            sumContribution = sumContribution + q->count;
            //erase the chat;
            m_chat[hashChat].erase(q);
            //return the chat's total contribution;
            return sumContribution;
        }
    }
    return sumContribution;
}

int ChatTrackerImpl::contribute(string user)
{
    //get user string hashed;
    int hashUser = getHash(user);
    //find the current chat that the user is in;
    //starting from the end;
    list<Info>::iterator p = m_info[hashUser].end();
    while (p != m_info[hashUser].begin())
    {
        p--;
        if (p->user == user)
        {
            p->count++;
            return p->count;
        }
    }
    //if there is no current chat available;
    return 0;
}


int ChatTrackerImpl::leave(string user, string chat)
{
    //get the user string hashed;
    int hashUser = getHash(user);
    //get the chat string hashed;
    int hashChat = getHash(chat);
    
    int userContribution = -1; //count the user's contribution to the chat;
    list<Info>::iterator p = m_info[hashUser].end();
    //loop through the m_info to find the user and the chat;
    while (p != m_info[hashUser].begin())
    {
        p--;
        //if the user is in that chat;
        if (p->user == user && p->chat == chat)
        {
            //add the the user's contribution to the total;
            userContribution = p->count;
            //erase the user;
            m_info[hashUser].erase(p);
            //now the user is in that chat and left, update the m_chat;
            //loop through the chat buckets;
            list<Info>::iterator q = m_chat[hashChat].end();
            while (q != m_chat[hashChat].begin())
            {
                q--;
                //check if the chat is already in the chat table;
                if (q->chat == chat)
                {
                    q->count = q->count + userContribution;
                    return userContribution;
                }
            }
            Info leftUser = Info(user, chat);
            leftUser.count = userContribution;
            m_chat[hashChat].push_back(leftUser);
            break;
        }
    }
    //not in the chat table
    return userContribution;
}

int ChatTrackerImpl::leave(string user)
{
    //get the user string hashed;
    int hashUser = getHash(user);
    //check if the user has a current chat;
    bool hasChat = false;
    //initialize the user's chat;
    string currentChat;
    //initialize the user's contribution;
    int currentContribution = -1;
    
    //loop through the info list;
    list<Info>::iterator p = m_info[hashUser].end();
    while (p != m_info[hashUser].begin())
    {
        p--;
        //find the user if the user exist;
        if (p->user == user)
        {
            //the user has a current chat;
            hasChat = true;
            //pass in the user's contribution to the current chat;
            currentContribution = p->count;
            //pass in the user's current chat name;
            currentChat = p->chat;
            m_info[hashUser].erase(p);
            break;
        }
    }
    
    if (hasChat == false) //user doesnt have a current chat;
    {
        return -1; //return -1;
    }
    //if the user leave a chat where he has contribution, update chat list;
    else
    {
        //get the chat string hashed;
        int hashChat = getHash(currentChat);
        //loop through the chat list;
        list<Info>::iterator q = m_chat[hashChat].end();
        while (q != m_chat[hashChat].begin())
        {
            q--;
            //find the chat that the user just left;
            if (q->chat == currentChat)
            {
                q->count = q->count + currentContribution;
                return currentContribution;
            }
        }
        Info leftUser = Info(user, currentChat);
        leftUser.count = currentContribution;
        m_chat[hashChat].push_back(leftUser);
    }
    //if this is a chat that is left for the first time;
    return currentContribution;
}

//*********** ChatTracker functions **************

// These functions simply delegate to ChatTrackerImpl's functions.
// You probably don't want to change any of this code.

ChatTracker::ChatTracker(int maxBuckets)
{
    m_impl = new ChatTrackerImpl(maxBuckets);
}

ChatTracker::~ChatTracker()
{
    delete m_impl;
}

void ChatTracker::join(string user, string chat)
{
    m_impl->join(user, chat);
}

int ChatTracker::terminate(string chat)
{
    return m_impl->terminate(chat);
}

int ChatTracker::contribute(string user)
{
    return m_impl->contribute(user);
}

int ChatTracker::leave(string user, string chat)
{
    return m_impl->leave(user, chat);
}

int ChatTracker::leave(string user)
{
    return m_impl->leave(user);
}

