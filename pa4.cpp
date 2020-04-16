#include "web.h"

/*
 * COMP2011 (Fall 2019) Assignment 4: The Web System
 * File: pa4.cpp
 *
 * Student name: Steven Indrayana Istimur
 * Student ID: 20554526
 * Student email: siistimur@connect.ust.hk
 *
 * Note: This is for your implementation.
 * Please first fill your information above.
 * Feel free to add your helper functions in pa4.cpp.
 * There is no need to change other files.
 */

class wn_stack {
private:
    const WebNode* data[50]; // Use an array to store data
    int top_index; // Starts from 0; -1 when empty
public:
    // CONSTRUCTOR member functions
    wn_stack(){

        top_index = -1;
    }

    // DESTRUCTOR
    ~wn_stack(){
        while (!empty()){
            data[top_index] = nullptr;
            --top_index;
        }
    }

    // ACCESSOR member functions: const => won’t modify data members
    bool empty() const {
        return (top_index == -1);
    }

    const WebNode* top() const {
        if (!empty())
            return data[top_index];
    }


    // MUTATOR member functions
    void push(const WebNode* x){
        data[++top_index] = x;
    }
    void pop(){
        if (!empty()){
            --top_index;
        }
    }
};





const WebNode* pathfinder (const WebNode* web_node, wn_stack* backup){
    if (web_node->numOfHyperlinks() > 0){
        for (int j = web_node->numOfHyperlinks() - 1; j > 0; --j) {
            backup->push(web_node->hyperlink()[j]);
        }

        web_node = web_node->hyperlink()[0];

    }

    else {
        if (!backup->empty()){
            web_node = backup->top();
            backup->pop();
        }
        else
            return nullptr;
    }
    return web_node;
}



const WebNode* check (const WebNode* web_node, const char* webAddress, wn_stack* backup){
    if (web_node == nullptr)
        return nullptr;

    for (int i = 0; i < web_node->numOfHyperlinks(); ++i) {

        if (!strcmp(web_node->hyperlink()[i]->webAddress(), webAddress)){
            return web_node;
        }
    }
    return check(pathfinder(web_node, backup), webAddress, backup);
}


WebNode::WebNode(const char* webAddress, const char* anchorText, int height){
    m_webAddress = new char [strlen(webAddress) + 1];
    strcpy(m_webAddress, webAddress);

    m_anchorText = new char [strlen(anchorText) + 1];
    strcpy(m_anchorText, anchorText);

    m_hyperlink = nullptr;
    m_numOfHyperlinks = 0;

    if (isHTMLfile(webAddress) && height > 1) {
        char **anchor_tag = nullptr;
        extractAllAnchorTags(webAddress, anchor_tag, m_numOfHyperlinks);

        if (m_numOfHyperlinks > 0)
            m_hyperlink = new WebNode *[m_numOfHyperlinks];

        for (int i = 0; i < m_numOfHyperlinks; ++i) {
            const char *web_address = getWebAddress(anchor_tag[i]);
            const char *anchor_text = getAnchorText(anchor_tag[i]);

            m_hyperlink[i] = new WebNode(web_address, anchor_text, height - 1);

            delete[] web_address;
            web_address = nullptr;

            delete[] anchor_text;
            anchor_text = nullptr;
        }

        for (int i = 0; i < m_numOfHyperlinks; ++i) {
            delete[] anchor_tag[i];
            anchor_tag[i] = nullptr;
        }

        delete[] anchor_tag;
        anchor_tag = nullptr;
    }
}



WebNode::~WebNode(){
    if (m_hyperlink != nullptr) {
        for (int i = m_numOfHyperlinks-1; i >= 0; --i) {
            delete m_hyperlink[i];
            m_hyperlink[i] = nullptr;
        }

    }


    delete[] m_webAddress;
    m_webAddress = nullptr;

    delete[] m_anchorText;
    m_anchorText = nullptr;


        delete[] m_hyperlink;
        m_hyperlink = nullptr;

}

void WebNode::printGraph(int depth) const{
    cout << m_anchorText << " (url:" << m_webAddress << ")" << endl;

    //for printing a hyperlink of a webNote
    for (int j = 0; j < m_numOfHyperlinks; ++j) {

        //for printing tab
        for (int k = 0; k <= depth; ++k) {
            cout << '\t';
        }
        m_hyperlink[j]->printGraph(depth + 1);
    }
}






const WebNode* WebNode::findParent(const char* webAddress) const {
    if (webAddress == nullptr)
        return nullptr;

    if (!strcmp(webAddress, m_webAddress))
        return nullptr;


    wn_stack* backup = new wn_stack();
    const WebNode* par = check(this, webAddress, backup);

    delete backup;
    backup = nullptr;

    return par;

}


void WebNode::printPath(const char* address1, const char* address2) const{
    if (address1 == nullptr || address2 == nullptr)
        return;

    if (!strcmp(address1, address2)){
        cout << address1;
        return;
    }
    wn_stack* before = new wn_stack();

    const char* children = address2;

    do {
        before->push(findParent(children));
        if (before->top() != nullptr)
            children = before->top()->webAddress();
        else
            break;
    } while (strcmp(children, address1));

    if (before->top() == nullptr){
        cout << "Path not found" << endl;
        delete before;
        before = nullptr;
        return;
    }

    while (!before->empty()){
        cout << before->top()->webAddress() << " ";
        before->pop();
    }
    cout << address2;

    delete before;
    before = nullptr;

}