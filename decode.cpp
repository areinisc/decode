//Alex Reinisch || Nick Walker Decode 24/4/2012
//This program was written to decode bitstreams

#include "LinkedBinaryTree.h"
#include "BitStreams.h"
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

int end;
char character;
string file, message, answer;
LinkedBinaryTree<int> huffman;
InBitStream stream;
typedef LinkedBinaryTree<int>::Position Position;

int treeEvaluator(LinkedBinaryTree<int>& tree, int rating) {
    if (rating == 1) {
        cout << "Tree is unworthy" << endl;
    }
    else {
        cout << "Are you proud of your tree?" << endl;
        cin >> answer;
        cout << "Well, you shouldn't be." << endl;
    }
    return 42;
}

void buildTree(Position p) {                                 // This function aims to build a huffman tree
    if (stream.read() == 1) *p = stream.read(9);             // if a 1 is found, use the next 9 bits as a node
    else {
        huffman.expandExternal(p);                           // generates children
        buildTree(p.left());                                 // calls the build on the left child
        buildTree(p.right());                                // calls the build on the right child
    }
}

char decode(Position pos) {                                  // Lets decode the ASCII in the tree
    while (!pos.isExternal()) {                              // while not a leaf
        if (stream.read() == 0)
            pos = pos.left();                                // go left for each 0
        else                                                 // if (stream.read == 1)
            pos = pos.right();                               // go right for each 1
    }
    if (*pos == 256)                                         // handling for the EOF character
        return '#';
    else {
        return (char) *pos;                                  // handling for all other characters
    }
}

int main() {
    huffman.addRoot();                                       // put a root on our tree
    Position position = huffman.root();                      // set our position "iterator" at huffman's root
    cout << "Name of file: ";
    cin >> file;
    stream.open(file);
    buildTree(position);                                     // build the huffman tree
    while (!stream.eof()){                                   // before the end of the input
        character=decode(huffman.root());                    // decode the tree
        if (character!='#')                                  // before the EOF character
            message+=character;                              // append the decoded character to the message
        else {
            while (!stream.eof()){
                stream.read();
            }
        }
    }
    cout << message << endl;
    return 0;
}
